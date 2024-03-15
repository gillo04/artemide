#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>

#include "raylib.h"
#include "raymath.h"

#include "circuit.h"
#include "draw.h"

#define SNAP_DIST 30

using namespace std;

/*
int get_snap(vector<Vector2> &pts, int ignore) {
    if (pts.size() - ignore <= 0) {
        return -1;
    }

    Vector2 mouse = GetMousePosition();
    int min_dist = Vector2Length(Vector2Subtract(mouse, pts[0]));
    int min_i = 0;
    for (int i = 1; i < pts.size() - ignore; i++) {
        int tmp = Vector2Length(Vector2Subtract(mouse, pts[i]));
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
AdjList adjacency_list(vector<Vector2> &pts, vector<Node> &circ.comps) {
    AdjList adj_list(pts.size());
    for (int i = 0; i < circ.comps.size(); i++) {
        if (circ.comps[i].a == -1) {
            continue;
        }
        adj_list[circ.comps[i].a].push_back({nodes[i].b, i});
        adj_list[circ.comps[i].b].push_back({nodes[i].a, i});
    }
    return adj_list;
}

vector<int> get_connected(int p, AdjList &adj_list, vector<Node> &circ.comps, vector<bool> &visited) {
    vector<int> connected(0);
    if (visited[p]) {
        return connected;
    }
    visited[p] = true;

    for (int i = 0; i < adj_list[p].size(); i++) {
        if (circ.comps[get<1>(adj_list[p][i])].type == C_WIRE) {
            vector<int> tmp = get_connected(get<0>(adj_list[p][i]), adj_list, circ.comps, visited);
            connected.insert(connected.end(), tmp.begin(), tmp.end());
        } else {
            connected.push_back(get<1>(adj_list[p][i]));
        }
    }
}

void s_resistor_series(vector<Vector2> &pts, vector<Node> &circ.comps) {
    AdjList adj_list = adjacency_list(pts, circ.comps);
    for (int i = 0; i < pts.size(); i++) {
        vector<bool> visited(pts.size(), false);
        vector<int> conn = get_connected(i, adj_list, circ.comps, visited);

        if (conn.size() == 2) {
            if (circ.comps[conn[0]].type == C_RESISTOR &&
                circ.comps[conn[1]].type == C_RESISTOR) { // TODO: soon to remove

                circ.comps[conn[0]].value += nodes[conn[1]].value;
                circ.comps[conn[1]].value = -1;
                circ.comps[conn[1]].type = C_WIRE;
            }
        }

    }
}

void s_resistor_parallel(vector<Vector2> &pts, vector<Node> &circ.comps) {
    AdjList adj_list = adjacency_list(pts, circ.comps);

    for (int i = 0; i < circ.comps.size(); i++) {
        if (circ.comps[i].type == C_WIRE || nodes[i].a == -1) {
            continue;
        }

        vector<bool> visited(pts.size(), false);
        vector<int> conn_a = get_connected(circ.comps[i].a, adj_list, nodes, visited);

        fill(visited.begin(), visited.end(), false);
        vector<int> conn_b = get_connected(circ.comps[i].b, adj_list, nodes, visited);

        for (int j = 0; j < conn_a.size(); j++) {
            if (circ.comps[conn_a[j]].type != C_RESISTOR || conn_a[j] == i) {
                continue;
            }

            if (find(conn_b.begin(), conn_b.end(), conn_a[j]) != conn_b.end()) {
                circ.comps[i].value = 1/(1/nodes[i].value + 1/nodes[conn_a[j]].value);
                // circ.comps.erase(nodes.begin() + conn_a[j]);
                circ.comps[conn_a[j]].a = -1;
                circ.comps[conn_a[j]].b = -1;
            }
        }
    }
}

void s_useless_wires(vector<Vector2> &pts, vector<Node> &circ.comps) {
    AdjList adj_list = adjacency_list(pts, circ.comps);
    for (int i = 0; i < pts.size(); i++) {
        if (adj_list[i].size() == 1) {
            int j = get<1>(adj_list[i][0]);
            if (circ.comps[j].type == C_WIRE) {
                // circ.comps.erase(nodes.begin() + conn[0]);
                circ.comps[j].a = -1;
                circ.comps[j].b = -1;
                adj_list[i].clear();
            }
        }
    }
}

void s_remove_dead_circ.comps(vector<Vector2> &pts, vector<Node> &nodes) {
    for (int i = 0; i < circ.comps.size(); i++) {
        if (circ.comps[i].a == -1) {
            circ.comps.erase(nodes.begin() + i);
        }
    }
}*/

int main() {
    /*vector<Vector2> pts;
    vector<Node> circ.comps;*/

    Circuit circ;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Artemide");

    // State
    int state = S_IDLE;
    int current_component = C_WIRE;

    int selected = -1;
    Vector2 tmp_a, tmp_b;
    while (!WindowShouldClose()) {
        int snap = circ.get_snap(GetMousePosition(), state == S_TRACING);

        // Handle events
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            state = S_TRACING;

            if (current_component != C_SELECT) {
                if (snap != -1) {
                    circ.add_point(Point(GetMousePosition(), -1));
                    circ.add_component(Component(
                        current_component,
                        snap,
                        circ.pts.size()-1,
                        5
                    ));
                    /*pts.push_back(GetMousePosition());
                    circ.comps.push_back({
                        .a = snap,
                        .b = pts.size()-1
                    });*/
                } else {
                    circ.add_point(Point(GetMousePosition(), -1));
                    circ.add_point(Point(GetMousePosition(), -1));
                    circ.add_component(Component(
                        current_component,
                        circ.pts.size()-2,
                        circ.pts.size()-1,
                        5
                    ));
                    /*pts.push_back(GetMousePosition());
                    pts.push_back(GetMousePosition());
                    circ.comps.push_back({
                        .a = pts.size()-2,
                        .b = pts.size()-1
                    });*/
                }

                /*circ.comps.back().type = current_component;
                circ.comps.back().value = 5;*/
            } else {
                selected = snap;
            }
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            state = S_IDLE;

            if (current_component != C_SELECT) {
                if (snap != -1) {
                    circ.pts.pop_back();
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
                /*s_resistor_series(pts, circ.comps);
                s_resistor_parallel(pts, circ.comps);
                s_useless_wires(pts, circ.comps);
                s_remove_dead_circ.comps(pts, nodes);*/
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
                        circ.comps.back().b = snap;
                    } else {
                        circ.comps.back().b = circ.pts.size()-1;
                        circ.pts[circ.comps.back().b].pos = GetMousePosition();
                    }
                } else {
                    circ.pts[selected].pos = GetMousePosition();
                }
                break;
        }

        // Draw
        BeginDrawing();
        ClearBackground((Color) {255, 255, 255, 255});

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
