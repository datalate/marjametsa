#include "character.h"

Character::Character(int x, int y, int health) :
    Entity{x, y},
    image_{64, 96, QImage::Format_ARGB32},
    painter_{&image_},
    spriteSheet_{nullptr},
    maxHealth_{health}, health_{health} {

    // Randomize starting direction
    dirX_ = rand() % 3 - 1;
    if (dirX_ == 0) {
        dirY_ = rand() % 3 - 1;
    }

    setZValue(pos().y() + 1);
}

void Character::move(int x, int y) {
    dirX_ = x - this->x();
    dirY_ = y - this->y();
    Entity::move(x, y);

    if (spriteSheet_ != nullptr) {
        updateSprite();
    }
}

int Character::maxHealth() const {
    return maxHealth_;
}

int Character::health() const {
    return health_;
}

void Character::decreaseHealth(int amount) {
    health_ -= amount;
    if (health_ < 0) {
        health_ = 0;
    }

    // Signal health change and possibly death
    emit healthChanged(this, health_);

    if (health_ == 0) {
        emit killed(this);
    }
}

void Character::resetHealth() {
    // When we get out of parking hall
    health_ = maxHealth_;

    emit healthChanged(this, health_);
}

bool Character::alive() const {
    return (health_ > 0);
}

void Character::setSpriteSheet(const QImage &spriteSheet) {
    spriteSheet_ = &spriteSheet;
    updateSprite();
    setOffset(pixmap().width() / 2, -48);
}

void Character::updateSprite() {
    // TODO: Do not hardcode sprites
    // TODO: Animation
    // TODO: Some kinda "PlayAnimation" method

    Q_ASSERT(spriteSheet_ != nullptr);

    // Update sprite depending on character direction
    image_.fill(Qt::transparent);
    if (dirX_ > 0) {
        painter_.drawImage(0, 0, *spriteSheet_, 0, 3 * 96, 64, 96);
    }
    else if (dirX_ < 0) {
        painter_.drawImage(0, 0, *spriteSheet_, 0, 0, 64, 96);
    }
    else if (dirY_ > 0) {
        painter_.drawImage(0, 0, *spriteSheet_, 0, 96, 64, 96);
    }
    else {
        painter_.drawImage(0, 0, *spriteSheet_, 0, 2 * 96, 64, 96);
    }
    setPixmap(QPixmap::fromImage(image_));

    setZValue(pos().y() + 1);
}
