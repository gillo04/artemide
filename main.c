#include <stdio.h>

#include "raylib.h"
#include "raymath.h"

#include "structures.h"
#include "draw.h"

#define SNAP_DIST 30
#define MAX_NODES 50

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

void print_graph(char graph[MAX_NODES*2][MAX_NODES*2], int point_c) {
    for (int i = 0; i < point_c; i++) {
        for (int j = 0; j < point_c; j++) {
            printf("%d ", graph[i][j]);
        }
        printf("\n");
    }
}

int find_node(int a, int b, Node nodes[], int node_c) {
    for (int i = 0; i < node_c; i++) {
        if (nodes[i].a == a && nodes[i].b == b) {
            return i;
        }
        if (nodes[i].a == b && nodes[i].b == a) {
            return i;
        }
    }
}

void simplify_circuit(Vector2 points[], int point_c,
                      Node nodes[], int node_c, 
                      char graph[MAX_NODES*2][MAX_NODES*2]) {
    for (int i = 0; i < node_c; i++) {
        if (nodes[i].a == -1) {
            continue;
        }
        // Case a
        {
            int conn_c = 0, last_conn = -1;
            for (int j = 0; j < point_c; j++) {
                conn_c += graph[nodes[i].a][j];
                if (graph[nodes[i].a][j] && j != nodes[i].b) {
                    last_conn = j;
                }
            }

            if (conn_c == 2) {
                int adj = find_node(nodes[i].a, last_conn, nodes, node_c);
                graph[nodes[i].a][last_conn] = 0;
                graph[last_conn][nodes[i].a] = 0;

                graph[nodes[adj].a][nodes[adj].b] = 0;
                graph[nodes[adj].b][nodes[adj].a] = 0;

                nodes[i].a = last_conn;
                nodes[adj].a = -1;
                nodes[adj].b = -1;
            }
        }

        // Case b
        {
            int conn_c = 0, last_conn = -1;
            for (int j = 0; j < point_c; j++) {
                conn_c += graph[nodes[i].b][j];
                if (graph[nodes[i].b][j] && j != nodes[i].a) {
                    last_conn = j;
                }
            }

            if (conn_c == 2) {
                int adj = find_node(nodes[i].b, last_conn, nodes, node_c);
                graph[nodes[i].b][last_conn] = 0;
                graph[last_conn][nodes[i].b] = 0;

                graph[nodes[adj].a][nodes[adj].b] = 0;
                graph[nodes[adj].b][nodes[adj].a] = 0;

                nodes[i].b = last_conn;
                nodes[adj].a = -1;
                nodes[adj].b = -1;


            }
        }
    }
}

int main() {
    Vector2 points[MAX_NODES*2];
    int point_c = 0;

    Node nodes[MAX_NODES];
    int node_c = 0;

    char graph[MAX_NODES*2][MAX_NODES*2];

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Artemide");

    // State
    int state = S_IDLE;
    int current_component = C_WIRE;

    int selected = -1;
    while (!WindowShouldClose()) {
        int snap = get_snap(points, point_c - (state == S_TRACING));

        // Handle events
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            state = S_TRACING;

            if (current_component != C_SELECT) {
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
                nodes[node_c].value = node_c * 5;
            } else {
                selected = snap;
            }
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            state = S_IDLE;

            if (current_component != C_SELECT) {
                if (snap != -1) {
                    point_c -= 1;
                }
                node_c++;
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
                printf("Converting to graph\n");
                printf("%d\n", point_c);

                for (int i = 0; i < point_c; i++) {
                    for (int j = 0; j < point_c; j++) {
                        graph[i][j] = 0;
                    }
                }

                for (int i = 0; i < node_c; i++) {
                    graph[nodes[i].b][nodes[i].a] = 1;
                    graph[nodes[i].a][nodes[i].b] = 1;
                }

                print_graph(graph, point_c);
                simplify_circuit(points, point_c, nodes, node_c, graph);
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
                        nodes[node_c].b = snap;
                    } else {
                        nodes[node_c].b = point_c-1;
                        points[nodes[node_c].b] = GetMousePosition();
                    }
                } else {
                    points[selected] = GetMousePosition();
                }
                break;
        }

        // Draw
        BeginDrawing();
        ClearBackground((Color) {255, 255, 255, 255});

        draw_current_comp(current_component);

        for (int i = 0; i < (state == S_TRACING) + node_c; i++) {
            if (nodes[i].a != -1) {
                switch (nodes[i].type) {
                    case C_WIRE:
                        draw_wire(points[nodes[i].a], points[nodes[i].b]);
                        break;
                    case C_RESISTOR:
                        draw_resistor(points[nodes[i].a], points[nodes[i].b], nodes[i].value);
                        break;
                }
            }
        }

        if (snap != -1) {
            DrawCircleV(points[snap], 10, (Color) {0, 150, 0, 255});
        }

        if (current_component == C_SELECT) {
            DrawCircleV(points[selected], 10, (Color) {0, 0, 150, 255});
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
