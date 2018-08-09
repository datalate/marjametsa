// Base class for every character, derived from Entity

#ifndef CHARACTER_H
#define CHARACTER_H

#include "entity.h"

#include <QPainter>

class Character : public Entity {
    Q_OBJECT
signals:
    void healthChanged(Character *c, int health);
    void killed(Character *c);
    void weaponChanged(Character *c, QString weapon);

public:
    explicit Character(int x, int y, int health);

    virtual void move(int x, int y);
    virtual QString name() const = 0;

    int maxHealth() const;
    int health() const;
    void decreaseHealth(int amount);
    void resetHealth();
    bool alive() const;

    // Must be set before updating sprite, calls updateSprite
    void setSpriteSheet(const QImage &spriteSheet);

private:
    // Checks the rotation and updates image, call setSpriteSheet first
    void updateSprite();

    QImage image_;
    QPainter painter_;
    const QImage *spriteSheet_; // Non-owning pointer

    int dirX_; // -1 turned to west, 1 turned to east
    int dirY_; // -1 turned to south, 1 turned to north

    int maxHealth_;
    int health_;
};

#endif // CHARACTER_H
