// Class used for player characters, derived from Character-class

#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"

class Player : public Character {
public:
    Player();

    virtual bool clickable() { return false; }

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

#endif // PLAYER_H
