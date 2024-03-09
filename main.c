#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 800
#define ARROW_THICKNESS 4.
#define RESISTOR_HEIGHT 24
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
    int id;
} Node;

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

int can_snap_to(Vector2 point) {
    return Vector2Length(Vector2Subtract(GetMousePosition(), point)) <= SNAP_DIST;
}

int main() {
    Vector2 points[MAX_NODES*2];
    int point_count = 0;

    Node nodes[MAX_NODES];
    int node_count = 0;

    int comp_count = 0;
    int wire_count = 0;

    char graph[MAX_NODES*2][MAX_NODES*2];
    for (int i = 0; i < MAX_NODES*2; i++) {
        for (int j = 0; j < MAX_NODES*2; j++) {
            graph[i][j] = 0;
        }
    }

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib [core] example - basic window");

    // State
    int state = S_IDLE;
    int current_component = C_RESISTOR;

    Vector2 cur_start, cur_end;
    while (!WindowShouldClose()) {
        // Handle events
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            state = S_TRACING;
            nodes[node_count].start = GetMousePosition();
            nodes[node_count].a = -1;
            for (int i = 0; i < node_count; i++) {
                if (can_snap_to(nodes[i].start)) {
                    // nodes[node_count].start = nodes[i].start;
                    nodes[node_count].a = i*2;
                    break;
                }
                if (can_snap_to(nodes[i].end)) {
                    // nodes[node_count].start = nodes[i].end;
                    nodes[node_count].a = i*2+1;
                    break;
                }
            }
            nodes[node_count].type = current_component;
            if (current_component == C_WIRE) {
                nodes[node_count].id = wire_count;
            } else {
                nodes[node_count].id = comp_count;
            }

            // Update graph
            if (nodes[node_count].a != -1) {
                graph[nodes[node_count].a][node_count*2] = 1;
                graph[node_count*2][nodes[node_count].a] = 1;

                for (int i = 0; i < node_count*2; i++) {
                    if (graph[i][nodes[node_count].a]) {
                        graph[i][node_count*2] = 1;
                        graph[node_count*2][i] = 1;
                    }
                }
            }

            node_count++;
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            state = S_IDLE;
            if (current_component == C_WIRE) {
                wire_count++;
            } else {
                comp_count++;
            }
            // Update graph
            if (nodes[node_count-1].b != -1) {
                graph[nodes[node_count-1].b][(node_count-1)*2+1] = 1;
                graph[(node_count-1)*2+1][nodes[node_count-1].b] = 1;

                for (int i = 0; i < (node_count-1)*2; i++) {
                    if (graph[i][nodes[node_count-1].b]) {
                        graph[i][(node_count-1)*2+1] = 1;
                        graph[(node_count-1)*2+1][i] = 1;
                    }
                }
            }
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
                // nodes_to_graph(nodes, graph);
                for (int i = 0; i < node_count*2; i++) {
                    for (int j = 0; j < node_count*2; j++) {
                        printf("%d ", graph[i][j]);
                    }
                    printf("\n");
                }

                for (int i = 0; i < node_count; i++) {
                    printf("%d\t%d\n", nodes[i].a, nodes[i].b);
                }
                break;
            default:
                break;
        }

        // Update state
        switch (state) {
            case S_IDLE:
                break;
            case S_TRACING:
                nodes[node_count-1].end = GetMousePosition();
                nodes[node_count-1].b = -1;
                for (int i = 0; i < node_count-1; i++) {
                    if (can_snap_to(nodes[i].start)) {
                        // nodes[node_count-1].end = nodes[i].start;
                        nodes[node_count-1].b = i*2;
                        break;
                    }
                    if (can_snap_to(nodes[i].end)) {
                        // nodes[node_count-1].end = nodes[i].end;
                        nodes[node_count-1].b = i*2+1;
                        break;
                    }
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

        for (int i = 0; i < node_count; i++) {
            switch (nodes[i].type) {
                case C_WIRE:
                    draw_wire(nodes[i].start, nodes[i].end);
                    break;
                case C_RESISTOR:
                    draw_resistor(nodes[i].start, nodes[i].end);
                    break;
            }

            if (can_snap_to(nodes[i].start)) {
                DrawCircleV(nodes[i].start, 7, (Color) {0, 150, 0, 255});
            }
            if (can_snap_to(nodes[i].end)) {
                DrawCircleV(nodes[i].end, 7, (Color) {0, 150, 0, 255});
            }
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
