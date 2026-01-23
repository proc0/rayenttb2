#include "ball.hpp"

void Ball::load() {
    std::string pathAssets = DIR_ASSETS;
    const char* pathBallTexture = pathAssets.append("/").append(TEXTURE_BALL_URI).c_str();
    ballTexture = LoadTexture(pathBallTexture);
}

BallUserData Ball::create(entt::registry& _registry, b2WorldId worldId, BallUserData* userDataContainer, int index) {
	b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = { RandomFloatRange( -38.0f, 38.0f ), RandomFloatRange( -28.0f, 28.0f ) };
    bodyDef.rotation = b2MakeRot( RandomFloatRange( -B2_PI, B2_PI ) );
    bodyDef.linearVelocity = { RandomFloatRange( -5.0f, 5.0f ), RandomFloatRange( -5.0f, 5.0f ) };
    bodyDef.angularVelocity = RandomFloatRange( -1.0f, 1.0f );
    bodyDef.gravityScale = 0.0f;
    bodyDef.linearDamping = 0.8f;
    // bodyDef.angularDamping = 0.5f;
    bodyDef.motionLocks = {
        .linearX = false,
        .linearY = false,
        .angularZ = true,
    };
    // Create entity
    auto entity = _registry.create();
    _registry.emplace<BallId>(entity);
    _registry.emplace<BallFrame>(entity, Rectangle({ 0, 0, BALL_SIZE, BALL_SIZE }), ballTexture, 0, 0, 8);
    
    // Link entity with body
    bodyDef.userData = userDataContainer + index;
    b2BodyId bodyId = b2CreateBody( worldId, &bodyDef );
    _registry.emplace<BallCollision>(entity, bodyId, 0.0f);
    
    // Create shape for body
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.material.restitution = 0.6f;
    shapeDef.density = 1500.0f;
    // No events when balls hit each other
    shapeDef.enableContactEvents = false;

    b2Circle circle = { { 0.0f, 0.0f }, 1.4f };
    b2CreateCircleShape( bodyId, &shapeDef, &circle );

    return {
    	.index = index,
    	.entId = entity,
    	.bodyId = bodyId,
    };
}

void Ball::render(BallCollision& col, BallFrame& tex) const {
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
    Vector2 p = screen.b2Vec2ToVector2(pos);
    
    DrawTexturePro(tex.texture, tex.frame, { p.x, p.y, BALL_SIZE, BALL_SIZE }, ballExtent, angle, WHITE);
}

void Ball::update(BallCollision& col, BallFrame& tex) {
    b2Vec2 vel = b2Body_GetLinearVelocity(col.bodyId);
    int speed = static_cast<int>(fabsf(b2Length(vel)))*10;

    // TODO: figure out if this needs to be the FPS, and
    // whether to pass through with Screen for Web (how to find out Web FPS?)
    if (speed > 120) {
        speed = 120;
    } else if (speed < 1) {
        if (speed < 0.000001f) {
            return;
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

void Ball::unload() {
    UnloadTexture(ballTexture);
}
