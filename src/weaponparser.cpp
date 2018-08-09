#include "weaponparser.h"
#include <QDebug>
#include <QDir>
#include <QFile>

WeaponParser::WeaponParser() :
    loaded_{false}
{
}

bool WeaponParser::loadFolder(const QString &weaponFolder) {
    const QDir folder{weaponFolder};
    qDebug() << "WeaponParser::loadFolder - Loading weapons from"
             << folder.absolutePath();

    // Get all user-visible normal files
    foreach (const QString &file,
             folder.entryList(QDir::Files | QDir::NoDotAndDotDot)) {
        loadWeapon(folder.absoluteFilePath(file));
    }

    loaded_ = true;
    return true;
}

bool WeaponParser::exists(const QString &name) const {
    return weapons_.contains(name);
}

const Weapon &WeaponParser::weapon(const QString &name) const {
    return *weapons_.find(name);
}

QStringList WeaponParser::names() const {
    return weapons_.keys();
}

bool WeaponParser::loadWeapon(const QString &fileName) {
    QFile file{fileName};

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString name{QFileInfo{file}.fileName()};
        int damage{0};
        int range{0};
        int falloff{0};
        int dice{0};
        QString readable{};
        bool damage_loaded{false};
        bool range_loaded{false};
        bool falloff_loaded{false};
        bool dice_loaded{false};
        bool readable_loaded{false};

        QTextStream in{&file};
        while (!in.atEnd()) {
            const QString row{in.readLine()};

            auto parts = row.split(":");
            // Get key from line
            auto key = parts[0].simplified();
            auto value = parts[1].simplified();

            // We only have two properties and they're named
            // Rest will be ignored for now
            if (key == QString{"range"}) {
                range = value.toInt();
                range_loaded = true;
            }
            else if (key == QString{"damage"}) {
                damage = value.toInt();
                damage_loaded = true;
            }
            else if (key == QString{"falloff"}) {
                falloff = value.toInt();
                falloff_loaded = true;
            }
            else if (key == QString{"dice"}) {
                dice = value.toInt();
                dice_loaded = true;
            }
            else if (key == QString{"readable"}) {
                readable = value;
                readable_loaded = true;
            }
        }

        if (!damage_loaded) {
            qDebug() << "Warning: Weapon lacks damage value:"
                     << name;
            return false;
        }

        if (!range_loaded) {
            qDebug() << "Warning: Weapon lacks range value:"
                     << name;
            return false;
        }

        if (!falloff_loaded) {
            qDebug() << "Warning: Weapon lacks damage falloff value:"
                     << name;
            return false;
        }

        if (!dice_loaded) {
            qDebug() << "Warning: Weapon lacks damage dice count:"
                     << name;
            return false;
        }

        if (!readable_loaded) {
            qDebug() << "Warning: Weapon lacks human-readable name:"
                     << name;
            return false;
        }

        weapons_.insert(name, Weapon{name, range, damage, falloff, dice,
                                     readable});
        qDebug() << "Weapon loaded:" << name;

    }

    return true;
}
