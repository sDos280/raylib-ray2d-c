/*******************************************************************************************
*
*   2d ray implementation
*
********************************************************************************************/

#ifndef RAY2D_COLLISION_IMPLEMENTATION
#define RAY2D_COLLISION_IMPLEMENTATION

#include "raylib.h"
#include "raymath.h"

// Ray2D, ray2D for raycasting
typedef struct Ray2D {
    Vector2 position;       // Ray position (origin)
    Vector2 direction;      // Ray direction
} Ray2D;

// Ray2DCollision, ray2D hit information
typedef struct Ray2DCollision {
    bool hit;               // Did the ray hit something?
    float distance;         // Distance to nearest hit
    Vector2 point;          // Point of nearest hit
    Vector2 normal;         // Surface normal of hit
} Ray2DCollision;

void DrawRay2D(Ray2D ray, Color color); // Draw a ray2d line
Ray2DCollision GetRay2DCollisionLineSegment(Ray2D ray, Vector2 p1, Vector2 p2);               // Get collision info between ray2D and a line segment 
Ray2DCollision GetRay2DCollisionCircle(Ray2D ray, Vector2 center, float radius);              // Get collision info between ray2D and circle
Ray2DCollision GetRay2DCollisionRectangle(Ray2D ray, Rectangle rect);                         // Get collision info between ray2D and rectangle
Ray2DCollision GetRay2DCollisionTriangle(Ray2D ray, Vector2 p1, Vector2 p2, Vector2 p3);      // Get collision info between ray2D and triangle

float Sign(float value);
Vector2 Vector2CrossProduct(Vector2 v);

// Draw a ray2d line
void DrawRay2D(Ray2D ray, Color color)
{
    float scale = 1000;

    DrawLine(ray.position.x, ray.position.y, ray.position.x + ray.direction.x * scale, ray.position.y + ray.direction.y * scale, color);
}

// source: http://www.jeffreythompson.org/collision-detection/line-line.php
// Get collision info between ray2d and a line segment 
Ray2DCollision GetRay2DCollisionLineSegment(Ray2D ray, Vector2 p1, Vector2 p2)
{
#define RAYLENGTH 1000000
    Ray2DCollision collision = { 0 };

    Vector2 rayEndPoint = Vector2Add(ray.position, Vector2Scale(ray.direction, RAYLENGTH));

    float uADenominator = (p2.y - p1.y) * (ray.position.x - rayEndPoint.x) - (p2.x - p1.x) * (ray.position.y - rayEndPoint.y);
    float uBDenominator = (p2.y - p1.y) * (ray.position.x - rayEndPoint.x) - (p2.x - p1.x) * (ray.position.y - rayEndPoint.y);
    if (uADenominator != 0 && uBDenominator != 0) {
        float uA = ((p2.x - p1.x) * (rayEndPoint.y - p1.y) - (p2.y - p1.y) * (rayEndPoint.x - p1.x)) / uADenominator;
        float uB = ((ray.position.x - rayEndPoint.x) * (rayEndPoint.y - p1.y) - (ray.position.y - rayEndPoint.y) * (rayEndPoint.x - p1.x)) / uBDenominator;
        if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {
            collision.hit = true;
            collision.point = (Vector2) { rayEndPoint.x + (uA * (ray.position.x - rayEndPoint.x)), rayEndPoint.y + (uA * (ray.position.y - rayEndPoint.y)) };
            collision.distance = Vector2Length(Vector2Subtract(collision.point, ray.position));
            float OnWhichSide = Sign((ray.position.x - p1.x) * (-p2.y + p1.y) + (ray.position.y - p1.y) * (p2.x - p1.x));
            if (OnWhichSide == 1) {
                collision.normal = Vector2Negate(Vector2CrossProduct(Vector2Normalize(Vector2Subtract(p1, p2))));
            }
            else if (OnWhichSide == -1) {
                collision.normal = Vector2CrossProduct(Vector2Normalize(Vector2Subtract(p1, p2)));
            }
            /*else if (OnWhichSide == 0) {
                // no normal, on the line
            }*/
        }
        else
        {
            collision.hit = false;
        }
    }
    else
    {
        collision.hit = false;
    }

    return collision;
}

