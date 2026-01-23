#include "world.hpp"

void World::load(){
    // b2SetLengthUnitsPerMeter( 10.0f );

    createWorld();
    createContainer();
    createPlayer(0, 0);
    
    ball.load();

    for (int i = 0; i < ENT_COUNT; ++i) {
        m_debrisIds[i] = b2_nullBodyId;
        m_bodyUserData[i].index = i;
        m_bodyUserData[i].entId = entt::null;
        createBall(i);
    }
}

void World::createWorld() {
    if (B2_IS_NON_NULL(worldId)) {
        b2DestroyWorld(worldId);
        worldId = b2_nullWorldId;
    }

    b2WorldDef worldDef = b2DefaultWorldDef();
    // todo experimental, lookup description    
    worldDef.enableContactSoftening = true;

    worldId = b2CreateWorld(&worldDef);
    // b2World_SetGravity(worldId, { 0.0f, -10.0f });
}

void World::createContainer() {
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

void World::createBall(int index) {
    BallUserData ballData = ball.create(_registry, worldId, m_bodyUserData, index);
    m_bodyUserData[index] = ballData;
    m_debrisIds[index] = ballData.bodyId;
}

void World::render() const {
    auto const view = registry().view<BallCollision, BallFrame>();
    for (auto [ent, col, tex] : view.each()) {
        ball.render(col, tex);
    }
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

    auto const view = registry().view<BallCollision, BallFrame>();
    for (auto [ent, col, tex] : view.each()) {
        ball.update(col, tex);
    }

    b2World_Step( worldId, timeStep, 4 );

    debug.render(worldId);
}

void World::unload(){
    b2DestroyWorld( worldId );
    worldId = b2_nullWorldId;
}

