#include "entity.h"

#include <QDebug>

Entity::Entity(int x, int y) :
    QObject{}, QGraphicsPixmapItem{}, x_{x}, y_{y} {

    move(x, y);
}

void Entity::move(int x, int y) {
    x_ = x;
    y_ = y;

    setPos(32 * x + 32 * y, 16 * y - 16 * x);
}

void Entity::mousePressEvent(QGraphicsSceneMouseEvent *e) {
    if (clickable()) {
        emit clicked(this);
    }
    else {
        QGraphicsPixmapItem::mousePressEvent(e);
    }
}

void Entity::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) {
    if (!clickable()) {
        QGraphicsPixmapItem::mouseReleaseEvent(e);
    }
}
