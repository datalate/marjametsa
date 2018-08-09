#include "player.h"

#include "common.h"

const QImage Player::KNIFE_SPRITESHEET{"../gfx/knife-teekkari-black.png"};
const QImage Player::PISTOL_SPRITESHEET{"../gfx/pistol-teekkari-black.png"};
const QImage Player::SMG_SPRITESHEET{"../gfx/smg-teekkari-black.png"};
const QImage Player::SHOTGUN_SPRITESHEET{"../gfx/shotgun-teekkari-black.png"};
const QImage Player::RIFLE_SPRITESHEET{"../gfx/rifle-teekkari-black.png"};

Player::Player() :
    Character{0, 0, HEALTH_PLAYER} {

    // Players start with a knife
    setWeapon(WEAPON_KNIFE);
}

void Player::setWeapon(const QString &weapon) {
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

QString Player::name() const {
    return "Pelaaja";
}

QString Player::weapon() const {
    return currentWeapon_;
}
