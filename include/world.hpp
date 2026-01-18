#pragma once

#include <string>
#include <raylib.h>
#include <entt/entt.hpp>
#include <box2d/id.h>
#include <box2d/types.h>
#include <box2d/box2d.h>
#include <box2d/math_functions.h>

#include "config.h"

#define URI_SOUND_SPLAT "splat1.wav"
#define ENT_COUNT 20
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

void DrawPolygonFcn( const b2Vec2* vertices, int vertexCount, b2HexColor color, void* context );

void DrawSolidPolygonFcn( b2Transform transform, const b2Vec2* vertices, int vertexCount, float radius, b2HexColor color,
                          void* context );

void DrawCircleFcn( b2Vec2 center, float radius, b2HexColor color, void* context );

void DrawSolidCircleFcn( b2Transform transform, float radius, b2HexColor color, void* context );

void DrawSolidCapsuleFcn( b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color, void* context );

void DrawLineFcn( b2Vec2 p1, b2Vec2 p2, b2HexColor color, void* context );

void DrawTransformFcn( b2Transform transform, void* context );

void DrawPointFcn( b2Vec2 p, float size, b2HexColor color, void* context );

void DrawStringFcn( b2Vec2 p, const char* s, b2HexColor color, void* context );

typedef struct Position { float x; float y; } Position;
typedef struct Velocity { float x; float y; } Velocity;
typedef struct ParticleTag {} ParticleTag;

struct BodyUserData
{
    int index;
};

class World {
    b2BodyId m_debrisIds[ENT_COUNT];
    BodyUserData m_bodyUserData[ENT_COUNT];
    Sound splat;
    b2WorldId worldId = b2_nullWorldId;
    b2DebugDraw debugDraw;
    float timeStep = 1.0f/60.0f;
    int count_;
    short entCount = 20;

    public:
    int screenWidth;
    int screenHeight;
    entt::registry& _registry;
    entt::registry& registry() { return _registry; }

    World(entt::registry& registry_): _registry(registry_) {};
    ~World() = default;
    
    int count();
    void load();
    void createWorld();
    void createGround();
    void spawnDebris();
    void render() const;
    entt::entity spawnParticle(float x, float y);
    void update();
    void unload();
};

void MovementSystem(World &world);