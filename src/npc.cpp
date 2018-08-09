#include "npc.h"

#include "common.h"

const QImage Npc::KNIFE_SPRITESHEET{"../gfx/knife-teekkari.png"};
const QImage Npc::PISTOL_SPRITESHEET{"../gfx/pistol-teekkari.png"};
const QImage Npc::SMG_SPRITESHEET{"../gfx/smg-teekkari.png"};
const QImage Npc::SHOTGUN_SPRITESHEET{"../gfx/shotgun-teekkari.png"};
const QImage Npc::RIFLE_SPRITESHEET{"../gfx/rifle-teekkari.png"};

Npc::Npc(int x, int y) :
    Character {x, y, 50} {

    // Npcs start with a knife
    setWeapon(WEAPON_KNIFE);
}

void Npc::setWeapon(const QString &weapon) {
    currentWeapon_ = weapon;

    // Update spritesheet to reflect weapon change
    if (weapon == WEAPON_KNIFE) {
        setSpriteSheet(KNIFE_SPRITESHEET);
    }
    else if (weapon == WEAPON_PISTOL) {
        setSpriteSheet(PISTOL_SPRITESHEET);
    }
    else if (weapon == WEAPON_SMG) {
        setSpriteSheet(SMG_SPRITESHEET);
    }
    else if (weapon == WEAPON_SHOTGUN) {
        setSpriteSheet(SHOTGUN_SPRITESHEET);
    }
    else if (weapon == WEAPON_RIFLE) {
        setSpriteSheet(RIFLE_SPRITESHEET);
    }

    emit weaponChanged(this, weapon);
}

QString Npc::name() const {
    return "Apuri";
}

QString Npc::weapon() const {
    return currentWeapon_;
}
