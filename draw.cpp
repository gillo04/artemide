#include "draw.h"
#include <iostream> 
#include <stdlib.h> 

#include "circuit.h"

Font font;
void init_draw() {
    font = LoadFontEx("./tnr.ttf", 60, 0, 0);
}

void draw_arrow(Vector2 start, Vector2 end, Color color) {
    DrawLineEx(start, end, ARROW_THICKNESS, color);

    Vector2 dir = Vector2Scale(
                    Vector2Normalize(
                        Vector2Subtract(end, start)), 15);

    DrawLineEx(end, Vector2Add(end, Vector2Rotate(dir, PI*3/4)), ARROW_THICKNESS, color);
    DrawLineEx(end, Vector2Add(end, Vector2Rotate(dir, -PI*3/4)), ARROW_THICKNESS, color);
}

void draw_resistor(Vector2 start, Vector2 end, float val) {
    Vector2 dir = Vector2Normalize(Vector2Subtract(end, start));
    Vector2 up_dir = Vector2Rotate(dir, PI*1/3);
    Vector2 down_dir = Vector2Rotate(dir, -PI*1/3);

    // Draw wire a
    float wire_len = Vector2Length(Vector2Subtract(end, start)) / 2 - 30;

    Vector2 cur_start = Vector2Add(start, Vector2Scale(dir, wire_len));
    DrawLineEx(start, cur_start, ARROW_THICKNESS, WIRE_COLOR);

    // Draw resistor
    Vector2 tmp = Vector2Add(cur_start, Vector2Scale(down_dir, RESISTOR_HEIGHT/2));
    DrawLineEx(cur_start, tmp, ARROW_THICKNESS, WIRE_COLOR);
    cur_start = tmp;

    Vector2 *vec = &up_dir;
    for (int i = 0; i < 5; i++) {
        tmp = Vector2Add(cur_start, Vector2Scale(*vec, RESISTOR_HEIGHT));
        DrawLineEx(cur_start, tmp, ARROW_THICKNESS, WIRE_COLOR);
        cur_start = tmp;

        if (vec == &down_dir) {
            vec = &up_dir;
        } else {
            vec = &down_dir;
        }
    }
    tmp = Vector2Add(cur_start, Vector2Scale(*vec, RESISTOR_HEIGHT/2));
    DrawLineEx(cur_start, tmp, ARROW_THICKNESS, WIRE_COLOR);
    cur_start = tmp;

    // Draw wire b
    DrawLineEx(cur_start, end, ARROW_THICKNESS, WIRE_COLOR);

    // Draw label
    Vector2 middle = Vector2Add(Vector2Scale(Vector2Subtract(end, start), 0.5f), start);
    // Ω
    char buff[14];
    sprintf(buff, "%g", val);
    DrawTextEx(font, buff, Vector2Add(middle, (Vector2) {20, 20}), 35, 0, WIRE_COLOR);
}

void draw_wire(Vector2 start, Vector2 end) {
    DrawLineEx(start, end, ARROW_THICKNESS, WIRE_COLOR);
}

void draw_terminal(Vector2 start, Vector2 end) {
    DrawLineEx(start, end, ARROW_THICKNESS, WIRE_COLOR);
    DrawCircleV(end, 10 + ARROW_THICKNESS, WIRE_COLOR);
    DrawCircleV(end, 10, (Color) {255, 255, 255, 255});
}

void draw_generator_circle(Vector2 start, Vector2 end) {
    DrawLineEx(start, end, ARROW_THICKNESS, WIRE_COLOR);

    Vector2 center = Vector2Add(Vector2Scale(Vector2Subtract(end, start), 0.5), start);
    DrawCircleV(center, 35 + ARROW_THICKNESS, WIRE_COLOR);
    DrawCircleV(center, 35, (Color) {255, 255, 255, 255});
}

void draw_tension_generator(Vector2 start, Vector2 end, float val) {
    DrawLineEx(start, end, ARROW_THICKNESS, WIRE_COLOR);

    Vector2 center = Vector2Add(Vector2Scale(Vector2Subtract(end, start), 0.5), start);

    draw_generator_circle(start, end);
    float angle = Vector2Angle(end, start) * RAD2DEG;
    DrawTextPro(font, "+", center, (Vector2) {12, 40}, angle-90, 40, 0, WIRE_COLOR);
    DrawTextPro(font, "|", center, (Vector2) {20, 20}, angle+180, 40, 0, WIRE_COLOR);

    char buff[14];
    sprintf(buff, "%g", val);
    DrawTextEx(font, buff, Vector2Add(center, (Vector2) {30, 30}), 35, 0, WIRE_COLOR);
}

void draw_current_generator(Vector2 start, Vector2 end, float val) {
    DrawLineEx(start, end, ARROW_THICKNESS, WIRE_COLOR);

    Vector2 center = Vector2Add(Vector2Scale(Vector2Subtract(end, start), 0.5), start);
    Vector2 dir = Vector2Scale(Vector2Normalize(Vector2Subtract(end, start)), 25);

    draw_generator_circle(start, end);
    draw_arrow(Vector2Subtract(center, dir), Vector2Add(dir, center), WIRE_COLOR);

    char buff[14];
    sprintf(buff, "%g", val);
    DrawTextEx(font, buff, Vector2Add(center, (Vector2) {30, 30}), 35, 0, WIRE_COLOR);
}

void draw_legend(int current_component) {
    char* legend[] {
        "0: Seleziona",
        "1: Filo",
        "2: Resistenza",
        "3: Terminale",
        "4: Generatore di tensione",
        "5: Generatore di corrente",
    };

    for (int i = 0; i < 6; i++) {
        Color c = (Color) {0, 0, 0, 255};
        if (current_component == i) {
            c = (Color) {255, 0, 0, 255};
        }
        DrawText(legend[i], 10, 10 + 50*i, 40, c);
    }
}

void draw_component(Vector2 a, Vector2 b, int type, float value) {
    switch (type) {
        case C_WIRE:
            draw_wire(a, b);
            break;
        case C_RESISTOR:
            draw_resistor(a, b, value);
            break;
        case C_TERMINAL:
            draw_terminal(a, b);
            break;
        case C_TENSION_GEN:
            draw_tension_generator(a, b, value);
            break;
        case C_CURRENT_GEN:
            draw_current_generator(a, b, value);
            break;
    }
}

