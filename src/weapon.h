// Contains the loaded weapon data and calculates damage values
// Populated and used by WeaponParser

#ifndef WEAPON_H
#define WEAPON_H

#include <QString>
#include <QMap>
#include <QObject>

#include <random>

class WeaponParser;

class Weapon {
public:
    friend WeaponParser;

    Weapon(const QString& name, int range, int damage, int falloff, int dice,
           const QString& readableName);

    QString name() const { return name_; }
    QString readableName() const { return QObject::tr(qPrintable(readableName_)); }
    int range() const { return range_; }

     // Calculates the damage based on distance
    int rollDamage(double range_sq) const;

private:
    QString name_;
    int range_;
    int damage_;
    int falloff_;
    int dice_;
    QString readableName_;
    mutable std::mt19937 rand_;

};

#endif // WEAPON_H
