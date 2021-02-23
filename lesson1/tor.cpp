#include <set>
#include <gmsh.h>
#include <vector>

std::vector<int> makeCirclePoints(double x, double y, double z, double r, 
                                  double lc) {
    std::vector<int> points;
    points.push_back(gmsh::model::geo::addPoint(x+r, y, z, lc));
    points.push_back(gmsh::model::geo::addPoint(x, y+r, z, lc));
    points.push_back(gmsh::model::geo::addPoint(x-r, y, z, lc));
    points.push_back(gmsh::model::geo::addPoint(x, y-r, z, lc));
    
    return points;
};

int makeTorSurface(double R, double r, double lc) {
    int c = gmsh::model::geo::addPoint(0, 0, 0, lc);
    int b = gmsh::model::geo::addPoint(0, 0, -r, lc);
    int t = gmsh::model::geo::addPoint(0, 0, r, lc);

    auto c1 = makeCirclePoints(0, 0, 0, R+r, lc);
    auto c3 = makeCirclePoints(0, 0, 0, R-r, lc);
    auto c2 = makeCirclePoints(0, 0, r, R, lc);
    auto c4 = makeCirclePoints(0, 0, -r, R, lc);

    std::vector<int> ctrs = {
        gmsh::model::geo::addPoint(R, 0, 0, lc), 
        gmsh::model::geo::addPoint(0, R, 0, lc), 
        gmsh::model::geo::addPoint(-R, 0, 0, lc), 
        gmsh::model::geo::addPoint(0, -R, 0, lc), 
        };
    std::vector<int> arcs;
    arcs.resize(32);
    for(int i = 0; i < 4; ++i) {
        arcs[8*i + 0] = gmsh::model::geo::addCircleArc(c1[i], ctrs[i], c2[i]);
        arcs[8*i + 1] = gmsh::model::geo::addCircleArc(c2[i], ctrs[i], c3[i]);
        arcs[8*i + 2] = gmsh::model::geo::addCircleArc(c3[i], ctrs[i], c4[i]);
        arcs[8*i + 3] = gmsh::model::geo::addCircleArc(c4[i], ctrs[i], c1[i]);

        arcs[8*i + 4] = gmsh::model::geo::addCircleArc(c1[i], c, c1[(i+1)%4]);
        arcs[8*i + 5] = gmsh::model::geo::addCircleArc(c2[i], t, c2[(i+1)%4]);
        arcs[8*i + 6] = gmsh::model::geo::addCircleArc(c3[i], c, c3[(i+1)%4]);
        arcs[8*i + 7] = gmsh::model::geo::addCircleArc(c4[i], b, c4[(i+1)%4]);
    };
    
    std::vector<int> surfList;
    surfList.resize(16);
    for(int i = 0; i < 4; ++i){
        surfList[4*i + 0] = gmsh::model::geo::addSurfaceFilling({gmsh::model::geo::addCurveLoop({arcs[8*i+0], arcs[8*i+5], -arcs[8*((i+1)%4)+0], -arcs[8*i+4]})});
        surfList[4*i + 1] = gmsh::model::geo::addSurfaceFilling({gmsh::model::geo::addCurveLoop({arcs[8*i+1], arcs[8*i+6], -arcs[8*((i+1)%4)+1], -arcs[8*i+5]})});
        surfList[4*i + 2] = gmsh::model::geo::addSurfaceFilling({gmsh::model::geo::addCurveLoop({arcs[8*i+2], arcs[8*i+7], -arcs[8*((i+1)%4)+2], -arcs[8*i+6]})});
        surfList[4*i + 3] = gmsh::model::geo::addSurfaceFilling({gmsh::model::geo::addCurveLoop({arcs[8*i+3], arcs[8*i+4], -arcs[8*((i+1)%4)+3], -arcs[8*i+7]})});
    };

    return gmsh::model::geo::addSurfaceLoop(surfList);
};

int main(int argc, char **argv)
{
  gmsh::initialize();

  gmsh::model::add("tor");

  double lc = 2e-2;

  double R = 0.5;
  double r1 = 0.09;
  double r2 = 0.18;

  auto outTor = makeTorSurface(R, r2, lc);
  auto inTor = makeTorSurface(R, r1, lc);

  gmsh::model::geo::addVolume({outTor, inTor});

  gmsh::model::geo::synchronize();

  gmsh::model::mesh::generate(3);

  gmsh::write("t2.msh");

  std::set<std::string> args(argv, argv + argc);
  if(!args.count("-nopopup")) gmsh::fltk::run();

  gmsh::finalize();

  return 0;
}
