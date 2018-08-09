// Parses the weapon folder and saves the data locally

#ifndef WEAPONPARSER_H
#define WEAPONPARSER_H

#include "weapon.h"
#include <QMap>

class WeaponParser
{
public:
    WeaponParser();
    bool loadFolder(const QString &weaponFolder);

    bool exists(const QString &name) const;
    const Weapon &weapon(const QString &name) const; // Weapon must exist
    QStringList names() const; // Returns the list of loaded weapons

private:
    QMap<QString, Weapon> weapons_; // Contains weapon data

    bool loadWeapon(const QString &fileName);
    bool loaded_;
};

#endif // WEAPONPARSER_H
