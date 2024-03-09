#include <stdio.h>

#include "raylib.h"
#include "raymath.h"

#include "draw.h"

#define SNAP_DIST 30
#define MAX_NODES 50

enum {
    S_IDLE,
    S_TRACING
};

enum {
    C_WIRE,
    C_RESISTOR,
    C_NODE
};

typedef struct {
    int type;
    int a, b;
    // int id;
} Node;

int get_snap(Vector2 points[], int point_c) {
    Vector2 mouse = GetMousePosition();
    int min_dist = Vector2Length(Vector2Subtract(mouse, points[0]));
    int min_i = 0;
    for (int i = 1; i < point_c; i++) {
        int tmp = Vector2Length(Vector2Subtract(mouse, points[i]));
        if (tmp < min_dist) {
            min_dist = tmp;
            min_i = i;
        }
    }
    if (min_dist <= SNAP_DIST) {
        return min_i;
    }
    return -1;
}

int main() {
    Vector2 points[MAX_NODES*2];
    int point_c = 0;

    Node nodes[MAX_NODES];
    int node_c = 0;

    int comp_c = 0;
    int wire_c = 0;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib [core] example - basic window");

    // State
    int state = S_IDLE;
    int current_component = C_WIRE;

    Vector2 cur_start, cur_end;
    while (!WindowShouldClose()) {
        int snap = get_snap(points, point_c - (state == S_TRACING));

        // Handle events
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            state = S_TRACING;

            if (snap != -1) {
                points[point_c] = GetMousePosition();       // b
                nodes[node_c].a = snap;
                nodes[node_c].b = point_c;
                point_c += 1;
            } else {
                points[point_c] = GetMousePosition();       // a
                points[point_c+1] = GetMousePosition();     // b
                nodes[node_c].a = point_c;
                nodes[node_c].b = point_c+1;
                point_c += 2;
            }
            nodes[node_c].type = current_component;
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            state = S_IDLE;

            if (snap != -1) {
                point_c -= 1;
            }
            node_c++;
        }

        switch (GetKeyPressed()) {
            case KEY_ONE:
                current_component = C_WIRE;
                break;
            case KEY_TWO:
                current_component = C_RESISTOR;
                break;
            case KEY_SPACE:
                printf("Converting to graph\n");
                printf("%d\n", point_c);
                break;
            default:
                break;
        }

        // Update state
        switch (state) {
            case S_IDLE:
                break;
            case S_TRACING:
                if (snap != -1) {
                    nodes[node_c].b = snap;
                } else {
                    nodes[node_c].b = point_c-1;
                    points[nodes[node_c].b] = GetMousePosition();
                }
                break;
        }

        // Draw
        BeginDrawing();
        ClearBackground((Color) {255, 255, 255, 255});
        DrawText("1: filo", 10, 10, 40, (Color) {0, 0, 0, 255});
        DrawText("2: resistenza", 10, 60, 40, (Color) {0, 0, 0, 255});

        switch (current_component) {
            case C_WIRE:
                DrawText("1: filo", 10, 10, 40, (Color) {255, 0, 0, 255});
                break;
            case C_RESISTOR:
                DrawText("2: resistenza", 10, 60, 40, (Color) {255, 0, 0, 255});
                break;
        }

        for (int i = 0; i < (state == S_TRACING) + node_c; i++) {
            switch (nodes[i].type) {
                case C_WIRE:
                    draw_wire(points[nodes[i].a], points[nodes[i].b]);
                    break;
                case C_RESISTOR:
                    draw_resistor(points[nodes[i].a], points[nodes[i].b]);
                    break;
            }
        }

        if (snap != -1) {
            DrawCircleV(points[snap], 10, (Color) {0,150, 0, 255});
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
