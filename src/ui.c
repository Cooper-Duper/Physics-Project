#include <raylib.h>

typedef struct field {
    char* formatString;
} MenuField;

typedef struct menStruct {
    int x;
    int y;
    int w;
    int h;
    char* title;
    Color bgColor;
    Color textColor;
    MenuField* fields;
} Menu;

void drawMenu(Menu menu, Camera2D camera, Font font) {
    BeginDrawing();
    BeginMode2D(camera);
    DrawRectangle(menu.x, menu.y, menu.w, menu.h, menu.bgColor);
    DrawTextEx(font, menu.title, (Vector2) {menu.x + 5, menu.y + 5}, 30.0, 1.0, menu.textColor);
}
