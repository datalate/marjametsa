// Handles the map scene

#ifndef TAMPERE_H
#define TAMPERE_H

#include <QObject>
#include <QString>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>

class TamperePark : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

signals:
    void clicked(const QString &name);

public:
    explicit TamperePark(int x, int y, const QString &name);
    QString name() const;

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *e);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);

private:
    QString name_;
};


class Tampere : public QObject {
    Q_OBJECT

public:
    explicit Tampere(QGraphicsScene &scene);
    void enterHerw();

    bool isParkSelected() const;
    QString selectedPark() const;

    void removePark(const QString &name);
    void gameWon();

private slots:
    void parkClicked(const QString &name);

private:
    void insertPark(int x, int y, const QString &name);

    // Should we own the scene or not? ATM non-owning pointer
    QGraphicsScene *const scene_;

    QMap<QString, TamperePark*> parks_;
    QString selectedPark_;
};

#endif // TAMPERE_H
