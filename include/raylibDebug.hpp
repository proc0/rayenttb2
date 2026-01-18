#include "box2d/box2d.h"
#include "raylib.h"

class RaylibDebugDraw {
private:
    float pixelsPerMeter;
    Vector2 offset = { 640.0f, 360.0f };
    b2DebugDraw draw;  // Store as member variable
    
    Vector2 B2ToScreen(b2Vec2 vec) const {
        return {
            offset.x + vec.x * pixelsPerMeter,
            offset.y - vec.y * pixelsPerMeter
        };
    }
    
    Color B2ColorToRaylib(b2HexColor hexColor) const {
        unsigned char r = (hexColor >> 16) & 0xFF;
        unsigned char g = (hexColor >> 8) & 0xFF;
        unsigned char b = hexColor & 0xFF;
        return {r, g, b, 255};
    }

public:
    RaylibDebugDraw(float ppm = 10.0f) : pixelsPerMeter(ppm) {
        // offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
        
        // Initialize with defaults
        draw = b2DefaultDebugDraw();
        draw.drawShapes = true;
        draw.drawJoints = true;
        draw.context = this;
        
        // Set up all callbacks once
        draw.DrawPolygonFcn = [](const b2Vec2* vertices, int vertexCount, b2HexColor color, void* context) {
            auto* self = static_cast<RaylibDebugDraw*>(context);
            for (int i = 0; i < vertexCount; i++) {
                Vector2 p1 = self->B2ToScreen(vertices[i]);
                Vector2 p2 = self->B2ToScreen(vertices[(i + 1) % vertexCount]);
                DrawLineV(p1, p2, self->B2ColorToRaylib(color));
            }
        };
        
        draw.DrawSolidPolygonFcn = [](b2Transform transform, const b2Vec2* vertices, int vertexCount, 
                                   float radius, b2HexColor color, void* context) {
            auto* self = static_cast<RaylibDebugDraw*>(context);
            Color fillColor = self->B2ColorToRaylib(color);
            fillColor.a = 128;
            
            for (int i = 0; i < vertexCount; i++) {
                b2Vec2 worldVert = b2TransformPoint(transform, vertices[i]);
                Vector2 p1 = self->B2ToScreen(worldVert);
                b2Vec2 worldVert2 = b2TransformPoint(transform, vertices[(i + 1) % vertexCount]);
                Vector2 p2 = self->B2ToScreen(worldVert2);
                DrawLineV(p1, p2, self->B2ColorToRaylib(color));
            }
        };
        
        draw.DrawCircleFcn = [](b2Vec2 center, float radius, b2HexColor color, void* context) {
            auto* self = static_cast<RaylibDebugDraw*>(context);
            Vector2 c = self->B2ToScreen(center);
            DrawCircleLines(c.x, c.y, radius * self->pixelsPerMeter, self->B2ColorToRaylib(color));
        };
        
        draw.DrawSolidCircleFcn = [](b2Transform transform, float radius, b2HexColor color, void* context) {
            auto* self = static_cast<RaylibDebugDraw*>(context);
            Vector2 c = self->B2ToScreen(transform.p);
            Color fillColor = self->B2ColorToRaylib(color);
            fillColor.a = 128;
            
            DrawCircle(c.x, c.y, radius * self->pixelsPerMeter, fillColor);
            DrawCircleLines(c.x, c.y, radius * self->pixelsPerMeter, self->B2ColorToRaylib(color));
            
            b2Vec2 axis = b2RotateVector(transform.q, {radius, 0.0f});
            Vector2 p = self->B2ToScreen(b2Add(transform.p, axis));
            DrawLineV(c, p, self->B2ColorToRaylib(color));
        };
        
        draw.DrawLineFcn = [](b2Vec2 p1, b2Vec2 p2, b2HexColor color, void* context) {
            auto* self = static_cast<RaylibDebugDraw*>(context);
            DrawLineV(self->B2ToScreen(p1), self->B2ToScreen(p2), self->B2ColorToRaylib(color));
        };
        
        draw.DrawTransformFcn = [](b2Transform transform, void* context) {
            auto* self = static_cast<RaylibDebugDraw*>(context);
            const float axisScale = 0.4f;
            
            b2Vec2 p1 = transform.p;
            b2Vec2 xAxis = b2RotateVector(transform.q, {axisScale, 0.0f});
            b2Vec2 p2 = b2Add(p1, xAxis);
            DrawLineV(self->B2ToScreen(p1), self->B2ToScreen(p2), RED);
            
            b2Vec2 yAxis = b2RotateVector(transform.q, {0.0f, axisScale});
            p2 = b2Add(p1, yAxis);
            DrawLineV(self->B2ToScreen(p1), self->B2ToScreen(p2), GREEN);
        };
        
        draw.DrawPointFcn = [](b2Vec2 p, float size, b2HexColor color, void* context) {
            auto* self = static_cast<RaylibDebugDraw*>(context);
            Vector2 point = self->B2ToScreen(p);
            DrawCircle(point.x, point.y, size, self->B2ColorToRaylib(color));
        };
        
        // draw.DrawStringFcn = [](b2Vec2 p, const char* s, void* context) {
        //     auto* self = static_cast<RaylibDebugDraw*>(context);
        //     Vector2 point = self->B2ToScreen(p);
        //     DrawText(s, point.x, point.y, 10, WHITE);
        // };
    }
    
    void UpdateOffset() {
        offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
    }
    
    void SetPixelsPerMeter(float ppm) {
        pixelsPerMeter = ppm;
    }
    
    // void SetFlags(bool shapes, bool joints, bool aabbs = false, bool mass = false) {
    //     draw.drawShapes = shapes;
    //     draw.drawJoints = joints;
    //     // draw.drawAABBs = aabbs;
    //     draw.drawMass = mass;
    // }
    
    void DrawWorld(b2WorldId worldId) {
        b2World_Draw(worldId, &draw);
    }
};
