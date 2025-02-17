#include "ScrollingScoreBoard.h"
#include "engine/Convert.h"

trippin::ScrollingScoreBoard::ScrollingScoreBoard(
        const Point<int> &windowSize,
        double scrollRate,
        SpriteManager &spriteManager) :
        windowSize(windowSize),
        scrollRate(scrollRate),
        scoreBoard(spriteManager.get("alpha")) {

}

void trippin::ScrollingScoreBoard::setScores(std::vector<Score> scores) {
    scoreBoard.setScores(std::move(scores));
}

void trippin::ScrollingScoreBoard::reset() {
    startTime = SDL_GetTicks();
}

void trippin::ScrollingScoreBoard::render() {
    int delta = static_cast<int>(SDL_GetTicks() - startTime);
    scrollTop = toInt(delta * scrollRate);

    int scrollWrap = scrollTop % (windowSize.y + scoreBoard.getHeight());
    Point<int> position;
    position.x = (windowSize.x - scoreBoard.getWidth()) / 2;
    position.y = scrollWrap + windowSize.y;
    scoreBoard.render(position);
}