// Class used for NPC characters, derived from Character-class

#ifndef NPC_H
#define NPC_H

#include "character.h"

class Npc : public Character {
public:
    explicit Npc(int x, int y);

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

#endif // NPC_H
