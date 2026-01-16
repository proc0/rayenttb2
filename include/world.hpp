#pragma once

#include <raylib.h>
#include <entt/entt.hpp>
#include <string>

#include "config.h"

#define URI_SOUND_SPLAT "splat1.wav"

typedef struct Position { float x; float y; } Position;
typedef struct Velocity { float x; float y; } Velocity;
typedef struct ParticleTag {} ParticleTag;

class World {
    int count_;
    Sound splat;
    
    public:
    int screenWidth;
    int screenHeight;
    entt::registry& _registry;
    entt::registry& registry() { return _registry; }

    World(entt::registry& registry_): _registry(registry_) {};
    ~World() = default;
    
    int count();
    void load();
    void render() const;
    entt::entity spawnParticle(float x, float y);
    void update();
    void unload();
};

void MovementSystem(World &world);