// source: https://www.bluebill.net/circle_ray_intersection.html
// Get collision info between ray2d and circle
Ray2DCollision GetRay2DCollisionCircle(Ray2D ray, Vector2 center, float radius)
{
    Ray2DCollision collision = { 0 };

    Vector2 u = Vector2Subtract(center, ray.position);
    Vector2 u1 = Vector2Scale(ray.direction, Vector2DotProduct(u, ray.direction));
    Vector2 u2 = Vector2Subtract(u, u1);

    float d = Vector2Length(u2);

    collision.hit = d <= radius;

    float m = sqrtf(radius * radius - d * d);

    // Check if ray origin is inside the circle to calculate the correct collision point
    if (Vector2Length(u) > radius)
    {
        Vector2 p2 = Vector2Add(ray.position, Vector2Add(u1, Vector2Scale(ray.direction, -m)));

        collision.distance = Vector2Length(Vector2Subtract(p2, ray.position));

        // Calculate collision point
        collision.point = p2;

        // Calculate collision normal (pointing outwards)
        collision.normal = Vector2Normalize(Vector2Subtract(collision.point, center));
    }
    else
    {
        Vector2 p1 = Vector2Add(ray.position, Vector2Add(u1, Vector2Scale(ray.direction, +m)));

        collision.distance = Vector2Length(Vector2Subtract(p1, ray.position));

        // Calculate collision point
        collision.point = p1;

        // Calculate collision normal (pointing outwards)
        collision.normal = Vector2Negate(Vector2Normalize(Vector2Subtract(collision.point, center)));
    }

    return collision;
}

// Get collision info between ray and Rectangle
Ray2DCollision GetRay2DCollisionRectangle(Ray2D ray, Rectangle rect)
{
#define RAYLENGTH 1000000
    Ray2DCollision edgesCollision[4] = { 0 };
    edgesCollision[0] = GetRay2DCollisionLineSegment(ray,  (Vector2) { rect.x, rect.y }, (Vector2) {rect.x + rect.width, rect.y});
    edgesCollision[1] = GetRay2DCollisionLineSegment(ray,  (Vector2) { rect.x + rect.width, rect.y }, (Vector2) { rect.x + rect.width, rect.y + rect.height });
    edgesCollision[2] = GetRay2DCollisionLineSegment(ray,  (Vector2) { rect.x + rect.width, rect.y + rect.height }, (Vector2) { rect.x, rect.y + rect.height });
    edgesCollision[3] = GetRay2DCollisionLineSegment(ray,  (Vector2) { rect.x, rect.y + rect.height }, (Vector2) { rect.x, rect.y });

    Ray2DCollision collision = { 0 };
    collision.distance = RAYLENGTH;

    for (int i = 0; i < 4; i++) {
        if (edgesCollision[i].hit)
        {
            // Save the closest hit edge
            if (collision.distance >= edgesCollision[i].distance) collision = edgesCollision[i];
        }
    }

    return collision;
}

// Get collision info between ray and triangle
Ray2DCollision GetRay2DCollisionTriangle(Ray2D ray, Vector2 p1, Vector2 p2, Vector2 p3)
{
#define RAYLENGTH 1000000
    Ray2DCollision edgesCollision[3] = { 0 };
    edgesCollision[0] = GetRay2DCollisionLineSegment(ray, p1, p2);
    edgesCollision[1] = GetRay2DCollisionLineSegment(ray, p2, p3);
    edgesCollision[2] = GetRay2DCollisionLineSegment(ray, p3, p1);

    Ray2DCollision collision = { 0 };
    collision.distance = RAYLENGTH;

    for (int i = 0; i < 3; i++) {
        if (edgesCollision[i].hit)
        {
            // Save the closest hit edge
            if (collision.distance >= edgesCollision[i].distance) collision = edgesCollision[i];
        }
    }

    return collision;
}

// Get sign value
float Sign(float value)
{
    if (value > 0.0f) return 1.0f;
    if (value < 0.0f) return -1.0f;
    return 0.0f;
}

Vector2 Vector2CrossProduct(Vector2 v)
{
    Vector2 result = { -v.y, v.x };

    return result;
}

#endif // !RAY2D_COLLISION_IMPLEMENTATION

