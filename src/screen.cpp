#include "screen.hpp"

Vector2 Screen::b2Vec2ToVector2(b2Vec2 vec) const {
    return {
        halfWidth + vec.x * pixelsPerMeter,
        halfHeight - vec.y * pixelsPerMeter
    };
}

b2Vec2 Screen::vector2Tob2Vec2(Vector2 vec) const {
    return {
        (vec.x - halfWidth) / pixelsPerMeter,
        (halfHeight - vec.y) / pixelsPerMeter
    };
}

void Screen::resize(int _width, int _height) {
	width = _width;
	height = _height;
	halfWidth = _width/2;
	halfHeight = _height/2;
}
