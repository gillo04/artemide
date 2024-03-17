#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>

#include "raylib.h"
#include "raymath.h"

#include "circuit.h"
#include "draw.h"

#define SNAP_DIST 30

int main() {
    Circuit circ;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Artemide");

    // State
    int state = S_IDLE;
    int current_component = C_WIRE;

    int selected = -1;
    Vector2 tmp_a, tmp_b;
    while (!WindowShouldClose()) {
        int snap = circ.get_snap(GetMousePosition(), 0);

        // Handle events
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            state = S_TRACING;

            if (current_component != C_SELECT) {
                if (snap != -1) {
                    tmp_a = circ.pts[snap].pos;
                    tmp_b = circ.pts[snap].pos;
                } else {
                    tmp_a = GetMousePosition();
                    tmp_b = GetMousePosition();
                }
            } else {
                selected = snap;
            }
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            state = S_IDLE;

            if (current_component != C_SELECT) {
                circ.add_component(current_component, tmp_a, tmp_b, 5);
            }
        }

        switch (GetKeyPressed()) {
            case KEY_ZERO:
                current_component = C_SELECT;
                break;
            case KEY_ONE:
                current_component = C_WIRE;
                break;
            case KEY_TWO:
                current_component = C_RESISTOR;
                break;
            case KEY_SPACE:
                printf("Simplifying\n");
                circ.simplify();
                break;
            case KEY_ESCAPE:
                CloseWindow();
                break;
            default:
                break;
        }

        // Update state
        switch (state) {
            case S_IDLE:
                break;
            case S_TRACING:
                if (current_component != C_SELECT) {
                    if (snap != -1) {
                        tmp_b = circ.pts[snap].pos;
                    } else {
                        tmp_b = GetMousePosition();
                    }
                } else {
                    circ.pts[selected].pos = GetMousePosition();
                }
                break;
        }

        // Draw
        BeginDrawing();
        ClearBackground((Color) {255, 255, 255, 255});

        if (state == S_TRACING) {
            draw_component(tmp_a, tmp_b, current_component, 69);
        }
        draw_current_comp(current_component);
        circ.draw();

        if (snap != -1) {
            DrawCircleV(circ.pts[snap].pos, 10, (Color) {0, 150, 0, 255});
        }

        if (current_component == C_SELECT) {
            DrawCircleV(circ.pts[selected].pos, 10, (Color) {0, 0, 150, 255});
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
