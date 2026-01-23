#pragma once

#include <string> // IWYU pragma: export
#include <raylib.h> // IWYU pragma: export
#include <entt/entt.hpp> // IWYU pragma: export
#include <box2d/id.h> // IWYU pragma: export
#include <box2d/types.h> // IWYU pragma: export
#include <box2d/box2d.h> // IWYU pragma: export
#include <box2d/math_functions.h> // IWYU pragma: export

#include "config.h" // IWYU pragma: export
#include "debug.hpp"
#include "screen.hpp"
#include "global.hpp" // IWYU pragma: export
#include "ball.hpp" // IWYU pragma: export

#define ENT_COUNT 50

class World {
    b2BodyId m_debrisIds[ENT_COUNT];
    BallUserData m_bodyUserData[ENT_COUNT];
    b2BodyId playerId;
    b2ShapeId playerShapeId;
    b2WorldId worldId = b2_nullWorldId;
    Rectangle ballRec = { 0, 0, BALL_SIZE, BALL_SIZE };
    float const timeStep = 1.0f/60.0f;

    public:
    int screenWidth;
    int screenHeight;
    entt::registry& _registry;
    entt::registry& registry() const { return _registry; }
    Screen& screen;
    Debug debug = Debug(screen);
    Ball ball = Ball(screen);

    World(entt::registry& registry_, Screen& _screen) : 
    _registry(registry_), 
    screen(_screen) {};
    ~World() = default;
    
    void load();
    void createWorld();
    void createContainer();
    void createPlayer(float x, float y);
    void render() const;
    void resize(int width, int height);
    void createBall(int index);
    void update();
    void unload();
};

void MovementSystem(World &world);
Vector2 MapVector(b2Vec2);
b2Vec2 UnmapVector(Vector2);