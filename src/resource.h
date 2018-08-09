// Player-collectable resources, derived from Entity

#ifndef RESOURCE_H
#define RESOURCE_H

#include "entity.h"

class Resource : public Entity {

public:
    explicit Resource(int x, int y);

    virtual bool clickable() { return false; }

private:
    static const QImage IMAGE_RESOURCE;
};

#endif // RESOURCE_H
