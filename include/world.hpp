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

#define TEXTURE_BALL_URI "ball.png"
#define BALL_SIZE 32

#define ENT_COUNT 50

typedef struct EntityId {} EntityId;
typedef struct CollisionData { b2BodyId bodyId; float angle; } CollisionData;
typedef struct TextureData {
    Rectangle frame = { 0, 0, BALL_SIZE, BALL_SIZE };
    Texture2D& texture;
    int frameCounter = 0;
    int frameIndex = 0;
    int frameSpeed = 8;
} TextureData;

struct BodyUserData {
    int index;
    entt::entity entId;
};

class World {
    b2BodyId m_debrisIds[ENT_COUNT];
    b2BodyId playerId;
    b2ShapeId playerShapeId;
    BodyUserData m_bodyUserData[ENT_COUNT];
    b2WorldId worldId = b2_nullWorldId;
    Texture2D ballTexture;
    Rectangle ballRec = { 0, 0, BALL_SIZE, BALL_SIZE };
    float const timeStep = 1.0f/60.0f;

    public:
    int screenWidth;
    int screenHeight;
    entt::registry& _registry;
    entt::registry& registry() const { return _registry; }
    Screen& screen;
    Debug debug = Debug(screen);

    World(entt::registry& registry_, Screen& _screen) : 
    _registry(registry_), 
    screen(_screen) {};
    ~World() = default;
    
    void load();
    void createWorld();
    void createGround();
    void createPlayer(float x, float y);
    void render() const;
    void resize(int width, int height);
    void spawnDebris(int index);
    void update();
    void unload();
};

void MovementSystem(World &world);
Vector2 MapVector(b2Vec2);
b2Vec2 UnmapVector(Vector2);