// Used to construct the parking hall from level data, derived from Entity

#ifndef TILE_H
#define TILE_H

#include "entity.h"

#include <QMouseEvent>

class Tile : public Entity {
    Q_OBJECT

signals:
    void hoverEnter(QPoint pos);
    void hoverLeave();

public:
    explicit Tile(int x, int y, QChar code);

    virtual bool clickable();

    QChar code() const;
    bool solid() const;

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    static const QImage IMAGE_GROUND;
    static const QImage IMAGE_WALL_CORNER;
    static const QImage IMAGE_WALL_SOUTHFACING;
    static const QImage IMAGE_WALL_WESTFACING;
    static const QImage IMAGE_PILLAR;

    QChar code_;
};

#endif // TILE_H
