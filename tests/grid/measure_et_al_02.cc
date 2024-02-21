// ---------------------------------------------------------------------
//
// Copyright (C) 2005 - 2024 by the deal.II authors
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


// Computes diameter, extent_in_direction, and minimum_vertex_distance on a
// variety of cells

#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/tria.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>

#include "../tests.h"

#define PRECISION 5


template <int dim>
void
create_triangulation(const unsigned int, Triangulation<dim> &)
{
  DEAL_II_NOT_IMPLEMENTED();
}


template <>
void
create_triangulation(const unsigned int case_no, Triangulation<2> &tria)
{
  switch (case_no)
    {
      case 0:
        GridGenerator::hyper_cube(tria, 1., 3.);
        break;
      case 1:
        {
          GridGenerator::hyper_cube(tria, 1., 3.);
          Point<2> &v0 = tria.begin_active()->vertex(0);
          v0[0]        = 0.;
          break;
        }
      case 2:
        {
          GridGenerator::hyper_cube(tria, 1., 3.);
          Point<2> &v0 = tria.begin_active()->vertex(0);
          v0[0]        = 0.;
          Point<2> &v3 = tria.begin_active()->vertex(3);
          v3[0]        = 4.;
          break;
        }
      default:
        DEAL_II_NOT_IMPLEMENTED();
    };
}


template <>
void
create_triangulation(const unsigned int case_no, Triangulation<3> &tria)
{
  switch (case_no)
    {
      case 0:
        GridGenerator::hyper_cube(tria, 1., 3.);
        break;
      case 1:
      case 2: // like case 1
        {
          GridGenerator::hyper_cube(tria, 1., 3.);
          Point<3> &v0 = tria.begin_active()->vertex(0);
          v0[0]        = 0.;
          break;
        }
      default:
        DEAL_II_NOT_IMPLEMENTED();
    };
}


template <int dim>
void
test()
{
  Triangulation<dim> tria;
  for (unsigned int case_no = 0; case_no < 3; ++case_no)
    {
      create_triangulation(case_no, tria);
      deallog << "dim" << dim << ":case" << case_no
              << ":diameter=" << tria.begin_active()->diameter() << std::endl;
      deallog << "dim" << dim << ":case" << case_no << ":extent_in_direction="
              << tria.begin_active()->extent_in_direction(0) << std::endl;
      deallog << "dim" << dim << ":case" << case_no
              << ":minimum_vertex_distance="
              << tria.begin_active()->minimum_vertex_distance() << std::endl;

      const BoundingBox<dim> box = tria.begin_active()->bounding_box();
      const std::pair<Point<dim>, Point<dim>> &pts = box.get_boundary_points();
      deallog << "dim" << dim << ":case" << case_no
              << ":bounding_box=" << pts.first << ", " << pts.second
              << std::endl;
      tria.clear();
    }
}


int
main()
{
  initlog();
  deallog << std::setprecision(PRECISION);

  test<2>();
  test<3>();
}
