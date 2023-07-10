// ---------------------------------------------------------------------
//
// Copyright (C) 2001 - 2022 by the deal.II authors
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

// This test checks functionality of compute_active_cell_layer_within_distance
// using a mesh generated by subdivided_hyper_rectangle. The motivation here is
// to check predicate skin using a slightly complex mesh than a refined
// hyper_cube, that could still be visually verified. The output is verified
// solely by visualizing the resulting active cells within skin distance to
// predicate subdomain using vtk files


// write VTK files for visual inspection
//#define WRITE_VTK


#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/grid_out.h>
#include <deal.II/grid/grid_tools.h>
#include <deal.II/grid/tria.h>

#include <vector>

#include "../tests.h"

template <int dim>
bool
pred_mat_id(const typename Triangulation<dim>::active_cell_iterator &cell)
{
  return cell->material_id() == 2;
}

template <int dim>
void
write_mat_id_to_file(const Triangulation<dim> &tria)
{
  int                                               count = 0;
  typename Triangulation<dim>::active_cell_iterator cell  = tria.begin_active(),
                                                    endc  = tria.end();
  for (; cell != endc; ++cell, ++count)
    {
      deallog << count << ' ' << static_cast<int>(cell->material_id())
              << std::endl;
    }
  deallog << std::endl;
}


template <int dim>
void
test()
{
  deallog << "dim = " << dim << std::endl;


  std::vector<double>
    step_sizes_i; // step sizes in i direction (similar in all directions)
  std::vector<std::vector<double>>
    step_sizes; // step sizes as input to the subdivided_hyper_rectangle


  unsigned int n_steps = 25;
  double       size    = 0.; // size of domain in i direction
  for (unsigned int j = 1; j < n_steps; ++j)
    {
      step_sizes_i.push_back((1. + double(j)) / double(n_steps));
      size += (1. + double(j)) / double(n_steps);
    }

  for (unsigned int d = 0; d < dim; ++d)
    step_sizes.push_back(step_sizes_i);

  const Point<dim> bottom_left;
  const Point<dim> upper_right = dim == 1 ? Point<dim>(size) :
                                 dim == 2 ? Point<dim>(size, size) :
                                            Point<dim>(size, size, size);

  Triangulation<dim> tria;
  GridGenerator::subdivided_hyper_rectangle(
    tria, step_sizes, bottom_left, upper_right, true);

  using cell_iterator = typename Triangulation<dim>::active_cell_iterator;

  // Mark a small block at the corner of the hypercube
  cell_iterator cell = tria.begin_active(), endc = tria.end();
  for (; cell != endc; ++cell)
    {
      bool mark = true;
      for (unsigned int d = 0; d < dim; ++d)
        mark &= cell->center()[d] > 5. && cell->center()[d] < 9.;

      if (mark == true)
        cell->set_material_id(2);
      else
        cell->set_material_id(1);
    }

  deallog << "Grid without skin:" << std::endl;
  write_mat_id_to_file(tria);
#ifdef WRITE_VTK
  // Write to file to visually check result
  {
    const std::string filename =
      "grid_no_skin_" + Utilities::int_to_string(dim) + "d.vtk";
    std::ofstream f(filename);
    GridOut().write_vtk(tria, f);
  }
#endif

  std::function<bool(const cell_iterator &)> predicate = pred_mat_id<dim>;

  // Compute a halo layer around material id 2 and set it to material id 3
  const std::vector<cell_iterator> cells_within_skin =
    GridTools::compute_active_cell_layer_within_distance(
      tria, predicate, 1.25); // General predicate

  // This test should fail if skin_thickness is 1./3. (which will accumulate an
  // extra layer of cells)

  AssertThrow(cells_within_skin.size() > 0, ExcMessage("No skin cells found."));
  for (typename std::vector<cell_iterator>::const_iterator it =
         cells_within_skin.begin();
       it != cells_within_skin.end();
       ++it)
    {
      (*it)->set_material_id(3);
    }

  deallog << "Grid with skin:" << std::endl;
  write_mat_id_to_file(tria);

#ifdef WRITE_VTK
  // Write to file to visually check result
  {
    const std::string filename =
      "grid_with_skin_" + Utilities::int_to_string(dim) + "d.vtk";
    std::ofstream f(filename);
    GridOut().write_vtk(tria, f);
  }
#endif
}


int
main()
{
  initlog();

  test<1>();
  test<2>(); // visually checked for 2D case
  // test<3> (); // the output is too long to include in the blessed file
  // visually checked for 3D case

  return 0;
}
