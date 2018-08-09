// Base class for everything that is added to the ParkingHall

#ifndef ENTITY_H
#define ENTITY_H

#include <QObject>
#include <QGraphicsPixmapItem>

class Entity : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

signals:
    void clicked(Entity *entity);

public:
    explicit Entity(int x, int y);

    int x() const { return x_; }
    int y() const { return y_; }

    virtual void move(int x, int y);
    virtual bool clickable() = 0;

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *e);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);

private:
    int x_, y_;
};

#endif // ENTITY_H
