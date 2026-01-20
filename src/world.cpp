#include "world.hpp"

void MovementSystem(World& world) {
    auto view = world.registry().view<Position, Velocity>();
    for (auto [ent, pos, vel] : view.each()) {
        // auto& pos = view.get<Position>(entity);
        // auto& vel = view.get<Velocity>(entity);
        pos.x += vel.x;
        pos.y += vel.y;
    }
}

void World::load(){
    spawnParticle(100.0f, 100.0f);

    // b2SetLengthUnitsPerMeter( 10.0f );

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

    // todo experimental, lookup description
    worldDef.enableContactSoftening = true;

    worldId = b2CreateWorld( &worldDef );
}

void World::createGround() {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    b2BodyId groundId = b2CreateBody( worldId, &bodyDef );

    b2Vec2 points[] = { { 40.0f, -32.0f }, { -40.0f, -32.0f }, { -40.0f, 32.0f }, { 40.0f, 32.0f } };

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
    bodyDef.position = { RandomFloatRange( -38.0f, 38.0f ), RandomFloatRange( -28.0f, 28.0f ) };
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
        b2Circle circle = { { 0.0f, 0.0f }, 1.0f };
        b2CreateCircleShape( m_debrisIds[index], &shapeDef, &circle );
    }
    else if (( index + 1 ) % 2 == 0)
    {
        b2Capsule capsule = { { 0.0f, -0.25f }, { 0.0f, 0.25f }, 0.75f };
        b2CreateCapsuleShape( m_debrisIds[index], &shapeDef, &capsule );
    }
    else
    {
        b2Polygon box = b2MakeBox( 0.4f, 1.0f );
        b2CreatePolygonShape( m_debrisIds[index], &shapeDef, &box );
    }
}

void World::render() const {
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
    if (IsKeyPressed(KEY_H)){
        if (IsCursorHidden()){
            ShowCursor();
        } else {
            HideCursor();
        }
    }

    MovementSystem(*this);
    b2World_Step( worldId, timeStep, 4 );

    debug.DrawWorld(worldId);
    // b2World_Draw( worldId, &raylibDebugDraw.draw );
}

void World::unload(){
    b2DestroyWorld( worldId );
    worldId = b2_nullWorldId;
}

