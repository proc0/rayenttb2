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

#define ENT_COUNT 50
#define RAND_LIMIT 32767
#define RAND_SEED 12345

inline uint32_t g_randomSeed;
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
typedef struct CollisionData { b2BodyId bodyId; } CollisionData;
typedef struct EntityId {} EntityId;

struct BodyUserData {
    int index;
    entt::entity entId;
};

class World {
    b2BodyId m_debrisIds[ENT_COUNT];
    BodyUserData m_bodyUserData[ENT_COUNT];
    b2WorldId worldId = b2_nullWorldId;
    Debug debug = Debug(10.0f);
    float const timeStep = 1.0f/60.0f;

    public:
    int screenWidth;
    int screenHeight;
    entt::registry& _registry;
    entt::registry& registry() { return _registry; }

    World(entt::registry& registry_): _registry(registry_) {};
    ~World() = default;
    
    void load();
    void createWorld();
    void createGround();
    void render() const;
    void resize(int width, int height);
    void spawnDebris();
    // entt::entity spawnParticle(float x, float y);
    void update();
    void unload();
};

void MovementSystem(World &world);