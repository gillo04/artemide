#include "circuit.h"

#include <iostream>
#include <algorithm>

#include "raymath.h"
#include "draw.h"

#define SNAP_DIST 30

Point::Point(Vector2 p, bool n) 
    : pos(p), visited(n) {}

Component::Component(int t, int a_, int b_, float v)
    : type(t), a(a_), b(b_), value(v) {}

Circuit::Circuit() {
    pts.clear();
    comps.clear();
    nodes.clear();
    arches.clear();
}

void Circuit::add_point(Point pt) {
    pts.push_back(pt);
}


void Circuit::add_component(int type_, Vector2 a_, Vector2 b_, float value_) {
    int snap_a = get_snap(a_, 0);
    if (snap_a == -1) {
        add_point(Point(a_, false));
        snap_a = pts.size() - 1;
    }

    int snap_b = get_snap(b_, 0);
    if (snap_b == -1) {
        add_point(Point(b_, false));
        snap_b = pts.size() - 1;
    }

    Component tmp(
        type_,
        snap_a,
        snap_b,
        value_
    );
    comps.push_back(tmp);
}

void Circuit::draw() {
    for (int i = 0; i < comps.size(); i++) {
        if (comps[i].a != -1) {
            draw_component(pts[comps[i].a].pos, pts[comps[i].b].pos, comps[i].type, comps[i].value);
        }
    }
}

int Circuit::get_snap(Vector2 mouse, int ignore) {
    if (pts.size() - ignore <= 0) {
        return -1;
    }

    int min_dist = Vector2Length(Vector2Subtract(mouse, pts[0].pos));
    int min_i = 0;
    for (int i = 1; i < pts.size() - ignore; i++) {
        int tmp = Vector2Length(Vector2Subtract(mouse, pts[i].pos));
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

bool Circuit::simplify() {
    bool modified = true;

    while (modified) {
        modified = false;
        build_abstract_circuit();
        modified |= s_series();

        cout << endl << endl;
        build_abstract_circuit();
        modified |= s_parallel();

        // modified |= s_useless_wires();
        s_remove_dead_components();
    }
    return modified;
}

vector<vector<tuple<int, int>>> Circuit::adjacency_list() {
    vector<vector<tuple<int, int>>> adj_list(pts.size());
    for (int i = 0; i < comps.size(); i++) {
        if (comps[i].a == -1) {
            continue;
        }
        adj_list[comps[i].a].push_back({comps[i].b, i});
        adj_list[comps[i].b].push_back({comps[i].a, i});
    }
    return adj_list;
}

vector<int> Circuit::get_connected(int p, vector<vector<tuple<int, int>>> &adj_list) {
    vector<int> connected = {};
    if (pts[p].visited) {
        return connected;
    }

    pts[p].visited = true;
    pts[p].node = nodes.size();
    for (int i = 0; i < adj_list[p].size(); i++) {
        if (comps[get<1>(adj_list[p][i])].type == C_WIRE) {
            vector<int> tmp = get_connected(get<0>(adj_list[p][i]), adj_list);
            connected.insert(connected.end(), tmp.begin(), tmp.end());
        } else {
            connected.push_back(get<1>(adj_list[p][i]));
        }
    }
    return connected;
}

void Circuit::build_abstract_circuit() {
    nodes.clear();
    arches.clear();
    arches.resize(comps.size(), Component(-1, -1, -1, -1));

    for (int i = 0; i < pts.size(); i++) {
        pts[i].visited = false;
    }

    vector<vector<tuple<int, int>>> adj_list = adjacency_list();
    for (int i = 0; i < comps.size(); i++) {
        if (pts[comps[i].a].visited) {
            arches[i].a = pts[comps[i].a].node;
        } else {
            vector<int> connected = get_connected(comps[i].a, adj_list);
            nodes.push_back(connected);
            arches[i].a = nodes.size() - 1;
        }

        if (pts[comps[i].b].visited) {
            arches[i].b = pts[comps[i].b].node;
        } else {
            vector<int> connected = get_connected(comps[i].b, adj_list);
            nodes.push_back(connected);
            arches[i].b = nodes.size() - 1;
        }

        arches[i].type = comps[i].type;
        arches[i].value = comps[i].value;
    }

    cout << "pts: " << pts.size() << endl;
    // print arches 
    for (int i = 0; i < arches.size(); i++) {
        cout << "arches[" << i << "]: " << arches[i].a << " " << arches[i].b << endl;
    }
    // print nodes
    for (int i = 0; i < nodes.size(); i++) {
        cout << "nodes[" << i << "]: ";
        for (int j = 0; j < nodes[i].size(); j++) {
            cout << nodes[i][j] << " ";
        }
        cout << endl;
    }
}

void Circuit::mark_arch_for_deletion(int i) {
    remove(nodes[arches[i].a].begin(), nodes[arches[i].a].end(), i);
    remove(nodes[arches[i].b].begin(), nodes[arches[i].b].end(), i);

    arches[i].a = -1;
    arches[i].b = -1;

    comps[i].a = -1;
    comps[i].b = -1;
}

bool Circuit::s_series() {
    bool changed = false;
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i].size() == 2) {
            if (comps[nodes[i][0]].type == C_RESISTOR
                && comps[nodes[i][1]].type == C_RESISTOR) { 
                comps[nodes[i][0]].value += comps[nodes[i][1]].value;
                comps[nodes[i][1]].value = 0;
                comps[nodes[i][1]].type = C_WIRE;
                changed = true;
            }
        }
    }
    return changed;
}

bool Circuit::s_parallel() {
    bool changed = false;
    for (int i = 0; i < arches.size(); i++) {
        if (arches[i].a == -1 || arches[i].type != C_RESISTOR) {
            continue;
        }

        for (int j = 0; j < arches.size(); j++) {
            if (i == j || arches[j].a == -1 || arches[j].type != C_RESISTOR) {
                continue;
            }

            if ((arches[i].a == arches[j].a && arches[i].b == arches[j].b)
                || (arches[i].a == arches[j].b && arches[i].b == arches[j].a)) {
                comps[i].value = 1/(1/comps[i].value + 1/comps[j].value);
                mark_arch_for_deletion(j);
                changed = true;
            }
        }
    }
    return changed;
}

bool Circuit::s_useless_wires() {
    bool changed = false;
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i].size() == 1 && comps[nodes[i][0]].type == C_WIRE) {
            comps[nodes[i][0]].a = -1;
            comps[nodes[i][0]].b = -1;
            changed = true;
        }
    }
    return changed;
}

void Circuit::s_remove_dead_components() {
    for (int i = 0; i < comps.size(); i++) {
        if (comps[i].a == -1) {
            comps.erase(comps.begin() + i);
            i--;
        }
    }
}
