// Contains the loaded mission data, populated and used by MissionParser

#ifndef MISSION_H
#define MISSION_H

#include <QString>
#include <QMap>

class MissionParser;

class Mission
{
    friend MissionParser; 
public:
    Mission(const QString& name);

    const QString &name() const { return name_; }
    int value(const QString &key) const { return data_.value(key); }
    
private:
    QString name_;
    QMap<QString, int> data_;
};

#endif // MISSION_H
