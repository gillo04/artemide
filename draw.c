#include "draw.h"

void draw_arrow(Vector2 start, Vector2 end, Color color) {
    DrawLineEx(start, end, ARROW_THICKNESS, color);

    Vector2 dir = Vector2Scale(
                    Vector2Normalize(
                        Vector2Subtract(end, start)), 20);

    DrawLineEx(end, Vector2Add(end, Vector2Rotate(dir, PI*3/4)), ARROW_THICKNESS, color);
    DrawLineEx(end, Vector2Add(end, Vector2Rotate(dir, -PI*3/4)), ARROW_THICKNESS, color);
}

void draw_resistor(Vector2 start, Vector2 end) {
    Vector2 dir = Vector2Normalize(Vector2Subtract(end, start));
    Vector2 up_dir = Vector2Rotate(dir, PI*1/3);
    Vector2 down_dir = Vector2Rotate(dir, -PI*1/3);

    // Draw wire a
    float wire_len = Vector2Length(Vector2Subtract(end, start)) / 2 - 30;

    Vector2 cur_start = Vector2Add(start, Vector2Scale(dir, wire_len));
    DrawLineEx(start, cur_start, ARROW_THICKNESS, (Color) {0, 0, 0, 255});

    // Draw resistor
    Vector2 tmp = Vector2Add(cur_start, Vector2Scale(down_dir, RESISTOR_HEIGHT/2));
    DrawLineEx(cur_start, tmp, ARROW_THICKNESS, (Color) {0, 0, 0, 255});
    cur_start = tmp;

    Vector2 *vec = &up_dir;
    for (int i = 0; i < 5; i++) {
        tmp = Vector2Add(cur_start, Vector2Scale(*vec, RESISTOR_HEIGHT));
        DrawLineEx(cur_start, tmp, ARROW_THICKNESS, (Color) {0, 0, 0, 255});
        cur_start = tmp;

        if (vec == &down_dir) {
            vec = &up_dir;
        } else {
            vec = &down_dir;
        }
    }
    tmp = Vector2Add(cur_start, Vector2Scale(*vec, RESISTOR_HEIGHT/2));
    DrawLineEx(cur_start, tmp, ARROW_THICKNESS, (Color) {0, 0, 0, 255});
    cur_start = tmp;

    // Draw wire b
    DrawLineEx(cur_start, end, ARROW_THICKNESS, (Color) {0, 0, 0, 255});
}

void draw_wire(Vector2 start, Vector2 end) {
    DrawLineEx(start, end, ARROW_THICKNESS, (Color) {0, 0, 0, 255});
}
