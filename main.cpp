#include <iostream>
#include <vector>
#include <tuple>
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

typedef vector<vector<tuple<int, int>>> AdjList;
AdjList adjacency_list(vector<Vector2> &points, vector<Node> &nodes) {
    AdjList adj_list(points.size());
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i].a == -1) {
            continue;
        }
        adj_list[nodes[i].a].push_back({nodes[i].b, i});
        adj_list[nodes[i].b].push_back({nodes[i].a, i});
    }
    return adj_list;
}

vector<int> get_connected(int p, AdjList &adj_list, vector<Node> &nodes, vector<bool> &visited) {
    vector<int> connected(0);
    if (visited[p]) {
        return connected;
    }
    visited[p] = true;

    for (int i = 0; i < adj_list[p].size(); i++) {
        if (nodes[get<1>(adj_list[p][i])].type == C_WIRE) {
            vector<int> tmp = get_connected(get<0>(adj_list[p][i]), adj_list, nodes, visited);
            connected.insert(connected.end(), tmp.begin(), tmp.end());
        } else {
            connected.push_back(get<1>(adj_list[p][i]));
        }
    }
}

void s_resistor_series(vector<Vector2> &points, vector<Node> &nodes) {
    AdjList adj_list = adjacency_list(points, nodes);
    for (int i = 0; i < points.size(); i++) {
        vector<bool> visited(points.size(), false);
        vector<int> conn = get_connected(i, adj_list, nodes, visited);

        for (int j = 0; j < conn.size(); j++) {
            cout << conn[j] << " ";
        }
        cout << endl;

        if (conn.size() == 2) {
            if (nodes[conn[0]].type == C_RESISTOR &&
                nodes[conn[1]].type == C_RESISTOR) { // TODO: soon to remove

                nodes[conn[0]].value += nodes[conn[1]].value;
                nodes[conn[1]].value = -1;
                nodes[conn[1]].type = C_WIRE;
            }
        }

    }
}

int main() {
    vector<Vector2> points;
    vector<Node> nodes;

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
                } else {
                    points.push_back(GetMousePosition());
                    points.push_back(GetMousePosition());
                    nodes.push_back({
                        .a = points.size()-2,
                        .b = points.size()-1
                    });
                }

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
                    points.pop_back();
                }
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
                s_resistor_series(points, nodes);
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
