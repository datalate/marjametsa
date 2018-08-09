// Used to highlight tiles with mouse

#ifndef TILEHIGHLIGHT_H
#define TILEHIGHLIGHT_H

#include "entity.h"

class TileHighlight : public Entity {
public:
    TileHighlight();

    virtual void move(int x, int y);
    virtual bool clickable() { return false; }

private:
    static const QImage IMAGE_HILIGHT;
};

#endif // TILEHIGHLIGHT_H
