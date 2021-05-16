#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>
#include <cmath>

#include "Polyhedron.h"


// Point methods
id_type Point::get_id() const { return id; }

bool operator==(const Point& l, const Point& r) {
  return l.id==r.id;
};

std::ostream& operator<<(std::ostream& out, const Point& pt) {
  out << pt.id;
  return out;
};

void Point::write_in_file(std::ostream& out) const {
  out << std::setprecision(16);
  out << r[0] << ' ' << r[1] << ' ' << r[2] << '\n';
};


// Edge methods
Edge::Edge(Point_ptr p1, Point_ptr p2, const Polyhedron* const base) : base(base) {
  if(p1 <= p2) {
    pts.first = p1;
    pts.second = p2;
  } else {
    pts.first = p2;
    pts.second = p1;
  };
  if(p1 == p2) {
    std::cout << "[WARNING]: Degenerate edge with point " << base->points[p1] << '\n';
  };
};

void Edge::add_associated_triangle(Triangle_ptr assoc_tr) {
  if(std::find(assoc_trs.begin(), assoc_trs.end(), assoc_tr) != assoc_trs.end()) {
    std::cout << "[WARNING]: Already included triangle added to Edge " <<
      *this << " associated triangles list\n";
  } else {
    assoc_trs.push_back(assoc_tr);
  };
};

Triangle_ptr Edge::get_rest_triangle(const Triangle_ptr tr) {
  if(assoc_trs[0] == tr) {
    if(assoc_trs.size() == 1) {
      std::cout << "[WARNING]: Only one triangle is associated with the edge " << *this << '\n';
      return -1;
    };
    return assoc_trs[1];
  } else {
    return assoc_trs[0];
  }
};

Point_ptr get_common_point(const Edge& e1, const Edge& e2) {
  if(e1.pts.first == e2.pts.first) { return e1.pts.first; };
  if(e1.pts.first == e2.pts.second) { return e1.pts.first; };
  if(e1.pts.second == e2.pts.first) { return e1.pts.second; };
  if(e1.pts.second == e2.pts.second) { return e1.pts.second; };
  return -1;
};


// Triangle methods
Point_ptr Triangle::get_point(int n) const {
  Point_ptr res = get_common_point(base->edges[edges[n]], base->edges[edges[(n+1)%3]]);
  if(res == -1) {
    std::cout << "[ERROR]: Wrong triangle with edges ["  << 
      base->edges[edges[0]] << "," << base->edges[edges[1]] << ","<< 
      base->edges[edges[2]] << "]\n";
    throw "Wrong triangle";
  } else {
    return res;
  };    
};

const Triangle& Triangle::operator=(const Triangle& in_tr) {
    edges = in_tr.edges;
    processed = in_tr.processed;
    return *this;
  };

std::ostream& operator<<(std::ostream& out, const Triangle& tr) {
  out << tr.get_point(0) << " " << tr.get_point(1) << " " << tr.get_point(2);
  return out;
};

bool Triangle::flip() {
  if(processed) {
    return false;
  } else {
    Edge_ptr tempe = edges[1];
    edges[1] = edges[2];
    edges[2] = tempe;
    processed = true;
    return true;
  };
};

bool Triangle::is_oriented_according(const Triangle& sample) const {
  int this_edge, sample_edge;
  for(int i = 0; i < 3; ++i) {
    for(int j = 0; j < 3; ++j) {
      if(this->edges[i] == sample.edges[j]) {
        if(this->get_point(i) != sample.get_point(j)) {
          return true;
        } else {
          return false;
        }
      };
    };
  };
  std::cout << "[ERROR]: Somehow the end of is_oriented_according method reached, " <<
               "all computations may be incorrect!\n";
  throw "End of is_oriented_according method reached";
  return false;
};

bool Triangle::has_point(Point_ptr p) const { 
  return (base->edges[edges[0]].has_point(p)) || 
          (base->edges[edges[1]].has_point(p));
};


//Polyhedron
Polyhedron::Polyhedron() = default;

