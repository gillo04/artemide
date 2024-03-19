#include <vector>
#include <string>
#include <tuple>
#include "raylib.h"

using namespace std;

enum {
    C_SELECT = 0,
    C_WIRE,
    C_RESISTOR,
    C_TERMINAL,
    C_TENSION_GEN,
    C_CURRENT_GEN
};

class Point {
public:
  Vector2 pos;
  bool visited;
  int node;

  Point(Vector2 p);
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
  // Contains adjacency list of the graphical circuit (pts and comps)
  vector<vector<tuple<int, int>>> adj_list;
  // Contains a list of components that are attached to a graph node
  vector<vector<int>> nodes;
  // Contains only non-wire components. a and b refer to nodes, not points
  vector<Component> arches;

public:
  // Constructor
  Circuit();
  // Load circuit from file at path
  Circuit(string path);
  // Prints circuit data (in the future it will be stored in a file)
  void print_circuit();
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
  // Populates adjacency list for graphical circuit
  void build_adjacency_list();
  // Returns a list of wire connected components to point p
  vector<int> get_connected(int p);
  // Populates nodes and arches
  void build_abstract_circuit();
  // Marks arch for deletion
  void mark_arch_for_deletion(int i);
  // Converts arch to wire, correcting nodes
  void convert_to_wire(int c);

  // Simplifies series
  bool s_series();
  // Simplifies parallel
  bool s_parallel();
  // Deletes compoentless wires
  bool s_useless_wires();
  // Deletes invalid components
  bool s_remove_dead_components();

  bool resistor_series(int first, int second);
  bool resistor_parallel(int first, int second);
  bool tension_gen_series(int first, int second);
};
