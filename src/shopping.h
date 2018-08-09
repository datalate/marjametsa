// Class used as the backend for the in-game weapons store. Keeps track of
// money, weapon prices and delegates weapon purchases to relevant front-ends
#ifndef SHOPPING_H
#define SHOPPING_H

#include <QObject>
#include <QHash>
#include <QVariant>
#include "weaponparser.h"

class Shopping : public QObject {
    Q_OBJECT
signals:
    void cashChanged(QVariant cash); // Emitted when cash amount changes
    void weaponBought(int slot, const QString& weapon); // When weapon was succesfully bought
    void weaponBoughtReadable(QVariant slot, QVariant readable); // Same, but for Finnish name
public:
    explicit Shopping(WeaponParser *weps);

    void addCash(int cash); // Add more cash as a reward

private slots:
    void buyWeapon(int slot, QString weapon); // Try to buy weapon

private:
    int cash_;
    WeaponParser *weps_;
    QHash<QString, int> prices_; // List of prices for weapons
};

#endif // SHOPPING_H