void Polyhedron::add_triangle(const std::vector<Point_ptr>& pts, id_type place) {
  // some checks
  if(pts.size() != 3) {
    std::cout << "[ERROR]: trying adding not triangle via add_triangle function\n";
    throw "Not triangle";
  };
  // <Optional TODO> Add check on Points lying in points array

  std::vector<Edge_ptr> es;
  for(int i = 0; i < 3; ++i) {
    Edge tmp = Edge(pts[i], pts[(i+1)%3], this);
    bool was_in_edges = false;
    for(int j = 0; j < edges.size(); ++j) {
      if(edges[j] == tmp) {
        es.push_back(j);
        was_in_edges = true;
        break;
      };
    };
    if(!was_in_edges) {
      edges.push_back(tmp);
      es.push_back(edges.size()-1);
    };
    // es.push_back(const_cast<Edge*>(&(*edges.insert(Edge(pts[i], pts[(i+1)%3])).first)));
  };
  Triangle_ptr added;
  if(place == -1) {
    trs.push_back(Triangle(es, this));
    added = trs.size()-1;
  } else {
    trs[place] = Triangle(es, this);
    added = place;
  };
  for(int i = 0; i < 3; ++i) {
    edges[es[i]].add_associated_triangle(added);
  };
};

void Polyhedron::add_triangle(Point_ptr p1, Point_ptr p2, Point_ptr p3) {
  std::vector<Point_ptr> pts = {p1, p2, p3};
  add_triangle(pts);
};

// generating from .off file
Polyhedron::Polyhedron(std::string fname) {
  std::ifstream f_in(fname);
  f_in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

  int node_count, faces_count, edge_count;
  f_in >> node_count >> faces_count >> edge_count;
  f_in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

  points.reserve(node_count);
  for(int i = 0; i < node_count; ++i) {
    double x, y, z;
    f_in >> x >> y >> z;
    points.push_back(Point(i, {x, y, z}));
  };

  trs.reserve(faces_count);
  for(int i = 0; i < faces_count; ++i) {
    int p1, p2, p3, dim;
    f_in >> dim >> p1 >> p2 >> p3;
    this->add_triangle(p1, p2, p3);
  };
};

// fixing
bool Polyhedron::orient_acording_triangle(Triangle_ptr init_tr) {
  bool res = true;
  for(Edge_ptr edge : trs[init_tr].edges) {
    std::vector<Triangle_ptr> as_trs = edges[edge].get_associated_triangles();
    if(as_trs.size() > 2) {
      std::cout << "[ERROR]: More than 2 triangles are associated with the edge " << 
        edges[edge] << ". Those are ";
      for(auto tr : as_trs) {
        std::cout << "{" << trs[tr] << "}";
      };
      std::cout << '\n';
      return false;
    };
    if(as_trs.size() == 2) {
      Triangle_ptr next;
      if(as_trs[0] == init_tr) {
        next = as_trs[1];
      } else {
        next = as_trs[0];
      };
      if(!trs[next].is_oriented_according(trs[init_tr])) {
        if(!trs[next].flip()) {
          std::cout << "[ERROR]: Triangle {" << trs[next] << 
            "} was already processed and its orientation not match triangle {" << 
            trs[init_tr] << "} orientation\n";
          return false;
        } else {
          res &= orient_acording_triangle(next);
        };
      } else {
        if(!trs[next].processed) {
          trs[next].processed = true;
          res &= orient_acording_triangle(next);
        };
      };
    };
  };
  return res;
};

bool Polyhedron::orient() {
  trs[0].processed = true;
  return orient_acording_triangle(0);
};

void Polyhedron::disorient() {
  std::for_each(trs.begin(), trs.end(), [](Triangle& tr){ tr.processed = false; });
};

void Polyhedron::export_triangles(std::ostream& out) {
  for(auto tr : trs) {
    out << "3  " << tr << '\n';
  };
};

void Polyhedron::write_in_file(std::ostream& out) {
  out << "OFF\n";
  out << points.size() << ' ' << trs.size() << ' ' << edges.size() << '\n';
  for(const Point& point : points) {
    point.write_in_file(out);
  };
  export_triangles(out);
};

std::vector<Triangle_ptr> Polyhedron::get_node_cycle(Triangle_ptr tr, Point_ptr p) {
  std::vector<Triangle_ptr> cycle;
  cycle.push_back(tr);
  Triangle_ptr next;
  Edge_ptr prev_edge;
  if(edges[trs[tr].edges[0]].has_point(p)) {
      prev_edge = trs[tr].edges[0];
      next = edges[prev_edge].get_rest_triangle(tr);
  } else {
      prev_edge = trs[tr].edges[1];
      next = edges[prev_edge].get_rest_triangle(tr);
  };
  
  while (next != tr) {
    for(Edge_ptr edi : trs[next].edges) {
      if(edges[edi].has_point(p) && (edi != prev_edge)) {
        cycle.push_back(next);
        next = edges[edi].get_rest_triangle(next);
        prev_edge = edi;
        break;
      };
    };
  };
  return cycle;
};

