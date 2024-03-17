#include "circuit.h"

#include <iostream>
#include <algorithm>
#include <sstream>

#include "raymath.h"
#include "draw.h"

#define SNAP_DIST 30

Point::Point(Vector2 p) 
    : pos(p) {}

Component::Component(int t, int a_, int b_, float v)
    : type(t), a(a_), b(b_), value(v) {}

Circuit::Circuit() {
    pts.clear();
    comps.clear();
    nodes.clear();
    arches.clear();
}

Circuit::Circuit(string path) {
    string source = LoadFileText(path.c_str());
    stringstream stream(source);

    int pt_count, comp_count;
    stream >> pt_count >> comp_count;

    for (int i = 0; i < pt_count; i++) {
        float x, y;
        stream >> x >> y;
        add_point(Point(Vector2{x, y}));
    }

    for (int i = 0; i < comp_count; i++) {
        int type, a, b;
        float value;
        stream >> type >> a >> b >> value;
        add_component(type, pts[a].pos, pts[b].pos, value);
    }
}

void Circuit::print_circuit() {
    string out = "";

    out += to_string(pts.size()) + " " + to_string(comps.size()) + "\n";
    for (int i = 0; i < pts.size(); i++) {
        out += to_string(pts[i].pos.x) + " " + to_string(pts[i].pos.y) + "\n";
    }

    for (int i = 0; i < comps.size(); i++) {
        out += to_string(comps[i].type) + " " + to_string(comps[i].a) + " " + to_string(comps[i].b) + " " + to_string(comps[i].value) + "\n";
    }

    cout << out << endl;
}

void Circuit::add_point(Point pt) {
    pts.push_back(pt);
}


void Circuit::add_component(int type_, Vector2 a_, Vector2 b_, float value_) {
    int snap_a = get_snap(a_, 0);
    if (snap_a == -1) {
        add_point(Point(a_));
        snap_a = pts.size() - 1;
    }

    int snap_b = get_snap(b_, 0);
    if (snap_b == -1) {
        add_point(Point(b_));
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
    // print_circuit();

    while (modified) {
        modified = false;
        build_adjacency_list();
        build_abstract_circuit();
        modified |= s_series();
        modified |= s_parallel();
        modified |= s_useless_wires();
        s_remove_dead_components();
    }
    return modified;
}

void Circuit::build_adjacency_list() {
    adj_list.clear();
    adj_list.resize(pts.size());
    for (int i = 0; i < comps.size(); i++) {
        if (comps[i].a == -1) {
            continue;
        }
        adj_list[comps[i].a].push_back({comps[i].b, i});
        adj_list[comps[i].b].push_back({comps[i].a, i});
    }
}

vector<int> Circuit::get_connected(int p) {
    vector<int> connected = {};
    if (pts[p].visited) {
        return connected;
    }

    pts[p].visited = true;
    pts[p].node = nodes.size();
    for (int i = 0; i < adj_list[p].size(); i++) {
        if (comps[get<1>(adj_list[p][i])].type == C_WIRE) {
            vector<int> tmp = get_connected(get<0>(adj_list[p][i]));
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

    for (int i = 0; i < comps.size(); i++) {
        if (comps[i].type == C_WIRE) {
            continue;
        }

        if (pts[comps[i].a].visited) {
            arches[i].a = pts[comps[i].a].node;
        } else {
            vector<int> connected = get_connected(comps[i].a);
            nodes.push_back(connected);
            arches[i].a = nodes.size() - 1;
        }

        if (pts[comps[i].b].visited) {
            arches[i].b = pts[comps[i].b].node;
        } else {
            vector<int> connected = get_connected(comps[i].b);
            nodes.push_back(connected);
            arches[i].b = nodes.size() - 1;
        }

        arches[i].type = comps[i].type;
        arches[i].value = comps[i].value;
    }

    /*cout << "pts: " << pts.size() << endl;
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
    }*/
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
    for (int i = 0; i < adj_list.size(); i++) {
        if (adj_list[i].size() == 1 && comps[get<1>(adj_list[i][0])].type == C_WIRE) {
            comps[get<1>(adj_list[i][0])].a = -1;
            comps[get<1>(adj_list[i][0])].b = -1;
            changed = true;
        }
    }
    return changed;
}

void Circuit::s_remove_dead_components() {
    // Erase components
    for (int i = 0; i < comps.size(); i++) {
        if (comps[i].a == -1) {
            comps.erase(comps.begin() + i);
            i--;
        }
    }

    // Erase points
    /*for (int i = 0; i < adj_list.size(); i++) {
        if (adj_list[i].size() == 0) {
            pts.erase(pts.begin() + i);
        }
    }*/
}
