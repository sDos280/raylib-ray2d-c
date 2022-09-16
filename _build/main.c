
#include "raylib.h"
#include "raymath.h"
#include "Ray2DExtetion.h"
#define RAYLENGTH 1000000

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "2d ray caster");

    Ray2D ray = (Ray2D){ 0 };
    Ray2DCollision collisions[4] = { 0 }; // id: 0->collisionCircle 1->collisionLine 2->collisionTriangle 3->collisionRectangle
    Vector2 pos = { screenWidth * 0.5, screenHeight * 0.5 };
    ray.direction = (Vector2){ 0, 0 };
    ray.position = pos;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsKeyDown(KEY_W)) pos.y -= 5;
        if (IsKeyDown(KEY_S)) pos.y += 5;
        if (IsKeyDown(KEY_A)) pos.x -= 5;
        if (IsKeyDown(KEY_D)) pos.x += 5;

        ray.position = pos;

        ray.direction = Vector2Normalize(Vector2Subtract(GetMousePosition(), ray.position));
        collisions[0] = GetRay2DCollisionCircle(ray, (Vector2) { 50, 50 }, 50);
        collisions[1] = GetRay2DCollisionLineSegment(ray, (Vector2) { screenWidth * 0.5, 100 }, (Vector2) { screenWidth * 0.5 - 50, screenHeight - 100 });
        collisions[2] = GetRay2DCollisionTriangle(ray, (Vector2) { screenWidth * 0.5 + 100, 100 }, (Vector2) { screenWidth * 0.5, screenHeight - 100 }, (Vector2) { screenWidth * 0.5 + 300, screenHeight - 200 });
        collisions[3] = GetRay2DCollisionRectangle(ray, (Rectangle) { 100, 100, 200, 300 });

        Ray2DCollision collision = { 0 };
        collision.distance = RAYLENGTH;

        for (int i = 0; i < 4; i++) {
            if (collisions[i].hit)
            {
                // Save the closest hit edge
                if (collision.distance >= collisions[i].distance) collision = collisions[i];
            }
        }


        BeginDrawing();

        ClearBackground(RAYWHITE);


        if (collision.hit) {
            DrawLineV(ray.position, collision.point, BLUE);
            DrawLineV(collision.point, Vector2Add(collision.point, Vector2Scale(collision.normal, 20)), GREEN);
        }

        DrawCircleLines(50, 50, 50, RED);
        DrawLineV((Vector2) { screenWidth * 0.5, 100 }, (Vector2) { screenWidth * 0.5 - 50, screenHeight - 100 }, RED);
        DrawTriangleLines((Vector2) { screenWidth * 0.5 + 100, 100 }, (Vector2) { screenWidth * 0.5, screenHeight - 100 }, (Vector2) { screenWidth * 0.5 + 300, screenHeight - 200 }, RED);
        DrawRectangleLines(100, 100, 200, 300, RED);
        DrawCircleV(pos, 5, RED);

        DrawText(TextFormat("<%f, %f>", ray.position.x, ray.position.y), 10, 10, 10, GREEN);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}