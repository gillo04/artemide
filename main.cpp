#include <iostream>
#include <vector>
#include <stdio.h>

#include "raylib.h"
#include "raymath.h"

#include "structures.h"
#include "draw.h"

#define SNAP_DIST 30
#define MAX_NODES 50

using namespace std;

int get_snap(vector<Vector2> &points, int ignore) {
    if (points.size() - ignore <= 0) {
        return -1;
    }

    Vector2 mouse = GetMousePosition();
    int min_dist = Vector2Length(Vector2Subtract(mouse, points[0]));
    int min_i = 0;
    for (int i = 1; i < points.size() - ignore; i++) {
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

/*int find_node(int a, int b, Node nodes[], int node_c) {
    for (int i = 0; i < node_c; i++) {
        if (nodes[i].a == a && nodes[i].b == b) {
            return i;
        }
        if (nodes[i].a == b && nodes[i].b == a) {
            return i;
        }
    }
}*/

vector<Node*> get_close_a(int n, vector<Node> &nodes) {
    vector<Node*> close;
    int close_c = 0;
    int pt = nodes[n].a;
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i].a == -1 || i == n) {
            continue;
        }

        if (nodes[i].a == pt || nodes[i].b == pt) {
            close.push_back(&nodes[i]);
        }
    }
    return close;
}

vector<Node*> get_close_b(int n, vector<Node> &nodes) {
    vector<Node*> close;
    int close_c = 0;
    int pt = nodes[n].b;
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i].a == -1 || i == n) {
            continue;
        }

        if (nodes[i].a == pt || nodes[i].b == pt) {
            close.push_back(&nodes[i]);
        }
    }
    return close;
}

void simplify_resistor_series(vector<Vector2> &points,
                              vector<Node> &nodes, 
                              char graph[MAX_NODES*2][MAX_NODES*2]) {
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i].a == -1 || nodes[i].type != C_RESISTOR) {
            continue;
        }
        // Case a
        {
            vector<Node*> close = get_close_a(i, nodes);

            int current_pt = nodes[i].a;
            if (close.size() == 1) {
                if (close[0]->type == C_RESISTOR) {
                    int last_conn = (close[0]->a == current_pt)
                                    ? close[0]->b
                                    : close[0]->a;

                    graph[current_pt][last_conn] = 0;
                    graph[last_conn][current_pt] = 0;

                    graph[close[0]->a][close[0]->b] = 0;
                    graph[close[0]->b][close[0]->a] = 0;

                    nodes[i].a = last_conn;
                    close[0]->a = -1;
                    close[0]->b = -1;

                    nodes[i].value += close[0]->value;
                }
            }
        }

        // Case b
        {
            vector<Node*> close = get_close_b(i, nodes);

            int current_pt = nodes[i].b;
            if (close.size() == 1) {
                if (close[0]->type == C_RESISTOR) {
                    int last_conn = (close[0]->a == current_pt)
                                    ? close[0]->b
                                    : close[0]->a;

                    graph[current_pt][last_conn] = 0;
                    graph[last_conn][current_pt] = 0;

                    graph[close[0]->a][close[0]->b] = 0;
                    graph[close[0]->b][close[0]->a] = 0;

                    nodes[i].b = last_conn;
                    close[0]->a = -1;
                    close[0]->b = -1;

                    nodes[i].value += close[0]->value;
                }
            }
        }
    }
}

int main() {
    vector<Vector2> points;
    vector<Node> nodes;

    char graph[MAX_NODES*2][MAX_NODES*2];

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Artemide");

    // State
    int state = S_IDLE;
    int current_component = C_WIRE;

    int selected = -1;
    while (!WindowShouldClose()) {
        int snap = get_snap(points, (state == S_TRACING));

        // Handle events
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            state = S_TRACING;

            if (current_component != C_SELECT) {
                if (snap != -1) {
                    points.push_back(GetMousePosition());
                    nodes.push_back({
                        .a = snap,
                        .b = points.size()-1
                    });
                    // points[point_c] = GetMousePosition();       // b
                    /*nodes[node_c].a = snap;
                    nodes[node_c].b = point_c;
                    point_c += 1;*/
                } else {
                    points.push_back(GetMousePosition());
                    points.push_back(GetMousePosition());
                    nodes.push_back({
                        .a = points.size()-2,
                        .b = points.size()-1
                    });
                    /*points[point_c] = GetMousePosition();       // a
                    points[point_c+1] = GetMousePosition();     // b
                    nodes[node_c].a = point_c;
                    nodes[node_c].b = point_c+1;
                    point_c += 2;*/
                }
                // Convert these lines to work with the nodes vector
                // nodes[node_c].type = current_component;
                // nodes[node_c].value = node_c * 5;

                nodes.back().type = current_component;
                nodes.back().value = nodes.size() * 5;
            } else {
                selected = snap;
            }
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            state = S_IDLE;

            if (current_component != C_SELECT) {
                if (snap != -1) {
                    // point_c -= 1;
                    points.pop_back();
                }
                // node_c++;
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

                for (int i = 0; i < points.size(); i++) {
                    for (int j = 0; j < points.size(); j++) {
                        graph[i][j] = 0;
                    }
                }

                for (Node node : nodes) {
                    graph[node.b][node.a] = 1;
                    graph[node.a][node.b] = 1;
                }

                print_graph(graph, points.size());
                simplify_resistor_series(points, nodes, graph);
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
                        nodes.back().b = snap;
                    } else {
                        nodes.back().b = points.size()-1;
                        points[nodes.back().b] = GetMousePosition();
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

        for (int i = 0; i < /*(state == S_TRACING) +*/ nodes.size(); i++) {
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
