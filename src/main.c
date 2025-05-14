#include <raylib.h>
#include <stdio.h>
#include <limits.h>
#include "utils.c"

int main(int argc, char** argv) {
    //SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
    SetTargetFPS(60);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(500, 500, "Cooper's Basic Physics Simulator");

    //Loading the font used for displaying values
    //Font pointFont = LoadFontFromMemory(".ttf", fontData, int dataSize, int fontSize, int *codepoints, int codepointCount);
    Camera2D camera = {0};
    camera.target = (Vector2) {0.0f, 0.0f};
    camera.offset = (Vector2) {200.0f, 200.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    int rectY = 0;
    int rectX = 0;
    const float panSpeed = 6.0f;
    float panScale;
    const float zoomScale = 0.2f;

    PointArr points = (PointArr) {
        .len = 10,
        .ptr = malloc(points.len * sizeof(PhysPoint)),
        .nextAddr = 0
    };

    Vector2 mouseDelta;
    //Main loop
    while(!WindowShouldClose()) {


        if (IsKeyDown(KEY_DOWN)) rectY += 2;
        if (IsKeyDown(KEY_UP)) rectY -= 2;
        if (IsKeyDown(KEY_LEFT)) rectX -= 2;
        if (IsKeyDown(KEY_RIGHT)) rectX += 2;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            puts("Mouse Pressed");
            Vector2 worldPos = GetScreenToWorld2D(GetMousePosition(), camera);
            PhysPoint new = {
                .position.x = (double) worldPos.x,
                .position.y = (double) worldPos.y,
                .mass = (double) (random() % 60) * 10000000.0 + 1.0
            };
            PointArrAdd(&points, new);
        }

        if (IsKeyDown(KEY_F)) camera.target = (Vector2) {rectX, rectY};
        if (IsKeyDown(KEY_LEFT_CONTROL)) {
            camera.zoom += GetMouseWheelMove() * zoomScale * camera.zoom;
            if (camera.zoom < 0.1) {
                camera.zoom = 0.1;
            }
        } else {
            mouseDelta = GetMouseWheelMoveV();
            panScale = panSpeed / camera.zoom;
            camera.target.x -= mouseDelta.x * panScale;
            camera.target.y -= mouseDelta.y * panScale;
        }


        
        //Breaks with more than 17 for some reason...
        for (int i = 0; i < points.nextAddr; i++) {
            for (int j = i + 1; j < points.nextAddr; j++) {
                updateGravObjects(&points.ptr[i], &points.ptr[j]);
            }
        }
        

        for (int i = 0; i < points.nextAddr; i++) {
            points.ptr[i].position.x += points.ptr[i].velocity.x;
            points.ptr[i].position.y += points.ptr[i].velocity.y;

            points.ptr[i].velocity.x += points.ptr[i].acceleration.x;
            points.ptr[i].velocity.y += points.ptr[i].acceleration.y;
<<<<<<< HEAD
=======
            //printf("(%.30lf, %.30lf)\n", points.ptr[i].acceleration.x, points.ptr[i].acceleration.y);
>>>>>>> 84287a0 (Trying to sync)
        }

        ClearBackground((Color) {0, 0, 0, 255});
        BeginDrawing();
        DrawLine(0, 0, GetMouseX(), GetMouseY(), (Color) {255, 255, 255, 255});
        BeginMode2D(camera);
        for (int i = 0; i < points.nextAddr; i++) {
            DrawCircleLines(points.ptr[i].position.x, points.ptr[i].position.y, 
                    points.ptr[i].mass/10000000, YELLOW);
        }

        DrawRectangle(rectX, rectY, 50, 50, (Color) {120, 200, 50, 255});
        DrawCircle(0, 0, 10.0f, GREEN);
        EndMode2D();
        EndDrawing();
    }
}
