// ---------------------------------------------------------------------
//
// Copyright (C) 2016 - 2018 by the deal.II authors
//
// This file is part of the deal.II library.
//
// The deal.II library is free software; you can use it, redistribute
// it, and/or modify it under the terms of the GNU Lesser General
// Public License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// The full text of the license can be found in the file LICENSE.md at
// the top level directory of deal.II.
//
// ---------------------------------------------------------------------


// Like the data_out_faces_postprocess_boundary_id_02 test, but also
// output information about internal faces (with -1 to indicate that
// no boundary indicator is available).


#include <deal.II/base/function.h>

#include <deal.II/dofs/dof_accessor.h>
#include <deal.II/dofs/dof_handler.h>

#include <deal.II/fe/fe_nothing.h>

#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/tria.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>

#include <deal.II/lac/vector.h>

#include <deal.II/numerics/data_out_faces.h>
#include <deal.II/numerics/data_postprocessor.h>
#include <deal.II/numerics/matrix_tools.h>
#include <deal.II/numerics/vector_tools.h>

#include "../tests.h"



std::ofstream logfile("output");


template <int dim>
void
test()
{
  Triangulation<dim> triangulation;
  FE_Nothing<dim>    fe;
  DoFHandler<dim>    dof_handler(triangulation);

  GridGenerator::hyper_cube(triangulation, 0, 1, true);
  triangulation.refine_global(1);
  triangulation.begin_active()->set_refine_flag();
  triangulation.execute_coarsening_and_refinement();

  dof_handler.distribute_dofs(fe);

  // Create a dummy vector. We will ignore its contents.
  Vector<double> solution(dof_handler.n_dofs());

  DataPostprocessors::BoundaryIds<dim> p;
  DataOutFaces<dim>                    data_out(false);
  data_out.attach_dof_handler(dof_handler);
  data_out.add_data_vector(solution, p);
  data_out.build_patches();
  data_out.write_gnuplot(logfile);
}


int
main()
{
  logfile << std::setprecision(2);
  deallog << std::setprecision(2);

  test<2>();
  test<2>();

  return 0;
}
