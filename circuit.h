#include <vector>
#include <tuple>
#include "raylib.h"

using namespace std;

enum {
    S_IDLE,
    S_TRACING
};

enum {
    C_SELECT,
    C_WIRE,
    C_RESISTOR,
    C_NODE
};

class Point {
public:
  Vector2 pos;
  bool visited;
  int node;

  Point(Vector2 p, bool n);
};

class Component {
public:
  int type;
  int a, b;
  float value;

  Component(int t, int a_, int b_, float v);
};

class Circuit {
public:
  // Contains graphical points
  vector<Point> pts;
  // Contains graphical components
  vector<Component> comps;

private:
  // Contains a list of components that are attached to a graph node
  vector<vector<int>> nodes;
  // Contains only non-wire components. a and b refer to nodes, not points
  vector<Component> arches;

public:
  Circuit();
  // Adds point. If .node = -1, the point is a new node 
  void add_point(Point pt);
  // Adds the component to the circuit
  void add_component(int type_, Vector2 a_, Vector2 b_, float value_);
  
  // Draws circuit
  void draw();
  // Returns index of snap point
  int get_snap(Vector2 mouse, int ignore);
  // Simplifies the circuit
  bool simplify();

private:
  // Returns adjacency list for graphical circuit
  vector<vector<tuple<int, int>>> adjacency_list();
  // Returns a list of wire connected components to point p
  vector<int> get_connected(int p, vector<vector<tuple<int, int>>> &adj_list);
  // Populates nodes and arches
  void build_abstract_circuit();
  // Marks arch for deletion
  void mark_arch_for_deletion(int i);

  // Simplifies series
  bool s_series();
  // Simplifies parallel
  bool s_parallel();
  // Deletes compoentless wires
  bool s_useless_wires();

  // Deletes invalid components
  void s_remove_dead_components();
};
