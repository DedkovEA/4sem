#ifndef POLYHEDRON_H
#define POLYHEDRON_H


#include <vector>
#include <iostream>


using id_type = uint32_t;

using Point_ptr = id_type;
using Edge_ptr = id_type;
using Triangle_ptr = id_type;

class Point;
class Edge;
class Triangle;
class Polyhedron;


class Point {
 private:
  friend class Polyhedron;
  id_type id;
  std::vector<double> r;
 public:
  Point(id_type id=0) : id(id), r({0.0, 0.0, 0.0}) {};
  Point(id_type id, std::vector<double> r) : id(id), r(r) {};
  
  id_type get_id() const;

  friend bool operator==(const Point& l, const Point& r);

  friend std::ostream& operator<<(std::ostream& out, const Point& pt);
  void write_in_file(std::ostream& out) const;
};


class Edge {
 private:
  friend class Triangle;
  friend class Polyhedron;
  std::pair<Point_ptr, Point_ptr> pts;
  std::vector<Triangle_ptr> assoc_trs;
  const Polyhedron* const base; 
 public:
  Edge(Point_ptr p1, Point_ptr p2, const Polyhedron* const base);

  // standart operators
  friend bool operator==(const Edge& l, const Edge& r) {
    return (l.pts == r.pts) && (l.base == r.base); // NOTE: be careful, pointer comparison
  };

  friend std::ostream& operator<<(std::ostream& out, const Edge& edge) {
    out << "{" << edge.pts.first << "," << edge.pts.second << "}";
    return out;
  };

  // associated_triangles-related methods
  void add_associated_triangle(Triangle_ptr assoc_tr);

  const std::vector<Triangle_ptr>& get_associated_triangles() const {
    return assoc_trs;
  };

  Triangle_ptr get_rest_triangle(const Triangle_ptr tr);

  // point-related methods
  friend Point_ptr get_common_point(const Edge& e1, const Edge& e2);

  bool has_point(Point_ptr p) const {
    return (p == pts.first) || (p == pts.second);
  };
};


class Triangle {
 private:
  friend class Polyhedron;
  const Polyhedron* const base;
  std::vector<Edge_ptr> edges;
  bool processed;
  

  Triangle(const std::vector<Edge_ptr>& edges, const Polyhedron* const base) : 
        edges(edges), processed(false), base(base) {};

  // default methods
  const Triangle& operator=(const Triangle& in_tr);

  friend std::ostream& operator<<(std::ostream& out, const Triangle& tr);

  // point-related methods
  Point_ptr get_point(int n) const;

  bool has_point(Point_ptr p) const;

  // orientation-related methods
  bool is_oriented_according(const Triangle& sample) const;

  // changes orientation, returns false if already processed
  bool flip();
};


class Polyhedron {
 private:
  friend class Edge;
  friend class Triangle;

  std::vector<Point> points;
  std::vector<Edge> edges;
  std::vector<Triangle> trs;

 public:
  Polyhedron();

  void add_triangle(const std::vector<Point_ptr>& pts, id_type place=-1);
  void add_triangle(Point_ptr p1, Point_ptr p2, Point_ptr p3); 
  const Edge& get_edge(Edge_ptr ed) { return edges[ed]; };
  const Triangle& get_triangle(Triangle_ptr tr) { return trs[tr]; };

  // generating from .off file
  Polyhedron(std::string fname);

  // fixing
  bool orient_acording_triangle(Triangle_ptr init_tr);

  bool orient();
  void disorient();

  void export_triangles(std::ostream& out);
  void write_in_file(std::ostream& out);

  // insights
  std::vector<Triangle_ptr> get_node_cycle(Triangle_ptr tr, Point_ptr p);
  std::vector<std::vector<Triangle_ptr>> node_analysis(Point_ptr p);

  // deep fixes
  bool split_on_node(Point_ptr node, double d);

  bool split_everything(double d);

  bool fix_useless_verticies();
};

#endif //POLYHEDRON_H