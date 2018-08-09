// Parses the level folder and saves the data locally

#ifndef LEVELPARSER_H
#define LEVELPARSER_H

#include <QObject>
#include <QStringList>
#include <QVector>
#include <QMap>

class LevelParser {
public:
    class Level {
    public:
        explicit Level(const QString &name);

        bool empty() const;
        QString name() const;
        const QVector<QString> &data() const;

        int width() const; // Returns 0 if level empty
        int height() const; // Returns 0 if level empty

        void addLine(const QString &line);

    private:
        QString name_;
        QVector<QString> data_;
    };

    LevelParser();

    bool loadFolder(const QString &levelFolder);

    bool exists(const QString &name) const;
    const Level &level(const QString &name) const; // Level must exist
    QStringList names() const; // Returns the list of loaded levels
    int count() const; // Returns the level count

private:
    static const QString ALLOWED_CHARS;

    bool loadLevel(const QString &fileName);

    QMap<QString, Level> levels_;
    bool loaded_;
};

#endif // LEVELPARSER_H
