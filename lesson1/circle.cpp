#include <set>
#include <gmsh.h>

int main(int argc, char **argv)
{
  gmsh::initialize();

  gmsh::model::add("circle");

  double lc = 1e-2;

  gmsh::model::geo::addPoint(0, 0, 0, lc, 5);
  gmsh::model::geo::addPoint(0.1, 0, 0, lc, 1);
  gmsh::model::geo::addPoint(0, 0.1, 0, lc, 2);
  gmsh::model::geo::addPoint(-0.1, 0, 0, lc, 3);
  gmsh::model::geo::addPoint(0, -0.1, 0, lc, 4);

  int a1 = gmsh::model::geo::addCircleArc(1, 5, 2);
  int a2 = gmsh::model::geo::addCircleArc(2, 5, 3);
  int a3 = gmsh::model::geo::addCircleArc(3, 5, 4);
  int a4 = gmsh::model::geo::addCircleArc(4, 5, 1);

  gmsh::model::geo::addCurveLoop({a1, a2, a3, a4}, 1);
  gmsh::model::geo::addPlaneSurface({1}, 1);

  gmsh::model::geo::synchronize();

  gmsh::model::mesh::generate(2);

  gmsh::write("t2.msh");

  std::set<std::string> args(argv, argv + argc);
  if(!args.count("-nopopup")) gmsh::fltk::run();

  gmsh::finalize();

  return 0;
}
