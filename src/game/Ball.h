#ifndef TRIPPIN_BALL_H
#define TRIPPIN_BALL_H

#include "SpriteObject.h"
#include "Activation.h"
#include "engine/ReflectiveCollision.h"
#include "lock/Guarded.h"

namespace trippin {
    class Ball : public SpriteObject {
    public:
        void init(const Configuration &config, const Map::Object &obj, const Sprite &spr) override;
        void beforeTick(Uint32 engineTicks) override;
        void afterTick(Uint32 engineTicks) override;
        void render(const Camera &camera) override;
        void setActivation(const Activation *activation);
        void setUniverse(Point<int> universe);
    private:
        ReflectiveCollision reflectiveCollision;
        const Activation *activation;
        Rect<int> universe;

        struct Channel {
            Point<int> roundedPosition;
            int frame;
        };
        Guarded<Channel> channel;
    };
}

#endif
