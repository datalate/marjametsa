#include "shopping.h"
#include <QDebug>

Shopping::Shopping(WeaponParser *weps) :
    cash_{0},
    weps_{weps},
    prices_{
        {"pistol", 100},
        {"smg", 250},
        {"shotgun", 500},
        {"rifle", 300} 
    }
{
}

void Shopping::addCash(int cash) {
    Q_ASSERT(cash >= 0);
    qDebug() << "Added" << cash << "cash";

    cash_ += cash;
    emit cashChanged(cash_);
}

void Shopping::buyWeapon(int slot, QString weapon){
    Q_ASSERT(prices_.find(weapon) != prices_.end());

    qDebug() << "Trying to buy" << weapon;
    int price{prices_.value(weapon)};
    if (cash_ >= price) {
        qDebug() << "Bought" << weapon;
        cash_ -= price;
        emit cashChanged(cash_);
        emit weaponBought(slot, weapon);
        emit weaponBoughtReadable(slot, weps_->weapon(weapon).readableName());
    }
}
