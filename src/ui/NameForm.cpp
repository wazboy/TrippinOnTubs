#include "NameForm.h"

trippin::NameForm::NameForm(const Point<int> &windowSize, SpriteManager &spriteManager) :
        windowSize(windowSize), sprite(spriteManager.get("alpha")), cursor(0) {
    name.resize(5);
}

void trippin::NameForm::reset() {
    cursor = 0;
}

void trippin::NameForm::render() {
    auto size = sprite.getSize();
    int padding = size.x / 8;
    int width = windowSize.x - ((size.x * columns) + (padding * (columns - 1)));
    int height = windowSize.y - ((size.y * (rows + 1)) + (padding * rows));
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            int digit = r * columns + c;
            if (digit < chars) {
                Point<int> pos{width / 2 + c * (size.x + padding), height / 2 + r * (size.y + padding)};
                sprite.render(pos, digit + 10);
            }
        }
    }

    int margin = size.x / (nameLength - 1);
    width = windowSize.x - (size.x * (nameLength + 1));
    for (int c = 0; c < nameLength; c++) {
        Point<int> pos{width / 2 + c * size.x + c * margin, height / 2 + rows * (size.y + padding)};
        if (c < cursor) {
            sprite.render(pos, 37);
            sprite.render(pos, name[c] - 'A' + 10);
        } else if (c == cursor) {
            sprite.render(pos, 36);
        } else {
            sprite.render(pos, 37);
        }
    }
}

void trippin::NameForm::onClick(const Point<int> &coords) {
    Point<int> size = sprite.getSize();
    int width = windowSize.x - (size.x * columns);
    int height = windowSize.y - (size.y * (rows + 1));

    Point<int> corner{width / 2, height / 2};
    Point<int> offset = coords - corner;
    int row = offset.y / size.y;
    int col = offset.x / size.x;
    if (row >= 0 && row < rows && col >= 0 && col < columns) {
        int digit = row * columns + col;
        name[cursor] = static_cast<char>(digit + 'A');
        cursor++;
    }
}

bool trippin::NameForm::nameEntered() const {
    return cursor == nameLength;
}

const std::string &trippin::NameForm::getName() const {
    return name;
}
