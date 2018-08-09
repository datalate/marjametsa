#include "resource.h"

const QImage Resource::IMAGE_RESOURCE{"../gfx/resource.png"};

Resource::Resource(int x, int y) :
    Entity{x, y} {

    setPixmap(QPixmap::fromImage(IMAGE_RESOURCE));
    setOffset(pixmap().width() / 2, 14);
    setZValue(pos().y() + 1);
}
