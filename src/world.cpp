#include "world.hpp"
#include "raylib.h"

void MovementSystem(World& world) {
    auto view = world.registry().view<Position, Velocity>();
    for (auto [ent, pos, vel] : view.each()) {
        // auto& pos = view.get<Position>(entity);
        // auto& vel = view.get<Velocity>(entity);
        pos.x += vel.x;
        pos.y += vel.y;
    }
}

int World::count() {
    return count_;
}

void World::load(){
    std::string pathAssets = DIR_ASSETS;
    const char* pathSoundSplat = pathAssets.append("/").append(URI_SOUND_SPLAT).c_str();
    splat = LoadSound(pathSoundSplat);
    count_ = 0;
    spawnParticle(100.0f, 100.0f);
}

void World::render() const {
    DrawRectangleGradientH(0, 0, screenWidth, screenHeight, BLUE, ORANGE);
    auto const view = _registry.view<Position>();
    for (auto e : view) {
        Position pos = _registry.get<Position>(e);
        DrawCircle(pos.x, pos.y, 20.0f, RAYWHITE);
    }
}

entt::entity World::spawnParticle(float x, float y) {
    auto entity = _registry.create();
    _registry.emplace<ParticleTag>(entity);
    _registry.emplace<Position>(entity, x, y);
    _registry.emplace<Velocity>(entity, 1.0f, 1.0f);
    return entity;
}

void World::update(){

    if(IsKeyPressed(KEY_SPACE)){
        count_++;
        PlaySound(splat);
    }
    
    if (IsKeyPressed(KEY_H)){
        if (IsCursorHidden()){
            ShowCursor();
        } else {
            HideCursor();
        }
    }

    MovementSystem(*this);
}

void World::unload(){
    UnloadSound(splat);
}

