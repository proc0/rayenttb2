#pragma once

#include <raylib.h>

#include "world.hpp"

class Display {
    public:
    int screenWidth;
    int screenHeight;

    Display(){};
    ~Display() = default;

    void load();
    void render() const;
    void update(World& world);
    void unload();
};
