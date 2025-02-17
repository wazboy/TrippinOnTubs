#ifndef TRIPPIN_TITLEMENU_H
#define TRIPPIN_TITLEMENU_H

#include "sprite/SpriteManager.h"
#include "ui/MenuLayout.h"

namespace trippin {
    class TitleMenu {
    public:
        TitleMenu(const Point<int> &windowSize, SpriteManager &spriteManager);
        void reset();
        void render();
        bool startClicked(const Point<int> &coords) const;
        bool trainClicked(const Point<int> &coords) const;
        bool exitClicked(const Point<int> &coords) const;
        bool highScoreClicked(const Point<int> &coords) const;
    private:
        MenuLayout<4> menuLayout;
    };
}

#endif
