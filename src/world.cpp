#include "world.hpp"
#include "raylib.h"

void DrawPolygonFcn( const b2Vec2* vertices, int vertexCount, b2HexColor color, void* context ) {
    // SampleContext* sampleContext = static_cast<SampleContext*>( context );
    // DrawPolygon( sampleContext->draw, vertices, vertexCount, color );
}

void DrawSolidPolygonFcn( b2Transform transform, const b2Vec2* vertices, int vertexCount, float radius, b2HexColor color,
                          void* context ) {
    // SampleContext* sampleContext = static_cast<SampleContext*>( context );
    // DrawSolidPolygon( sampleContext->draw, transform, vertices, vertexCount, radius, color );
}

void DrawCircleFcn( b2Vec2 center, float radius, b2HexColor color, void* context ) {
    // SampleContext* sampleContext = static_cast<SampleContext*>( context );
    // DrawCircle( sampleContext->draw, center, radius, color );
    DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), radius, BLACK);
}

void DrawSolidCircleFcn( b2Transform transform, float radius, b2HexColor color, void* context ) {
    // SampleContext* sampleContext = static_cast<SampleContext*>( context );
    // DrawSolidCircle( sampleContext->draw, transform, radius, color );
}

void DrawSolidCapsuleFcn( b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color, void* context ) {
    // SampleContext* sampleContext = static_cast<SampleContext*>( context );
    // DrawSolidCapsule( sampleContext->draw, p1, p2, radius, color );
}

void DrawLineFcn( b2Vec2 p1, b2Vec2 p2, b2HexColor color, void* context ) {
    // SampleContext* sampleContext = static_cast<SampleContext*>( context );
    // DrawLine( sampleContext->draw, p1, p2, color );
    DrawLine(static_cast<int>(p1.x), static_cast<int>(p1.y), static_cast<int>(p2.x), static_cast<int>(p2.y), YELLOW);
}

void DrawTransformFcn( b2Transform transform, void* context ) {
    // SampleContext* sampleContext = static_cast<SampleContext*>( context );
    // DrawTransform( sampleContext->draw, transform, 1.0f );
}

void DrawPointFcn( b2Vec2 p, float size, b2HexColor color, void* context ) {
    // SampleContext* sampleContext = static_cast<SampleContext*>( context );
    // DrawPoint( sampleContext->draw, p, size, color );
    DrawCircle(static_cast<int>(p.x), static_cast<int>(p.y), size, BLACK); 
}

void DrawStringFcn( b2Vec2 p, const char* s, b2HexColor color, void* context ) {
    // SampleContext* sampleContext = static_cast<SampleContext*>( context );
    // DrawWorldString( sampleContext->draw, &sampleContext->camera, p, color, s );
}

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

    // b2SetLengthUnitsPerMeter( 60.0f );

    debugDraw = b2DefaultDebugDraw();
    debugDraw.DrawPolygonFcn = DrawPolygonFcn;
    debugDraw.DrawSolidPolygonFcn = DrawSolidPolygonFcn;
    debugDraw.DrawCircleFcn = DrawCircleFcn;
    debugDraw.DrawSolidCircleFcn = DrawSolidCircleFcn;
    debugDraw.DrawSolidCapsuleFcn = DrawSolidCapsuleFcn;
    debugDraw.DrawLineFcn = DrawLineFcn;
    debugDraw.DrawTransformFcn = DrawTransformFcn;
    debugDraw.DrawPointFcn = DrawPointFcn;
    debugDraw.DrawStringFcn = DrawStringFcn;

    createWorld();
    createGround();

    for (int i = 0; i < ENT_COUNT; ++i)
    {
        m_debrisIds[i] = b2_nullBodyId;
        m_bodyUserData[i].index = i;
        spawnDebris();
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
    // worldDef.enableSleep = context->enableSleep;

    // todo experimental
    // worldDef.enableContactSoftening = true;

    worldId = b2CreateWorld( &worldDef );
}

void World::createGround() {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    b2BodyId groundId = b2CreateBody( worldId, &bodyDef );

    b2Vec2 points[] = { { 40.0f, -40.0f }, { -40.0f, -40.0f }, { -40.0f, 40.0f }, { 40.0f, 40.0f } };

    b2ChainDef chainDef = b2DefaultChainDef();
    chainDef.count = 4;
    chainDef.points = points;
    chainDef.isLoop = true;

    b2CreateChain( groundId, &chainDef );
}

void World::spawnDebris()
{
    int index = -1;
    for (int i = 0; i < ENT_COUNT; ++i)
    {
        if (B2_IS_NULL( m_debrisIds[i] ))
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        return;
    }

    // Debris
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = { RandomFloatRange( -38.0f, 38.0f ), RandomFloatRange( -38.0f, 38.0f ) };
    bodyDef.rotation = b2MakeRot( RandomFloatRange( -B2_PI, B2_PI ) );
    bodyDef.linearVelocity = { RandomFloatRange( -5.0f, 5.0f ), RandomFloatRange( -5.0f, 5.0f ) };
    bodyDef.angularVelocity = RandomFloatRange( -1.0f, 1.0f );
    bodyDef.gravityScale = 0.0f;
    bodyDef.userData = m_bodyUserData + index;
    m_debrisIds[index] = b2CreateBody( worldId, &bodyDef );

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.material.restitution = 0.8f;

    // No events when debris hits debris
    shapeDef.enableContactEvents = false;

    if (( index + 1 ) % 3 == 0)
    {
        b2Circle circle = { { 0.0f, 0.0f }, 0.5f };
        b2CreateCircleShape( m_debrisIds[index], &shapeDef, &circle );
    }
    else if (( index + 1 ) % 2 == 0)
    {
        b2Capsule capsule = { { 0.0f, -0.25f }, { 0.0f, 0.25f }, 0.25f };
        b2CreateCapsuleShape( m_debrisIds[index], &shapeDef, &capsule );
    }
    else
    {
        b2Polygon box = b2MakeBox( 0.4f, 0.6f );
        b2CreatePolygonShape( m_debrisIds[index], &shapeDef, &box );
    }
}

void World::render() const {
    // DrawRectangleGradientH(0, 0, screenWidth, screenHeight, BLUE, ORANGE);
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
    b2World_Step( worldId, timeStep, 4 );

    b2World_Draw( worldId, &debugDraw );
}

void World::unload(){
    UnloadSound(splat);
    b2DestroyWorld( worldId );
    worldId = b2_nullWorldId;
}

