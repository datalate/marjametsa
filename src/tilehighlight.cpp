#include "tilehighlight.h"

const QImage TileHighlight::IMAGE_HILIGHT{"../gfx/grid_hilight.png"};

TileHighlight::TileHighlight() :
    Entity{0, 0} {

    setVisible(false);
    setPixmap(QPixmap::fromImage(IMAGE_HILIGHT));
    setOffset(pixmap().width() / 2 - 6, 16);
    setZValue(10000); // Above everything else
}

void TileHighlight::move(int x, int y) {
    Entity::move(x, y);

    setZValue(pos().y() + pixmap().height());
}
