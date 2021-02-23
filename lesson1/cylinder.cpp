#include <set>
#include <gmsh.h>

int main(int argc, char **argv)
{
  gmsh::initialize();

  gmsh::model::add("cylinder");

  double lc = 3e-2;

  gmsh::model::geo::addPoint(0, 0, 0, lc, 5);
  gmsh::model::geo::addPoint(0.1, 0, 0, lc, 1);
  gmsh::model::geo::addPoint(0, 0.1, 0, lc, 2);
  gmsh::model::geo::addPoint(-0.1, 0, 0, lc, 3);
  gmsh::model::geo::addPoint(0, -0.1, 0, lc, 4);

  gmsh::model::geo::addPoint(0, 0, 0.4, lc, 10);
  gmsh::model::geo::addPoint(0.1, 0, 0.4, lc, 6);
  gmsh::model::geo::addPoint(0, 0.1, 0.4, lc, 7);
  gmsh::model::geo::addPoint(-0.1, 0, 0.4, lc, 8);
  gmsh::model::geo::addPoint(0, -0.1, 0.4, lc, 9);

  int a1 = gmsh::model::geo::addCircleArc(1, 5, 2);
  int a2 = gmsh::model::geo::addCircleArc(2, 5, 3);
  int a3 = gmsh::model::geo::addCircleArc(3, 5, 4);
  int a4 = gmsh::model::geo::addCircleArc(4, 5, 1);
  int b1 = gmsh::model::geo::addCircleArc(6, 10, 7);
  int b2 = gmsh::model::geo::addCircleArc(7, 10, 8);
  int b3 = gmsh::model::geo::addCircleArc(8, 10, 9);
  int b4 = gmsh::model::geo::addCircleArc(9, 10, 6);

  int v1 = gmsh::model::geo::addLine(1, 6);
  int v2 = gmsh::model::geo::addLine(2, 7);
  int v3 = gmsh::model::geo::addLine(3, 8);
  int v4 = gmsh::model::geo::addLine(4, 9);

  int cl1 = gmsh::model::geo::addCurveLoop({a1, v2, -b1, -v1});
  int cl2 = gmsh::model::geo::addCurveLoop({a2, v3, -b2, -v2});
  int cl3 = gmsh::model::geo::addCurveLoop({a3, v4, -b3, -v3});
  int cl4 = gmsh::model::geo::addCurveLoop({a4, v1, -b4, -v4});

  int bot = gmsh::model::geo::addCurveLoop({a1, a2, a3, a4});
  int top = gmsh::model::geo::addCurveLoop({b1, b2, b3, b4});

  gmsh::model::geo::addSurfaceFilling({cl1}, 1);
  gmsh::model::geo::addSurfaceFilling({cl2}, 2);
  gmsh::model::geo::addSurfaceFilling({cl3}, 3);
  gmsh::model::geo::addSurfaceFilling({cl4}, 4);

  gmsh::model::geo::addPlaneSurface({bot}, 5);
  gmsh::model::geo::addPlaneSurface({top}, 6);


  gmsh::model::geo::addSurfaceLoop({1, 2, 3, 4, 5, 6}, 1);

  gmsh::model::geo::addVolume({1});

  gmsh::model::geo::synchronize();

  gmsh::model::mesh::generate(3);

  gmsh::write("t2.msh");

  std::set<std::string> args(argv, argv + argc);
  if(!args.count("-nopopup")) gmsh::fltk::run();

  gmsh::finalize();

  return 0;
}
