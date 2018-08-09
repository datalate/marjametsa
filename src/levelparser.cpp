#include "levelparser.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include "common.h"

const QString LevelParser::ALLOWED_CHARS{QString() +
    TILE_GROUND + TILE_PILLAR + TILE_WALL_CORNER +
    TILE_WALL_SOUTHFACING + TILE_WALL_WESTFACING + TILE_WALL_INVISIBLE +
    CHARACTER_PLAYER + CHARACTER_ENEMY + CHARACTER_NPC + RESOURCE_SPAWNER
};

LevelParser::LevelParser() :
    loaded_{false} {

}

bool LevelParser::loadFolder(const QString &levelFolder) {
    const QDir folder{levelFolder};
    qDebug() << "LevelParser::loadFolder - Loading levels from"
             << folder.absolutePath();

    // Get all user-visible normal files
    foreach (const QString &file,
             folder.entryList(QDir::Files | QDir::NoDotAndDotDot)) {
        loadLevel(folder.absoluteFilePath(file));
    }

    loaded_ = true;
    return true;
}

bool LevelParser::exists(const QString &name) const {
    return levels_.contains(name);
}

const LevelParser::Level &LevelParser::level(const QString &name) const {
    return *levels_.find(name);
}

QStringList LevelParser::names() const {
    return levels_.keys();
}

int LevelParser::count() const {
    return levels_.size();
}

bool LevelParser::loadLevel(const QString &fileName) {
    QFile file{fileName};

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Level level(QFileInfo{file}.fileName()); // Needed to get the actual file name
        bool hasPlayerSpawn{false};
        int npcSpawnCount{0};

        QTextStream in{&file};
        while (!in.atEnd()) {
            const QString row = in.readLine();

            if (!level.empty() && row.length() != level.width()) {
                qDebug() << "LevelParser::loadLevel - Invalid level:" << fileName;

                return false;
            }

            foreach (QChar c, row) {
                if (!ALLOWED_CHARS.contains(c)) {
                    qDebug() << "LevelParser::loadLevel - Invalid character found:"
                             << c << "in" << fileName;

                    return false;
                }

                if (c == CHARACTER_PLAYER) {
                    if (hasPlayerSpawn) {
                        qDebug() << "LevelParser::loadLevel - Too many player spawns in"
                                 << fileName;

                        return false;
                    }

                    hasPlayerSpawn = true;
                }
                else if (c == CHARACTER_NPC) {
                    if (npcSpawnCount > 3) {
                        qDebug() << "LevelParser::loadLevel - Too many npc spawns in"
                                 << fileName;

                        return false;
                    }

                    ++npcSpawnCount;
                }
            }

            level.addLine(row);
        }

        if (!hasPlayerSpawn) {
            qDebug() << "LevelParser::loadLevel - Player spawn missing in"
                     << fileName;

            return false;
        }
        else if (npcSpawnCount != 3) {
            qDebug() << "LevelParser::loadLevel - Invalid number of npc spawns in"
                     << fileName;

            return false;
        }

        levels_.insert(level.name(), level);

        qDebug() << "LevelParser::loadLevel - Added map" << fileName
                 << "of size" << level.width() << "x" << level.height();
    }
    else {
        qDebug() << "LevelParser::loadLevel - Couldn't open level file:"
                 << fileName;

        return false;
    }

    return true;
}


LevelParser::Level::Level(const QString &name) :
    name_{name} {

}

void LevelParser::Level::addLine(const QString &line) {
    data_.push_back(line);
}

bool LevelParser::Level::empty() const {
    return data_.empty();
}

QString LevelParser::Level::name() const {
    return name_;
}

const QVector<QString> &LevelParser::Level::data() const {
    return data_;
}

int LevelParser::Level::width() const {
    if (empty()) {
        return 0;
    }
    else {
        return data_[0].size();
    }
}

int LevelParser::Level::height() const {
    if (empty()) {
        return 0;
    }
    else {
        return data_.size();
    }
}
