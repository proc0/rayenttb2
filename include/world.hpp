#pragma once

#include <string> // IWYU pragma: export
#include <raylib.h>
#include <entt/entt.hpp>
#include <box2d/id.h>
#include <box2d/types.h>
#include <box2d/box2d.h>
#include <box2d/math_functions.h>

#include "config.h"
#include "debug.hpp"

#define TEXTURE_BALL_URI "ball.png"
#define BALL_SIZE 32

#define ENT_COUNT 50
#define RAND_LIMIT 32767
#define RAND_SEED 12345

static uint32_t g_randomSeed = RAND_SEED;
// Simple random number generator. Using this instead of rand() for cross-platform determinism.
B2_INLINE int RandomInt()
{
    // XorShift32 algorithm
    uint32_t x = g_randomSeed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    g_randomSeed = x;

    // Map the 32-bit value to the range 0 to RAND_LIMIT
    return (int)( x % ( RAND_LIMIT + 1 ) );
}

// Random number in range [-1,1]
B2_INLINE float RandomFloat()
{
    float r = (float)( RandomInt() & ( RAND_LIMIT ) );
    r /= RAND_LIMIT;
    r = 2.0f * r - 1.0f;
    return r;
}

// Random floating point number in range [lo, hi]
B2_INLINE float RandomFloatRange( float lo, float hi )
{
    float r = (float)( RandomInt() & ( RAND_LIMIT ) );
    r /= RAND_LIMIT;
    r = ( hi - lo ) * r + lo;
    return r;
}

// typedef struct Position { float x; float y; } Position;
// typedef struct Velocity { float x; float y; } Velocity;
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
    Debug debug = Debug(10.0f);
    Rectangle ballRec = { 0, 0, BALL_SIZE, BALL_SIZE };
    float const timeStep = 1.0f/60.0f;
    // short frameCounter = 0;
    // short frameIndex = 0;
    // short frameSpeed = 8;

    public:
    int screenWidth;
    int screenHeight;
    entt::registry& _registry;
    entt::registry& registry() const { return _registry; }

    World(entt::registry& registry_): _registry(registry_) {};
    ~World() = default;
    
    void load();
    void createWorld();
    void createGround();
    void createPlayer(float x, float y);
    void render() const;
    void resize(int width, int height);
    void spawnDebris(int index);
    // entt::entity spawnParticle(float x, float y);
    void update();
    void unload();
};

void MovementSystem(World &world);
Vector2 MapVector(b2Vec2);
b2Vec2 UnmapVector(Vector2);