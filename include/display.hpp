#pragma once

#include "world.hpp"

class Display {
    public:
    int screenWidth;
    int screenHeight;

    Display(){};
    ~Display() = default;

    void load();
    void render() const;
    void resize(int width, int height);
    void update(World& world);
    void unload();
};
