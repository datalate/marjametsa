#include "tile.h"

#include <QDebug>
#include "common.h"

const QImage Tile::IMAGE_GROUND{"../gfx/asphalt_piece.png"};
const QImage Tile::IMAGE_PILLAR{"../gfx/pillar.png"};
const QImage Tile::IMAGE_WALL_CORNER{"../gfx/wall_corner.png"};
const QImage Tile::IMAGE_WALL_SOUTHFACING{"../gfx/wall_southfacing.png"};
const QImage Tile::IMAGE_WALL_WESTFACING{"../gfx/wall_westfacing.png"};

const int GROUND_Y_OFFSET{16};
const int PILLAR_Y_OFFSET{-37};
const int OTHER_Y_OFFSET{-16};

Tile::Tile(int x, int y, QChar code) :
    Entity{x, y}, code_{code} {

    if (code_ == TILE_GROUND) { // Only highlight ground floor
        setAcceptHoverEvents(true);
    }
    else {
        setAcceptHoverEvents(false);
    }

    if (clickable()) {
        setAcceptedMouseButtons(Qt::LeftButton);
        setFlags(QGraphicsItem::ItemIsSelectable);
    }

    switch (code.toLatin1()) {
    case TILE_GROUND:
    {
        setPixmap(QPixmap::fromImage(IMAGE_GROUND));
        break;
    }
    case TILE_PILLAR:
    {
        setPixmap(QPixmap::fromImage(IMAGE_PILLAR));
        break;
    }
    case TILE_WALL_CORNER:
    {
        setPixmap(QPixmap::fromImage(IMAGE_WALL_CORNER));
        break;
    }
    case TILE_WALL_WESTFACING:
    {
        setPixmap(QPixmap::fromImage(IMAGE_WALL_WESTFACING));
        break;
    }
    case TILE_WALL_SOUTHFACING:
    {
        setPixmap(QPixmap::fromImage(IMAGE_WALL_SOUTHFACING));
        break;
    }
    case TILE_WALL_INVISIBLE:
    {
        // "Invisible" corner
        // Needed to break walls for correct map dimension
        // eg. x____+
        //          |
        //          x
        // Also used for the parts of the map that just don't exist ie. where
        // there's no floor intentionally
        break;
    }
    default:
        Q_ASSERT(false); // Tile of wrong type
    }

    if (code == TILE_GROUND) {
        setOffset(pixmap().width() / 2, GROUND_Y_OFFSET);
    } else if (code == TILE_PILLAR) {
        setOffset(pixmap().width() / 2, PILLAR_Y_OFFSET);
    }
    else {
        setOffset(pixmap().width() / 2, OTHER_Y_OFFSET);
    }

    if (code == TILE_PILLAR) {
        setZValue(pos().y() + 1); // Pillars are above all other tiles
    }
    else {
        setZValue(pos().y()); // Bottom tiles overlap above ones
    }
}

bool Tile::clickable() {
    return (code_ != TILE_PILLAR); // Click through pillars
}

QChar Tile::code() const {
    return code_;
}

bool Tile::solid() const {
    return (code_ != TILE_GROUND);
}

void Tile::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    Q_UNUSED(event);
    emit hoverEnter({x(), y()});

    //QGraphicsItem::hoverEnterEvent(event); Probably not needed?
}

void Tile::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    Q_UNUSED(event);
    emit hoverLeave();
    //QGraphicsItem::hoverEnterEvent(event);
}
