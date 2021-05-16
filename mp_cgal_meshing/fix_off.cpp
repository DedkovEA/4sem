#include "Polyhedron.h"
#include <fstream>
#include <iostream>

const double DEFAULT_SPLIT_DISTANCE = 0.0005;

int main(int argc, char* argv[]) {
  const std::string fname = (argc > 1) ? argv[1] : "input.off";
  std::cout << "[INFO]: Start opening " << fname << '\n';
  try {
    std::ifstream f_in(fname);
    if(!f_in.is_open()) {
      std::cout << "[INFO]: Failed to open file " << fname << '\n';
        return -1;
    };

    std::cout << "[INFO]: Generating surface object from file " << fname <<'\n';
    Polyhedron surface(fname);
    std::cout << "[INFO]: Surface successfully generated. Start orienting.\n";
    surface.orient();
    std::cout << "[INFO]: Surface has been successfully oriented\n";

    double d = (argc > 2) ? std::atof(argv[2]) : DEFAULT_SPLIT_DISTANCE;
    if(d > 0) {
      std::cout << "[INFO]: Start splitting disconnected facet complexes via d = " <<
                    d << '\n';
      std::cout << "[NOTE]: May be a lot of [WARNING] messages about adding " << 
                   "already included triangles";
      if(surface.split_everything(d)) {
        std::cout << "[INFO]: All points with disconnected facet complexes have " << 
                    "been succesfully splitted via d = " << d << '\n';
      } else {
        std::cout << "[INFO]: There were no points with disconnected facet complexes\n";
      };
    };

    std::cout << "[INFO]: Start removing excessive verticies\n";
    if(surface.fix_useless_verticies()) {
      std::cout << "[INFO]: All excessive verticises have been successfully removed\n";
    } else {
      std::cout << "[INFO]: There were no excessive verticises\n";
    };
    std::cout << "[INFO]: Starting extra orientation\n";
    surface.orient();
    std::cout << "[INFO]: Extra orientation passed without troubles\n";

    const std::string ofname = "fixed_" + fname;
    std::ofstream f_out(ofname);
    std::cout << "[INFO]: Writing result geometry to file " << ofname << '\n';
    surface.write_in_file(f_out);
    std::cout << "[INFO]: Sucessfully written. Program finished with no errors\n";
  } catch(const char* e) {
    std::cout << "[ERROR]: Program failed with error: " << e;
  };

  return 0;
};

