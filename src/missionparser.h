// Parses the mission folder and saves the data locally

#ifndef MISSIONPARSER_H
#define MISSIONPARSER_H

#include "mission.h"
#include <QMap>

class MissionParser
{
public:
    MissionParser();
    bool loadFolder(const QString &missionFolder);

    const Mission& mission(const QString& name) const;

private:
    QMap<QString, Mission> missions_;
    bool loadMission(const QString &fileName);
    bool loaded_;
};

#endif // MISSIONPARSER_H
