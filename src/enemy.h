// Class used for enemy characters, derived from Character-class

#ifndef ENEMY_H
#define ENEMY_H

#include "character.h"
#include "common.h"

class Enemy : public Character {
public:
    explicit Enemy(int x, int y, int health = HEALTH_ENEMY);

    virtual bool clickable() { return true; }

    void setWeapon(const QString &weapon);
    virtual QString name() const;
    QString weapon() const;

private:
    static const QImage KNIFE_SPRITESHEET;
    static const QImage PISTOL_SPRITESHEET;
    static const QImage SMG_SPRITESHEET;
    static const QImage SHOTGUN_SPRITESHEET;
    static const QImage RIFLE_SPRITESHEET;

    QString currentWeapon_;
};

#endif // ENEMY_H
