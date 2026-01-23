#pragma once

#include <raylib.h> // IWYU pragma: export
#include <entt/entt.hpp> // IWYU pragma: export
#include <box2d/id.h> // IWYU pragma: export
#include <box2d/box2d.h> // IWYU pragma: export

#include "global.hpp" // IWYU pragma: export
#include "config.h" // IWYU pragma: export
#include "screen.hpp"

#define TEXTURE_BALL_URI "ball.png"
#define BALL_SIZE 32
#define BALL_EXTENT Vector2({ BALL_SIZE/2.0f, BALL_SIZE/2.0f })

typedef struct BallId {} BallId;
typedef struct BallCollision { 
	b2BodyId bodyId; 
	float angle; 
} BallCollision;
typedef struct BallFrame {
    Rectangle frame = { 0, 0, BALL_SIZE, BALL_SIZE };
    Texture2D& texture;
    int frameCounter = 0;
    int frameIndex = 0;
    int frameSpeed = 8;
} BallFrame;

struct BallUserData {
    int index;
    entt::entity entId;
    b2BodyId bodyId;
};

class Ball {
    Texture2D ballTexture;
	Screen& screen;
	Vector2 ballExtent = BALL_EXTENT;

public:
	Ball(Screen& _screen) : screen(_screen) {};
	~Ball() = default;

	void load();
	BallUserData create(entt::registry&, b2WorldId, BallUserData* userDataContainer, int index);
	void render(BallCollision&, BallFrame&) const;
	void update(BallCollision&, BallFrame&);
	void unload();
};
