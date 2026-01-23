#pragma once

#include "debug.hpp"
#include "ball.hpp"

#define TIME_STEP 1.0f/60.0f
#define SUB_STEP 4

#define BALL_COUNT 50

class World {
    BallUserData ballUserData[BALL_COUNT];
    b2BodyId ballIds[BALL_COUNT];
    entt::registry registry;
    Screen& screen;
    Debug debug = Debug(screen);
    Ball ball = Ball(screen);

    b2BodyId playerId;
    b2ShapeId playerShapeId;
    b2WorldId worldId = b2_nullWorldId;

    public:
    World(Screen& _screen) : screen(_screen) {};
    ~World() = default;
    
    void load();
    void createWorld();
    void createContainer();
    void createPlayer(float x, float y);
    void render() const;
    void debugRender();
    void createBall(int index);
    void update();
    void unload();
};
