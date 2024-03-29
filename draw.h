#pragma once
#include "raylib.h"
#include "raymath.h"

#define SCREEN_WIDTH 2000
#define SCREEN_HEIGHT 1000
#define ARROW_THICKNESS 4.
#define RESISTOR_HEIGHT 24
#define WIRE_COLOR (Color) {0, 0, 0, 255}

void init_draw();

void draw_arrow(Vector2 start, Vector2 end, Color color);

void draw_resistor(Vector2 start, Vector2 end, float val);

void draw_wire(Vector2 start, Vector2 end);

void draw_terminal(Vector2 start, Vector2 end);

void draw_legend(int current_component);

void draw_component(Vector2 a, Vector2 b, int type, float value);
