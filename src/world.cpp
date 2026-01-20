#include "world.hpp"
#include "box2d/box2d.h"
#include "box2d/math_functions.h"
#include "raylib.h"
// #include "raylib.h"

// void MovementSystem(World& world) {
//     auto view = world.registry().view<Position, Velocity>();
//     for (auto [ent, pos, vel] : view.each()) {
//         // auto& pos = view.get<Position>(entity);
//         // auto& vel = view.get<Velocity>(entity);
//         pos.x += vel.x;
//         pos.y += vel.y;
//     }
// }

// void CollisionSystem(World& world) {
//     auto const view = world.registry().view<CollisionData>();
//     for (auto [ent, collision] : view.each()) {
//         b2Vec2 vel = b2Body_GetLinearVelocity(collision.bodyId);

//         if (fabsf(vel.x) > 20.0f) {
//             TraceLog(LOG_INFO, "HAH");
//         }
//         // auto& pos = view.get<Position>(entity);
//         // auto& vel = view.get<Velocity>(entity);
//         // pos.x += vel.x;
//         // pos.y += vel.y;
//     }
// }

Vector2 MapVector(b2Vec2 vec) {
    // half screen width/height - 15 for some reason
    // todo: why -15?
    return {
        625 + vec.x * 10.0f,
        345 - vec.y * 10.0f
    };
}

void UpdateBall(World& world) {
    auto const view = world.registry().view<CollisionData, TextureData>();
    for (auto [ent, col, tex] : view.each()) {
        b2Vec2 vel = b2Body_GetLinearVelocity(col.bodyId);
        int speed = static_cast<int>(b2Length(vel))*3;

        if (speed > 60) {
            speed = 60;
        } else if (speed < 1) {
            speed = 1;
        }

        tex.frameCounter++;

        if (tex.frameCounter >= (60/speed))
        {
            tex.frameCounter = 0;
            tex.frameIndex++;

            if (tex.frameIndex > 31) tex.frameIndex = 0;

            tex.frame.x = static_cast<float>(tex.frameIndex*BALL_SIZE);
        }

    }
}

void DrawBall(const World& world) {
    auto const view = world.registry().view<CollisionData, TextureData>();
    for (auto [ent, col, tex] : view.each()) {
        b2Vec2 pos = b2Body_GetPosition(col.bodyId);
        b2Rot rot = b2Body_GetRotation(col.bodyId);
        float rad = b2Rot_GetAngle(rot);
        float angle = RAD2DEG*rad;
        Vector2 p = MapVector(pos);
        
        DrawTexturePro(tex.texture, tex.frame, { p.x, p.y, 32, 32 }, {15, 15}, angle, WHITE);
        
        // auto& pos = view.get<Position>(entity);
        // auto& vel = view.get<Velocity>(entity);
        // pos.x += vel.x;
        // pos.y += vel.y;
    }
}

void World::load(){

    // spawnParticle(100.0f, 100.0f);
    // b2SetLengthUnitsPerMeter( 10.0f );
    std::string pathAssets = DIR_ASSETS;
    const char* pathBallTexture = pathAssets.append("/").append(TEXTURE_BALL_URI).c_str();
    ballTexture = LoadTexture(pathBallTexture);

    createWorld();
    createGround();

    for (int i = 0; i < ENT_COUNT; ++i) {
        m_debrisIds[i] = b2_nullBodyId;
        m_bodyUserData[i].index = i;
        m_bodyUserData[i].entId = entt::null;
        spawnDebris(i);
    }
}

void World::createWorld()
{
    if ( B2_IS_NON_NULL( worldId ) )
    {
        b2DestroyWorld( worldId );
        worldId = b2_nullWorldId;
    }

    b2WorldDef worldDef = b2DefaultWorldDef();
    // worldDef.workerCount = context->workerCount;
    // worldDef.enqueueTask = EnqueueTask;
    // worldDef.finishTask = FinishTask;
    // worldDef.userTaskContext = this;
    // todo experimental, lookup description    
    worldDef.enableContactSoftening = true;

    worldId = b2CreateWorld( &worldDef );
    // b2World_SetGravity(worldId, { 0.0f, -10.0f });
}

void World::createGround() {
    b2BodyDef body = b2DefaultBodyDef();
    b2BodyId groundId = b2CreateBody( worldId, &body );

    b2Vec2 points[] = { { 40.0f, -32.0f }, { -40.0f, -32.0f }, { -40.0f, 32.0f }, { 40.0f, 32.0f } };

    b2ChainDef chainDef = b2DefaultChainDef();
    chainDef.count = 4;
    chainDef.points = points;
    chainDef.isLoop = true;

    b2CreateChain( groundId, &chainDef );
}

