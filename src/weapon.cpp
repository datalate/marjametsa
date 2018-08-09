#include "weapon.h"

#include <QDebug>

Weapon::Weapon(const QString& name, int range, int damage, int falloff,
        int dice, const QString& readableName) :
    name_{name},
    range_{range},
    damage_{damage},
    falloff_{falloff},
    dice_{dice},
    readableName_{readableName},
    rand_{}
{
    std::random_device r;
    // Seed rand from a true random device, if possible
    rand_.seed(r());
}

int Weapon::rollDamage(double range_sq) const {
    // This is the damage algorithm:
    // - First, we roll all dice (ie. damage: 3 and dice: 1 for the pistol means 1d3)
    // - Then we calculate range/range_ to find the falloff coefficient, which is truncated
    // - Finally, we reduce falloff * coefficient damage
    // This means that falloff means the damage redruced when at the very last square of range
    
    int result{0};
    
    // This is a die
    std::uniform_int_distribution<int> d{1, damage_};

    // Roll
    for (int i{0}; i < dice_; ++i) {
        int roll = d(rand_);
        result += roll;
    }

    // Parabolic decay
    int coeff(range_sq / (range_ * range_));

    result -= coeff * falloff_;

    // Don't allow negative damage
    if (result < 0) {
        result = 0;
    }

    return result;
}
