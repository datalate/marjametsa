#include "missionparser.h"
#include <QDebug>
#include <QDir>
#include <QFile>

MissionParser::MissionParser() :
    loaded_{false}
{
}

bool MissionParser::loadFolder(const QString &missionFolder) {
    const QDir folder{missionFolder};
    qDebug() << "MissionParser::loadFolder - Loading missions from"
             << folder.absolutePath();

    // Get all user-visible normal files
    foreach (const QString &file,
             folder.entryList(QDir::Files | QDir::NoDotAndDotDot)) {
        loadMission(folder.absoluteFilePath(file));
    }

    loaded_ = true;
    return true;
}

const Mission &MissionParser::mission(const QString &name) const {
    return *missions_.find(name);
}

bool MissionParser::loadMission(const QString &fileName) {
    QFile file{fileName};

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Mission mission(QFileInfo{file}.fileName());

        QTextStream in{&file};
        while (!in.atEnd()) {
            const QString row = in.readLine();

            auto parts = row.split(":");
            // Get key from line
            auto key = parts[0].simplified();
            auto value = parts[1].simplified().toInt();

            mission.data_.insert(key, value);
        }

        missions_.insert(mission.name(), mission);

        qDebug() << "Mission loaded:" << fileName;
    }


    return true;
}