void World::spawnDebris(int index)
{
    // int index = -1;
    // for (int i = 0; i < ENT_COUNT; ++i) {
    //     if (B2_IS_NULL( m_debrisIds[i] )) {
    //         index = i;
    //         break;
    //     }
    // }

    // if (index == -1) {
    //     return;
    // }

    // Create dynamic body
    b2BodyDef body = b2DefaultBodyDef();
    body.type = b2_dynamicBody;
    body.position = { RandomFloatRange( -38.0f, 38.0f ), RandomFloatRange( -28.0f, 28.0f ) };
    // body.position = { static_cast<float>(GetRandomValue( -38.0f, 38.0f )), static_cast<float>(GetRandomValue( -28.0f, 28.0f )) };
    body.rotation = b2MakeRot( RandomFloatRange( -B2_PI, B2_PI ) );
    body.linearVelocity = { RandomFloatRange( -5.0f, 5.0f ), RandomFloatRange( -5.0f, 5.0f ) };
    body.angularVelocity = RandomFloatRange( -1.0f, 1.0f );
    body.gravityScale = 0.0f;

    // Create entity
    auto entity = _registry.create();
    _registry.emplace<EntityId>(entity);
    _registry.emplace<TextureData>(entity, Rectangle({ 0, 0, BALL_SIZE, BALL_SIZE }), ballTexture, 0, 0, 8);
    m_bodyUserData[index].entId = entity;
    
    // Link entity with body
    body.userData = m_bodyUserData + index;
    b2BodyId bodyId = b2CreateBody( worldId, &body );
    m_debrisIds[index] = bodyId;
    _registry.emplace<CollisionData>(entity, bodyId);
    
    // Create shape for body
    b2ShapeDef shape = b2DefaultShapeDef();
    shape.material.restitution = 0.8f;

    // No events when debris hits debris
    shape.enableContactEvents = false;


    TraceLog(LOG_INFO, "pos %f, %f", body.position.x, body.position.y);

    b2Circle circle = { { 0.0f, 0.0f }, 1.4f };
    b2CreateCircleShape( m_debrisIds[index], &shape, &circle );
    // if (( index + 1 ) % 3 == 0) {
    //     b2Circle circle = { { 0.0f, 0.0f }, 1.4f };
        
    //     b2CreateCircleShape( m_debrisIds[index], &shape, &circle );
    // } 
    // // else if (( index + 1 ) % 2 == 0) {
    // //     b2Capsule capsule = { { 0.0f, -0.25f }, { 0.0f, 0.25f }, 0.75f };
        
    // //     b2CreateCapsuleShape( m_debrisIds[index], &shape, &capsule );
    // // } 
    // else {
    //     b2Polygon box = b2MakeBox( 0.4f, 1.0f );
        
    //     b2CreatePolygonShape( m_debrisIds[index], &shape, &box );
    // }

}

void World::render() const {
    // auto const view = _registry.view<Position>();
    // for (auto e : view) {
    //     Position pos = _registry.get<Position>(e);
    //     DrawCircle(pos.x, pos.y, 20.0f, RAYWHITE);
    // }
    // DrawTextureRec(ballTexture, ballRec, {100,100}, WHITE);
    DrawBall(*this);
}

// entt::entity World::spawnParticle(float x, float y) {
//     auto entity = _registry.create();
//     _registry.emplace<ParticleTag>(entity);
//     _registry.emplace<Position>(entity, x, y);
//     _registry.emplace<Velocity>(entity, 1.0f, 1.0f);
//     return entity;
// }

void World::resize(int width, int height) {
    screenWidth = width;
    screenHeight = height;
    debug.resize(width, height);
}

void World::update(){
    // frameCounter++;

    // if (frameCounter >= (60/frameSpeed))
    // {
    //     frameCounter = 0;
    //     frameIndex++;

    //     if (frameIndex > 31) frameIndex = 0;

    //     ballRec.x = static_cast<float>(frameIndex*BALL_SIZE);
    // }
    UpdateBall(*this);
    // CollisionSystem(*this);
    b2World_Step( worldId, timeStep, 4 );

    // debug.render(worldId);

    // b2World_Draw( worldId, &raylibDebugDraw.draw );
}

void World::unload(){
    UnloadTexture(ballTexture);
    b2DestroyWorld( worldId );
    worldId = b2_nullWorldId;
}

