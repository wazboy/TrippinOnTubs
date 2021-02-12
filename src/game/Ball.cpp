#include "Ball.h"
#include "lock/Exchange.h"

void trippin::Ball::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    SpriteObject::init(config, obj, spr);
    inactive = true;
    channel.ref() = {roundedPosition, 0};
    reflectiveCollision.setCoefficient(obj.coefficient);
    platformCollision.set(&reflectiveCollision);
}

void trippin::Ball::beforeTick(Uint32 engineTicks) {
    if (inactive && activation->shouldActivate(roundedBox)) {
        inactive = false;
    }
}

void trippin::Ball::afterTick(Uint32 engineTicks) {
    Exchange<Channel> ex{channel};
    auto &ch = ex.get();

    // early exit if not activated yet
    if (inactive) {
        return;
    }

    if (activation->shouldDeactivate(roundedBox)) {
        expired = true;
        return;
    }

    ch.roundedPosition = roundedPosition;
    if (engineTicks % sprite->getFramePeriodTicks() == 0) {
        ch.frame = (ch.frame + 1) % sprite->getFrames();
    }
}

void trippin::Ball::render(const trippin::Camera &camera) {
    auto ch = channel.get();
    sprite->render(ch.roundedPosition, ch.frame, camera);
}

void trippin::Ball::setActivation(const Activation *act) {
    activation = act;
}
