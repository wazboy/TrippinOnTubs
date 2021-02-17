#ifndef TRIPPIN_RUNNINGCLOCK_H
#define TRIPPIN_RUNNINGCLOCK_H

#include "SpriteObject.h"
#include "Activation.h"
#include "Score.h"
#include "Spirit.h"
#include "lock/Guarded.h"

namespace trippin {
    class RunningClock : public SpriteObject {
    public:
        void init(const Configuration &config, const Map::Object &obj, const Sprite &spr) override;
        void setGoggin(const Goggin *goggin);
        void setSpirit(Spirit *spirit);
        void beforeTick(Uint32 engineTicks) override;
        void afterTick(Uint32 engineTicks) override;
        void render(const Camera &camera) override;
        void setActivation(const Activation *activation);
        void setScore(Score *score);
        void setSoundManager(SoundManager &soundManager);
    private:
        constexpr static const int FRAME_CLOUD_FIRST = 24;

        const Goggin *goggin;
        Spirit *spirit;
        int hitTicks;
        bool hitGoggin;

        double runningAcceleration;
        const Activation *activation;
        Score *score;
        SoundManager *soundManager;

        Mix_Chunk *sound;
        bool playedSound;

        struct Channel {
            Point<int> roundedPosition;
            int frame;
        };
        Guarded<Channel> channel;
    };
}

#endif
