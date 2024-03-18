#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>

#include "raylib.h"
#include "raymath.h"

#include "circuit.h"
#include "draw.h"

#define SNAP_DIST 30

enum {
    S_IDLE,
    S_TRACING,
    S_WRITING
};

int main(int argc, char* argv[]) {
    Circuit circ;
    if (argc == 2) {
        circ = Circuit(argv[1]);
    }

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Artemide");

    // State
    int state = S_IDLE;
    int current_component = C_WIRE;

    int selected = -1;
    Vector2 tmp_a, tmp_b;
    string tmp_value;
    while (!WindowShouldClose()) {
        int snap = circ.get_snap(GetMousePosition(), 0);
        
        // Events and updates 
        switch (state) {
            case S_IDLE:
                // Start drawing component
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

                // Change current component
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
                    case KEY_THREE:
                        current_component = C_TERMINAL;
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
                break;
            case S_TRACING:
                // Finish drawing component
                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                    state = S_IDLE;

                    if (current_component != C_SELECT) {
                        circ.add_component(current_component, tmp_a, tmp_b, 0);
                    }

                    if (current_component == C_RESISTOR) {
                        tmp_value = "";
                        state = S_WRITING;
                    }
                }

                // Update component position
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
            case S_WRITING:
                // Handle resistor value input
                char key = GetCharPressed();
                if (key > 0) {
                    tmp_value += key;
                }

                if (IsKeyPressed(KEY_BACKSPACE)) {
                    if (tmp_value.size() > 0) {
                        tmp_value.pop_back();
                    }
                }

                if (IsKeyPressed(KEY_ENTER)) {
                    circ.comps.back().value = stof(tmp_value);
                    state = S_IDLE;
                }
                break;
        }

        // Draw
        BeginDrawing();
        ClearBackground((Color) {255, 255, 255, 255});
        draw_legend(current_component);
        circ.draw();

        switch (state) {
            case S_IDLE:
                if (snap != -1) {
                    DrawCircleV(circ.pts[snap].pos, 10, (Color) {0, 150, 0, 255});
                }
                break;
            case S_TRACING:
                if (snap != -1) {
                    DrawCircleV(circ.pts[snap].pos, 10, (Color) {0, 150, 0, 255});
                }
                if (current_component == C_SELECT) {
                    DrawCircleV(circ.pts[selected].pos, 10, (Color) {0, 0, 150, 255});
                }
                draw_component(tmp_a, tmp_b, current_component, 0);
                break;
            case S_WRITING:
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color) {0, 0, 0, 100});
                DrawText(tmp_value.c_str(), 300, 300, 50, (Color) {0, 0, 0, 255});
                break;
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
