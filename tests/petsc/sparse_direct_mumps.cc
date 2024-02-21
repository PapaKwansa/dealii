// ---------------------------------------------------------------------
//
// Copyright (C) 2004 - 2024 by the deal.II authors
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


// test the PETSc SparseDirectMumps solver


#include <deal.II/lac/petsc_precondition.h>
#include <deal.II/lac/petsc_solver.h>
#include <deal.II/lac/petsc_sparse_matrix.h>
#include <deal.II/lac/petsc_vector.h>
#include <deal.II/lac/vector_memory.h>

#include <iostream>
#include <typeinfo>

#include "../tests.h"

#include "../testmatrix.h"



int
main(int argc, char **argv)
{
  initlog();
  deallog << std::setprecision(4);

  Utilities::MPI::MPI_InitFinalize mpi_initialization(argc, argv, 1);
  {
    const unsigned int n_points = 32;
    unsigned int       size     = (n_points - 1) * (n_points - 1);

    deallog << "Size " << n_points << " Unknowns " << size << std::endl;

    // Make matrix
    FDMatrix                    testproblem(n_points, n_points);
    PETScWrappers::SparseMatrix A(size, size, 5);
    testproblem.five_point(A);

    IndexSet indices(size);
    indices.add_range(0, size);
    PETScWrappers::MPI::Vector f(indices, MPI_COMM_WORLD);
    PETScWrappers::MPI::Vector u(indices, MPI_COMM_WORLD);
    A.compress(VectorOperation::insert);

    // First test non-symmetric mode:
    {
      deallog << "Testing non-symmetric mode" << std::endl;

      u = 0.;
      f = 1.;

      SolverControl                    cn;
      PETScWrappers::SparseDirectMUMPS solver(cn);
      solver.solve(A, u, f);

      PETScWrappers::MPI::Vector tmp(indices, MPI_COMM_WORLD);
      deallog << "residual = " << A.residual(tmp, u, f) << std::endl;
    }

    // Now also test the case where the matrix can be assumed to be
    // symmetric (which of course it is here):
    {
      deallog << "Testing symmetric mode" << std::endl;

      u = 0.;
      f = 1.;

      SolverControl                    cn;
      PETScWrappers::SparseDirectMUMPS solver(cn);
      solver.set_symmetric_mode(true);
      solver.solve(A, u, f);

      PETScWrappers::MPI::Vector tmp(indices, MPI_COMM_WORLD);
      deallog << "residual = " << A.residual(tmp, u, f) << std::endl;
    }
  }
}
