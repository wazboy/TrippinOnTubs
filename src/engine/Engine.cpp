#include "SDL.h"
#include "engine/Engine.h"
#include "engine/Clock.h"

void trippin::Engine::add(Object *obj) {
    auto &vec = obj->inactive ? inactive : (obj->platform ? platforms : objects);
    vec.push_back(obj);
}

void trippin::Engine::beforeTick(Uint32 engineTicks) {
    auto fn = [engineTicks](auto obj) { obj->beforeTick(engineTicks); };
    std::for_each(inactive.begin(), inactive.end(), fn);
    std::for_each(platforms.begin(), platforms.end(), fn);
    std::for_each(objects.begin(), objects.end(), fn);
    std::for_each(listeners.begin(), listeners.end(), fn);
}

void trippin::Engine::afterTick(Uint32 engineTicks) {
    auto fn = [engineTicks](auto obj) { obj->afterTick(engineTicks); };
    std::for_each(inactive.begin(), inactive.end(), fn);
    std::for_each(platforms.begin(), platforms.end(), fn);
    std::for_each(objects.begin(), objects.end(), fn);
    std::for_each(listeners.begin(), listeners.end(), fn);
}

void trippin::Engine::promoteActive() {
    std::for_each(inactive.begin(), inactive.end(), [this](Object *obj) {
        if (!obj->inactive) {
            auto &vec = obj->platform ? platforms : objects;
            vec.push_back(obj);
        }
    });

    auto it = std::remove_if(inactive.begin(), inactive.end(), [](Object *obj) { return !obj->inactive; });
    inactive.erase(it, inactive.end());
}

void trippin::Engine::removeExpired() {
    auto fn = [](Listener *listener) { return listener->isExpired(); };
    inactive.erase(std::remove_if(inactive.begin(), inactive.end(), fn), inactive.end());
    platforms.erase(std::remove_if(platforms.begin(), platforms.end(), fn), platforms.end());
    objects.erase(std::remove_if(objects.begin(), objects.end(), fn), objects.end());
    listeners.erase(std::remove_if(listeners.begin(), listeners.end(), fn), listeners.end());
}

void trippin::Engine::tick(Uint32 engineTicks) {
    // SDL_Log("inactive=%d, platforms=%d, objects=%d", inactive.size(), platforms.size(), objects.size());
    beforeTick(engineTicks);
    promoteActive();
    removeExpired();
    applyMotion();
    snapObjects();
    applyPhysics();
    afterTick(engineTicks);
}

void trippin::Engine::applyMotion() {
    std::for_each(objects.begin(), objects.end(), [](Object *obj) { obj->applyMotion(); });
}

void trippin::Engine::snapObjects() {
    // prepare for snapping
    for (auto obj : objects) {
        obj->snapCollisions.clear();
        obj->snappedToMe = false;
    }

    // first, snap objects to each platform
    for (auto plat : platforms) {
        snapToPlatform(plat);
    }

    // next, snap objects to each object in priority order
    Object *it;
    while ((it = nextObjectToSnap()) != nullptr) {
        snapToPlatform(it);
    }
}

trippin::Object *trippin::Engine::nextObjectToSnap() {
    Object *max = nullptr;
    for (auto obj : objects) {
        if (!obj->snappedToMe && (max == nullptr || hasHigherSnapPriorityThan(obj, max))) {
            max = obj;
        }
    }
    if (max != nullptr) {
        max->snappedToMe = true;
    }
    return max;
}

bool trippin::Engine::hasHigherSnapPriorityThan(Object *left, Object *right) {
    // object with most collision sides is highest priority
    auto lcnt = left->snapCollisions.count();
    auto rcnt = right->snapCollisions.count();
    if (lcnt != rcnt)
        return lcnt > rcnt;

    // object with highest y position (lower on screen) is next highest priority
    return left->position.y > right->position.y;
}

void trippin::Engine::snapToPlatform(Object *plat) {
    for (auto obj : objects) {
        if (!obj->snappedToMe) {
            if (sameLane(obj, plat)) {
                auto overlap = obj->roundedBox.intersect(plat->roundedBox);
                if (overlap) {
                    snapTo(*obj, *plat, overlap);
                }
                auto collision = obj->roundedBox.collision(plat->roundedBox);
                if (collision) {
                    obj->snapCollisions |= collision;
                }
            }
        }
    }
}

