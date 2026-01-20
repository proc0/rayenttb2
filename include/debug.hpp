#include "box2d/box2d.h"
#include "box2d/types.h"
#include "raylib.h"

class Debug {
    b2DebugDraw draw;
    Vector2 offset = {0};
    float const pixelsPerMeter;
    int screenWidth;
    int screenHeight;

public:
    Debug(float ppm = 10.0f) : pixelsPerMeter(ppm) {
        // Initialize with defaults
        draw = b2DefaultDebugDraw();
        draw.drawShapes = true;
        draw.drawJoints = true;
        draw.context = this;
        
        // Draw callbacks
        draw.DrawPolygonFcn = [](const b2Vec2* vertices, int vertexCount, b2HexColor color, void* context) {
            auto* const self = static_cast<Debug*>(context);
            for (int i = 0; i < vertexCount; i++) {
                Vector2 p1 = self->mapVector(vertices[i]);
                Vector2 p2 = self->mapVector(vertices[(i + 1) % vertexCount]);
                
                DrawLineV(p1, p2, self->mapColor(color));
            }
        };
        
        draw.DrawSolidPolygonFcn = [](b2Transform transform, const b2Vec2* vertices, int vertexCount, float radius, b2HexColor color, void* context) {
            auto* const self = static_cast<Debug*>(context);
            Color fillColor = self->mapColor(color);
            
            for (int i = 0; i < vertexCount; i++) {
                b2Vec2 worldVert = b2TransformPoint(transform, vertices[i]);
                Vector2 p1 = self->mapVector(worldVert);
                b2Vec2 worldVert2 = b2TransformPoint(transform, vertices[(i + 1) % vertexCount]);
                Vector2 p2 = self->mapVector(worldVert2);
                
                DrawLineV(p1, p2, fillColor);
            }
        };
        
        draw.DrawCircleFcn = [](b2Vec2 center, float radius, b2HexColor color, void* context) {
            auto* const self = static_cast<Debug*>(context);
            Vector2 c = self->mapVector(center);
            
            DrawCircleLinesV(c, radius * self->pixelsPerMeter, self->mapColor(color));
        };
        
        draw.DrawSolidCircleFcn = [](b2Transform transform, float radius, b2HexColor color, void* context) {
            auto* const self = static_cast<Debug*>(context);
            Vector2 c = self->mapVector(transform.p);
            Color fillColor = self->mapColor(color);
            
            DrawCircleV(c, radius * self->pixelsPerMeter, fillColor);
            DrawCircleLinesV(c, radius * self->pixelsPerMeter, self->mapColor(color));
            
            b2Vec2 axis = b2RotateVector(transform.q, {radius, 0.0f});
            Vector2 p = self->mapVector(b2Add(transform.p, axis));
            
            DrawLineV(c, p, self->mapColor(color));
        };
        
        draw.DrawLineFcn = [](b2Vec2 p1, b2Vec2 p2, b2HexColor color, void* context) {
            auto* const self = static_cast<Debug*>(context);
            
            DrawLineV(self->mapVector(p1), self->mapVector(p2), self->mapColor(color));
        };
        
        draw.DrawTransformFcn = [](b2Transform transform, void* context) {
            auto* const self = static_cast<Debug*>(context);
            const float axisScale = 0.4f;
            
            b2Vec2 p1 = transform.p;
            b2Vec2 xAxis = b2RotateVector(transform.q, {axisScale, 0.0f});
            b2Vec2 p2 = b2Add(p1, xAxis);
            
            DrawLineV(self->mapVector(p1), self->mapVector(p2), RED);
            
            b2Vec2 yAxis = b2RotateVector(transform.q, {0.0f, axisScale});
            p2 = b2Add(p1, yAxis);
            
            DrawLineV(self->mapVector(p1), self->mapVector(p2), GREEN);
        };
        
        draw.DrawPointFcn = [](b2Vec2 p, float size, b2HexColor color, void* context) {
            auto* const self = static_cast<Debug*>(context);
            Vector2 point = self->mapVector(p);
            
            DrawCircleV(point, size, self->mapColor(color));
        };
        
        draw.DrawStringFcn = [](b2Vec2 p, const char* s, b2HexColor color, void* context) {
            auto* const self = static_cast<Debug*>(context);
            Color fillColor = self->mapColor(color);
            Vector2 point = self->mapVector(p);

            DrawText(s, point.x, point.y, 10, fillColor);
        };
    }
    
    Vector2 mapVector(b2Vec2 vec) const {
        return {
            offset.x + vec.x * pixelsPerMeter,
            offset.y - vec.y * pixelsPerMeter
        };
    }
    
    Color mapColor(b2HexColor hexColor) const {
        unsigned char r = (hexColor >> 16) & 0xFF;
        unsigned char g = (hexColor >> 8) & 0xFF;
        unsigned char b = hexColor & 0xFF;
        return {r, g, b, 255};
    }

    void resize(int width, int height) {
        screenWidth = width;
        screenHeight = height;
        offset = { screenWidth / 2.0f, screenHeight / 2.0f };
    }
    
    void render(b2WorldId worldId) {
        b2World_Draw(worldId, &draw);
    }
};
