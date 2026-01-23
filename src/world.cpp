#include "world.hpp"

void UpdateBall(World& world) {
    auto const view = world.registry().view<CollisionData, TextureData>();
    for (auto [ent, col, tex] : view.each()) {
        b2Vec2 vel = b2Body_GetLinearVelocity(col.bodyId);
        int speed = static_cast<int>(fabsf(b2Length(vel)))*10;

        // TODO: figure out if this needs to be the FPS, and
        // whether to pass through with Screen for Web (how to find out Web FPS?)
        if (speed > 120) {
            speed = 120;
        } else if (speed < 1) {
            if (speed < 0.000001) {
                continue;
            }

            speed = 1;
        }

        tex.frameCounter++;

        if (tex.frameCounter >= (120/speed))
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
        b2Vec2 vel = b2Body_GetLinearVelocity(col.bodyId);
        b2Vec2 unitV = b2Normalize(vel);
        float angle = col.angle;
        if (unitV.x != 0.0f && unitV.y != 0.0f) {
            b2Rot rot = b2ComputeRotationBetweenUnitVectors(unitV, { 1.0f, 0 });
            float rad = b2Rot_GetAngle(rot);
            angle = RAD2DEG*rad;
            col.angle = angle;
        }

        b2Vec2 pos = b2Body_GetWorldPoint(col.bodyId, b2Vec2({ 0, 0 }));
        Vector2 p= world.screen.b2Vec2ToVector2(pos);
        
        DrawTexturePro(tex.texture, tex.frame, { p.x, p.y, 32, 32 }, { 16, 16 }, angle, WHITE);
    }
}

void World::load(){
    // b2SetLengthUnitsPerMeter( 10.0f );
    std::string pathAssets = DIR_ASSETS;
    const char* pathBallTexture = pathAssets.append("/").append(TEXTURE_BALL_URI).c_str();
    ballTexture = LoadTexture(pathBallTexture);

    createWorld();
    createGround();
    createPlayer(0, 0);
    
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

void World::createPlayer(float x, float y) {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = { x, y };
    bodyDef.gravityScale = 0.0f;
    bodyDef.linearDamping = 0.5f;
    bodyDef.angularDamping = 0.5f;
    bodyDef.isBullet = true;

    playerId = b2CreateBody( worldId, &bodyDef );
    b2MassData massData = {
        .mass = 5.0f,
        .center = { 1.0f, 10.0f },
        .rotationalInertia = 5.0f
    };
    b2Body_SetMassData(playerId, massData);

    b2Polygon box = b2MakeBox( 0.5f, 10.0f );
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 2000.0f;
    // Enable contact events for the player shape
    shapeDef.enableContactEvents = true;

    playerShapeId = b2CreatePolygonShape( playerId, &shapeDef, &box );
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
    body.rotation = b2MakeRot( RandomFloatRange( -B2_PI, B2_PI ) );
    body.linearVelocity = { RandomFloatRange( -5.0f, 5.0f ), RandomFloatRange( -5.0f, 5.0f ) };
    body.angularVelocity = RandomFloatRange( -1.0f, 1.0f );
    body.gravityScale = 0.0f;
    body.linearDamping = 0.8f;
    // body.angularDamping = 0.5f;
    body.motionLocks = {
        .linearX = false,
        .linearY = false,
        .angularZ = true,
    };
    // Create entity
    auto entity = _registry.create();
    _registry.emplace<EntityId>(entity);
    _registry.emplace<TextureData>(entity, Rectangle({ 0, 0, BALL_SIZE, BALL_SIZE }), ballTexture, 0, 0, 8);
    m_bodyUserData[index].entId = entity;
    
    // Link entity with body
    body.userData = m_bodyUserData + index;
    b2BodyId bodyId = b2CreateBody( worldId, &body );
    m_debrisIds[index] = bodyId;
    _registry.emplace<CollisionData>(entity, bodyId, 0.0f);
    
    // Create shape for body
    b2ShapeDef shape = b2DefaultShapeDef();
    shape.material.restitution = 0.6f;
    shape.density = 1500.0f;

    // No events when debris hits debris
    shape.enableContactEvents = false;

    b2Circle circle = { { 0.0f, 0.0f }, 1.4f };
    b2CreateCircleShape( m_debrisIds[index], &shape, &circle );
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

void World::resize(int width, int height) {
    screenWidth = width;
    screenHeight = height;
}

void World::update(){
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        Vector2 mousePos = { static_cast<float>(GetMouseX()), static_cast<float>(GetMouseY()) };
        b2Vec2 pos = screen.vector2Tob2Vec2(mousePos);

        if (B2_IS_NON_NULL(playerId)) {
            b2DestroyShape(playerShapeId, false);
            b2DestroyBody(playerId);
        }

        createPlayer(static_cast<float>(pos.x), static_cast<float>(pos.y));

        // b2Body_ApplyMassFromShapes(playerId);
    }

    if (B2_IS_NON_NULL(playerId)) {

        b2Vec2 position = b2Body_GetPosition( playerId );
        Vector2 mouseDelta = GetMouseDelta();
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            b2Vec2 force = { 0, mouseDelta.y*500000.0f };
            b2Body_ApplyForce( playerId, force, position, true );
        } else {
            b2Body_ApplyAngularImpulse( playerId, mouseDelta.x*100000.0f, true);
        }
    }

    UpdateBall(*this);
    b2World_Step( worldId, timeStep, 4 );

    debug.render(worldId);

}

void World::unload(){
    UnloadTexture(ballTexture);
    b2DestroyWorld( worldId );
    worldId = b2_nullWorldId;
}

