#ifndef TRIPPINONTUBS_SPRITETYPE_H
#define TRIPPINONTUBS_SPRITETYPE_H

namespace trippin {
    enum SpriteType {
        goggin,
        ground,
        ball
    };

    constexpr int numSprites = 3;

    constexpr SpriteType spriteTypes[] = {SpriteType::goggin, SpriteType::ground, SpriteType::ball};

    constexpr const char* getSpriteName(SpriteType type) {
        switch (type) {
            case SpriteType::goggin:
                return "goggin";
            case SpriteType::ground:
                return "ground";
            case SpriteType::ball:
                return "ball";
        }
    }
}

#endif
