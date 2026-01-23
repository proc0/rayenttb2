#pragma once

#include <box2d/types.h>
#include <raylib.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define PIXELS_PER_METER 10.0f

class Screen {

public:
	int width;
	int height;
	int halfWidth;
	int halfHeight;
	float pixelsPerMeter;

	Screen(int _width, int _height, float _pixelsPerMeter):
	width(_width),
	height(_height),
	halfWidth(_width/2),
	halfHeight(_height/2),
	pixelsPerMeter(_pixelsPerMeter) {};
	~Screen() = default;

	Vector2 b2Vec2ToVector2(b2Vec2) const;
	b2Vec2 vector2Tob2Vec2(Vector2) const;
	void resize(int _width, int _height);
};
