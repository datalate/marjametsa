#include "enemy.h"

const QImage Enemy::KNIFE_SPRITESHEET{"../gfx/knife.png"};
const QImage Enemy::PISTOL_SPRITESHEET{"../gfx/pistol.png"};
const QImage Enemy::SMG_SPRITESHEET{"../gfx/smg.png"};
const QImage Enemy::SHOTGUN_SPRITESHEET{"../gfx/shotgun.png"};
const QImage Enemy::RIFLE_SPRITESHEET{"../gfx/rifle.png"};

Enemy::Enemy(int x, int y, int health) :
    Character{x, y, health} {

    // Randomize weapon
    const int randInt{rand() % 100};
    if (randInt > 30) {
        setWeapon(WEAPON_KNIFE);
    }
    else if (randInt > 6) {
        setWeapon(WEAPON_PISTOL);
    }
    else if (randInt > 3) {
        setWeapon(WEAPON_SHOTGUN);
    }
    else {
        setWeapon(WEAPON_SMG);
    }

    setAcceptedMouseButtons(Qt::LeftButton);
    setFlags(QGraphicsItem::ItemIsSelectable);
}

void Enemy::setWeapon(const QString &weapon) {
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
}

QString Enemy::name() const {
    return "Vihu";
}

QString Enemy::weapon() const {
    return currentWeapon_;
}
