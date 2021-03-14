#include <dolfin.h>
#include "poisson.h"

using namespace dolfin;

// Source term (right-hand side)
class Source : public Expression
{
  void eval(Array<double>& values, const Array<double>& x) const
  {
    double dx = x[0] - 0.5;
    double dy = x[1] - 0.5;
    if(dx*dx + dy*dy < 0.04) {
        values[0] = 10;
    } else {
        values[0] = 0;
    };
  };
};

// Normal derivative (Neumann boundary condition)
class dUdN : public Expression
{
  void eval(Array<double>& values, const Array<double>& x) const
  {
    values[0] = sin(5*x[0]);
  }
};

// Sub domain for Dirichlet boundary condition
class DirichletBoundary : public SubDomain
{
  bool inside(const Array<double>& x, bool on_boundary) const
  {
    return (x[0] < DOLFIN_EPS or x[0] > 1.0 - DOLFIN_EPS) or
           (x[1] < DOLFIN_EPS or x[1] > 1.0 - DOLFIN_EPS);
  }
};

class BoundaryExpr : public Expression
{
  void eval(Array<double>& values, const Array<double>& x) const
    {
      values[0] = x[1];
    };
};

int main()
{
  // Create mesh and function space
  auto mesh = std::make_shared<Mesh>(UnitSquareMesh::create({{32, 32}}, CellType::Type::triangle));
  auto V = std::make_shared<poisson::FunctionSpace>(mesh);

  // Define boundary condition
  auto u0 = std::make_shared<BoundaryExpr>();
  auto boundary = std::make_shared<DirichletBoundary>();
  DirichletBC bc(V, u0, boundary);

  // Define variational forms
  poisson::BilinearForm a(V, V);
  poisson::LinearForm L(V);
  auto f = std::make_shared<Source>();
  auto g = std::make_shared<dUdN>();
  L.f = f;
  L.g = g;

  // Compute solution
  Function u(V);
  solve(a == L, u, bc);

  // Save solution in VTK format
  File file("poisson_linear_bounds.pvd");
  file << u;
 
  // Second picture
  auto u02 = std::make_shared<Constant>(0.0);
  DirichletBC bc2(V, u02, boundary);

  // Compute solution
  solve(a == L, u, bc2);

  // Save solution in VTK format
  File file2("poisson_default_bounds.pvd");
  file2 << u;
 
  return 0;
}