std::vector<std::vector<Triangle_ptr>> Polyhedron::node_analysis(Point_ptr p) {
  std::vector<uint8_t> analysed = std::vector<uint8_t>(trs.size(), 0);
  std::vector<std::vector<Triangle_ptr>> res;
  for(id_type i = 0; i < trs.size(); ++i) {
    if(trs[i].has_point(p) && (!analysed[i])) {
      std::vector<Triangle_ptr> node_cycle = this->get_node_cycle(i, p);
      for(Triangle_ptr tr : node_cycle) {
        analysed[tr] = 1;
      };
      res.push_back(node_cycle);
    };
  };
  return res;
};

bool Polyhedron::split_on_node(Point_ptr node, double d) {
  auto cycles = node_analysis(node);
  if(cycles.size() <= 1) {
    return false;
  };
  for(auto cycle : cycles) {
    std::vector<double> mean = {0.0, 0.0, 0.0};
    std::vector<Point_ptr> stable;
    for(auto tr : cycle) {
      for(int i = 0; i < 3; ++i) {
        Point_ptr tempp = trs[tr].get_point(i);
        if(tempp != node) {
          bool was_already = false;
          for(auto p : stable) {
            if(p == tempp) {
              was_already = true;
            };
          };
          if(!was_already) {
            stable.push_back(node);
          };
          for(int j = 0; j < 3; ++j) {
            mean[j] += points[tempp].r[j];
          };
        };
      };
    };
    double norm = 0;
    int scale = cycle.size() * 2;
    for(int j = 0; j < 3; ++j) {
      mean[j] /= scale;
      mean[j] -= points[node].r[j];
      norm += mean[j] * mean[j];
    };
    for(int j = 0; j < 3; ++j) {
      mean[j] = mean[j] / sqrt(norm) * d + points[node].r[j];
    };
    points.push_back(Point(points.size(), mean));
    Point_ptr new_p = points.size()-1;
    for(auto tr : cycle) {
      std::vector<Point_ptr> new_pts;
      for(int j = 0; j < 3; ++j) {
        Point_ptr cur_p = trs[tr].get_point(j);
        if(cur_p == node) {
          new_pts.push_back(new_p);
        } else {
          new_pts.push_back(cur_p);
        };
      };
      add_triangle(new_pts, tr);
    };
  };
  return true;
};

bool Polyhedron::split_everything(double d) {
  std::vector<Point_ptr> pts_to_spl;
  for(int i = 0; i < points.size(); ++i) {
    if(node_analysis(i).size() > 1) {
      pts_to_spl.push_back(i);
    };
  };
  if(pts_to_spl.size() == 0) {
    return false;
  };

  for(auto p : pts_to_spl) {
    split_on_node(p, d);
  };
  return true;
};

bool Polyhedron::fix_useless_verticies() {
  std::vector<uint8_t> useless = std::vector<uint8_t>(points.size(), 1);
  bool res = false;
  for(auto tr : trs) {
    for(int i = 0; i < 3; ++i) {
      useless[tr.get_point(i)] = 0;
    };
  };
  std::vector<Point> new_points; 
  std::vector<Point_ptr> new_index(points.size(), -1);
  for(int i = 0; i < useless.size(); ++i) {
    if(useless[i] == 0) {
      new_points.push_back(points[i]);
      new_index[i] = new_points.size() - 1;
    } else {
      res = true;
    };
  };
  std::vector<uint8_t> is_edge_processed(edges.size(), 0);
  for(int i = 0; i < trs.size(); ++i) {
    for(int j = 0; j < 3; ++j) {
      if(!is_edge_processed[trs[i].edges[j]]) {
        Point_ptr f, s;
        f = edges[trs[i].edges[j]].pts.first;
        s = edges[trs[i].edges[j]].pts.second;
        edges[trs[i].edges[j]].pts.first = new_index[f];
        edges[trs[i].edges[j]].pts.second = new_index[s];
        is_edge_processed[trs[i].edges[j]] = 1;
      };
    };
  };
  points = new_points;
  return res;
};