void trippin::Engine::snapTo(Object &obj, const Object &p, const Rect<int> &overlap) {
    auto x = 0.0;
    auto y = 0.0;

    // examine horizontal overlap - the overlap rect must align with left-right sides of the two objects
    // this check excludes interior collisions, for example a tall, skinny object that drops vertically in the
    // middle of a wide platform
    if (obj.roundedBox.leftAlignedWith(overlap) && p.roundedBox.rightAlignedWith(overlap)) {
        x = overlap.w;
    } else if (obj.roundedBox.rightAlignedWith(overlap) && p.roundedBox.leftAlignedWith(overlap)) {
        x = -overlap.w;
    }

    // examine vertical overlap - the overlap rect must align with top-bottom sides of the two objects
    // this check excludes interior collisions, for example a short, wide object that moves laterally in the
    // middle of a tall platform
    if (obj.roundedBox.topAlignedWith(overlap) && p.roundedBox.bottomAlignedWith(overlap)) {
        y = overlap.h;
    } else if (obj.roundedBox.bottomAlignedWith(overlap) && p.roundedBox.topAlignedWith(overlap)) {
        y = -overlap.h;
    }

    // use smallest displacement
    if (x != 0 && (y == 0 || (std::abs(x) < std::abs(y)))) {
        // dont snap back over previous overlap - oldest snap wins
        if ((x > 0 && !obj.snapCollisions.testRight()) || (x < 0 && !obj.snapCollisions.testLeft())) {
            obj.position.x += x;
            obj.syncPositions();
        }
    } else if (y != 0) {
        // same as above
        if ((y > 0 && !obj.snapCollisions.testBottom()) || (y < 0 && !obj.snapCollisions.testTop())) {
            obj.position.y += y;
            obj.syncPositions();
        }
    }
}

void trippin::Engine::applyPhysics() {
    for (auto platform : platforms) {
        for (auto object : objects) {
            if (sameLane(object, platform)) {
                auto collision = object->roundedBox.collision(platform->roundedBox);
                if (collision) {
                    applyPlatformCollision(*object, *platform, collision);
                }
            }
        }
    }

    for (int i = 0; i < objects.size(); i++) {
        auto a = objects[i];
        for (int j = i + 1; j < objects.size(); j++) {
            auto b = objects[j];
            if (sameLane(a, b)) {
                auto collision = a->roundedBox.collision(b->roundedBox);
                if (collision) {
                    applyObjectCollision(*a, *b, collision);
                }
            }
        }
    }
}

void trippin::Engine::applyPlatformCollision(Object &object, Object &platform, const Sides &sides) {
    auto collision = object.platformCollision.isPresent()
                     ? object.platformCollision.get()
                     : platformCollision;
    object.onPlatformCollision(platform, sides);
    collision->onCollision(object, platform, sides);
}

void trippin::Engine::applyObjectCollision(Object &left, Object &right, const Sides &sides) {
    left.objectCollisions |= sides;
    right.objectCollisions |= sides.flip();
    left.onObjectCollision(right, sides);
    right.onObjectCollision(left, sides.flip());
    objectCollision->onCollision(left, right, sides);
}

void trippin::Engine::setTickPeriod(int tp) {
    tickPeriod = tp;
}

int run(void *data) {
    auto engine = (trippin::Engine *) data;
    engine->runEngineLoop();
    return 0;
}

void trippin::Engine::runEngineLoop() {
    Clock clock{static_cast<Uint32>(tickPeriod)};
    while (!stopped) {
        if (!paused) {
            tick(clock.getTicks());
        }
        clock.next();
    }
}

void trippin::Engine::start() {
    thread = SDL_CreateThread(run, "Engine Thread", (void *) this);
}

void trippin::Engine::pause() {
    paused = true;
}

void trippin::Engine::stop() {
    stopped = true;
}

void trippin::Engine::setPlatformCollision(trippin::Collision *collision) {
    platformCollision = collision;
}

void trippin::Engine::setObjectCollision(trippin::Collision *collision) {
    objectCollision = collision;
}

void trippin::Engine::addListener(trippin::Listener *listener) {
    listeners.push_back(listener);
}