// ---------------------------------------------------------------------
//
// Copyright (C) 2020 - 2024 by the deal.II authors
//
// This file is part of the deal.II library.
//
// The deal.II library is free software; you can use it, redistribute
// it, and/or modify it under the terms of the GNU Lesser General
// Public License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// The full text of the license can be found in the file LICENSE at
// the top level of the deal.II distribution.
//
// ---------------------------------------------------------------------


// Another test for the optimizer. Here we compare the output of
// a direct call to SymEngine's optimizer to that returned by our wrappers
// to these classes.
//
// The expressions tested are non-trivial. It was initial thought that this
// would highlight a bug in one of the author's own codes, but turned out not
// to be the case. This bug was subsequently localised in symengine/cse_05.
// Nevertheless, having a test with a complex set of dependent variables is not
// a bad thing...

#include <deal.II/differentiation/sd.h>

#include <symengine/basic.h>
#include <symengine/lambda_double.h>
#include <symengine/parser.h>
#include <symengine/real_double.h>
#include <symengine/symbol.h>
#include <symengine/symengine_rcp.h>

#include <algorithm>
#include <map>
#include <sstream>
#include <vector>

#include "../tests.h"

namespace SE = SymEngine;
namespace SD = dealii::Differentiation::SD;

void
test_explicit()
{
  SE::RCP<const SE::Symbol> H_0      = SE::symbol("H_0");
  SE::RCP<const SE::Symbol> H_2      = SE::symbol("H_2");
  SE::RCP<const SE::Symbol> H_1      = SE::symbol("H_1");
  SE::RCP<const SE::Symbol> mu_r     = SE::symbol("mu_r");
  SE::RCP<const SE::Symbol> mu_e     = SE::symbol("mu_e");
  SE::RCP<const SE::Symbol> C_22     = SE::symbol("C_22");
  SE::RCP<const SE::Symbol> C_12     = SE::symbol("C_12");
  SE::RCP<const SE::Symbol> C_11     = SE::symbol("C_11");
  SE::RCP<const SE::Symbol> C_00     = SE::symbol("C_00");
  SE::RCP<const SE::Symbol> C_01     = SE::symbol("C_01");
  SE::RCP<const SE::Symbol> C_02     = SE::symbol("C_02");
  SE::RCP<const SE::Symbol> kappa    = SE::symbol("kappa");
  SE::RCP<const SE::Symbol> sf_sat_0 = SE::symbol("sf_sat_0");
  SE::RCP<const SE::Symbol> H_sat_0  = SE::symbol("H_sat_0");

  SE::vec_basic       independents      = {H_0,
                                           H_2,
                                           H_1,
                                           mu_r,
                                           mu_e,
                                           C_22,
                                           C_12,
                                           C_11,
                                           C_00,
                                           C_01,
                                           C_02,
                                           kappa,
                                           sf_sat_0,
                                           H_sat_0};
  std::vector<double> independents_vals = {0,
                                           75000,
                                           0,
                                           5.0,
                                           30000.0,
                                           0.9025,
                                           0,
                                           1.05263,
                                           1.05263,
                                           0,
                                           0,
                                           14990000.0,
                                           2.0,
                                           200000.0};

  SE::
    vec_basic dependents = {SE::parse("0.25*kappa*(-1.0 - C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00 - 2.0*log(sqrt(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00))) + 0.5*mu_e*(-3 + C_00*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-0.333333333333333) + C_11*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-0.333333333333333) + C_22*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-0.333333333333333))*(1.0 + erf(1.77245385090552*(H_0**2 + H_1**2 + H_2**2)/H_sat_0**2)*sf_sat_0) - 6.28318530717959e-07*mu_r*sqrt(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)*(H_0*(1.0*H_0*(C_22*C_11 - C_12**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(-C_11*C_02 + C_12*C_01)*H_2/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(C_12*C_02 - C_22*C_01)*H_1/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)) + H_1*(1.0*H_1*(C_22*C_00 - C_02**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_2*(C_01*C_02 - C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(C_12*C_02 - C_22*C_01)*H_0/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)) + H_2*(1.0*H_1*(C_01*C_02 - C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_2*(C_11*C_00 - C_01**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(-C_11*C_02 + C_12*C_01)*H_0/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)))"), SE::parse("2.0*(0.0 + 0.25*kappa*(C_22*C_11 - 1.0*(C_22*C_11 - C_12**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00) - C_12**2) + 0.5*mu_e*(1.0 + erf(1.77245385090552*(H_0**2 + H_1**2 + H_2**2)/H_sat_0**2)*sf_sat_0)*(-0.333333333333333*C_00*(C_22*C_11 - C_12**2)*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-1.33333333333333) - 0.333333333333333*C_11*(C_22*C_11 - C_12**2)*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-1.33333333333333) - 0.333333333333333*C_22*(C_22*C_11 - C_12**2)*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-1.33333333333333) + (-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-0.333333333333333)) - 6.28318530717959e-07*mu_r*(H_0*(-1.0*H_0*(C_22*C_11 - C_12**2)**2/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*(-C_11*C_02 + C_12*C_01)*H_2*(C_22*C_11 - C_12**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*(C_12*C_02 - C_22*C_01)*H_1*(C_22*C_11 - C_12**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2) + H_1*(0.0 + 1.0*H_1*C_22/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) - 1.0*H_2*C_12/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) - 1.0*H_1*(C_22*C_00 - C_02**2)*(C_22*C_11 - C_12**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*H_2*(C_01*C_02 - C_12*C_00)*(C_22*C_11 - C_12**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*(C_12*C_02 - C_22*C_01)*H_0*(C_22*C_11 - C_12**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2) + H_2*(0.0 - 1.0*H_1*C_12/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_2*C_11/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) - 1.0*H_1*(C_01*C_02 - C_12*C_00)*(C_22*C_11 - C_12**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*H_2*(C_11*C_00 - C_01**2)*(C_22*C_11 - C_12**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*(-C_11*C_02 + C_12*C_01)*H_0*(C_22*C_11 - C_12**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2))*sqrt(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00) - 3.14159265358979e-07*mu_r*(C_22*C_11 - C_12**2)*(H_0*(1.0*H_0*(C_22*C_11 - C_12**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(-C_11*C_02 + C_12*C_01)*H_2/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(C_12*C_02 - C_22*C_01)*H_1/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)) + H_1*(1.0*H_1*(C_22*C_00 - C_02**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_2*(C_01*C_02 - C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(C_12*C_02 - C_22*C_01)*H_0/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)) + H_2*(1.0*H_1*(C_01*C_02 - C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_2*(C_11*C_00 - C_01**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(-C_11*C_02 + C_12*C_01)*H_0/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)))/sqrt(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00))"), SE::parse("2.0*(0.0 + 0.25*kappa*(C_22*C_00 - 1.0*(C_22*C_00 - C_02**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00) - C_02**2) + 0.5*mu_e*(1.0 + erf(1.77245385090552*(H_0**2 + H_1**2 + H_2**2)/H_sat_0**2)*sf_sat_0)*(-0.333333333333333*C_00*(C_22*C_00 - C_02**2)*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-1.33333333333333) - 0.333333333333333*C_11*(C_22*C_00 - C_02**2)*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-1.33333333333333) - 0.333333333333333*C_22*(C_22*C_00 - C_02**2)*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-1.33333333333333) + (-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-0.333333333333333)) - 6.28318530717959e-07*mu_r*sqrt(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)*(H_0*(0.0 + 1.0*H_0*C_22/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) - 1.0*H_2*C_02/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) - 1.0*H_0*(C_22*C_00 - C_02**2)*(C_22*C_11 - C_12**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*(-C_11*C_02 + C_12*C_01)*H_2*(C_22*C_00 - C_02**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*(C_12*C_02 - C_22*C_01)*H_1*(C_22*C_00 - C_02**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2) + H_1*(-1.0*H_1*(C_22*C_00 - C_02**2)**2/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*H_2*(C_01*C_02 - C_12*C_00)*(C_22*C_00 - C_02**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*(C_12*C_02 - C_22*C_01)*H_0*(C_22*C_00 - C_02**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2) + H_2*(0.0 - 1.0*H_0*C_02/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_2*C_00/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) - 1.0*H_1*(C_01*C_02 - C_12*C_00)*(C_22*C_00 - C_02**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*H_2*(C_11*C_00 - C_01**2)*(C_22*C_00 - C_02**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*(-C_11*C_02 + C_12*C_01)*H_0*(C_22*C_00 - C_02**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2)) - 3.14159265358979e-07*mu_r*(C_22*C_00 - C_02**2)*(H_0*(1.0*H_0*(C_22*C_11 - C_12**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(-C_11*C_02 + C_12*C_01)*H_2/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(C_12*C_02 - C_22*C_01)*H_1/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)) + H_1*(1.0*H_1*(C_22*C_00 - C_02**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_2*(C_01*C_02 - C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(C_12*C_02 - C_22*C_01)*H_0/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)) + H_2*(1.0*H_1*(C_01*C_02 - C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_2*(C_11*C_00 - C_01**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(-C_11*C_02 + C_12*C_01)*H_0/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)))/sqrt(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00))"), SE::parse("2.0*(0.0 + 0.25*kappa*(C_11*C_00 - 1.0*(C_11*C_00 - C_01**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00) - C_01**2) + 0.5*mu_e*(-0.333333333333333*C_00*(C_11*C_00 - C_01**2)*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-1.33333333333333) - 0.333333333333333*C_11*(C_11*C_00 - C_01**2)*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-1.33333333333333) - 0.333333333333333*C_22*(C_11*C_00 - C_01**2)*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-1.33333333333333) + (-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-0.333333333333333))*(1.0 + erf(1.77245385090552*(H_0**2 + H_1**2 + H_2**2)/H_sat_0**2)*sf_sat_0) - 6.28318530717959e-07*mu_r*sqrt(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)*(H_0*(0.0 + 1.0*H_0*C_11/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) - 1.0*H_1*C_01/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) - 1.0*H_0*(C_11*C_00 - C_01**2)*(C_22*C_11 - C_12**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*(-C_11*C_02 + C_12*C_01)*H_2*(C_11*C_00 - C_01**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*(C_12*C_02 - C_22*C_01)*H_1*(C_11*C_00 - C_01**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2) + H_1*(0.0 - 1.0*H_0*C_01/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_1*C_00/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) - 1.0*H_1*(C_11*C_00 - C_01**2)*(C_22*C_00 - C_02**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*H_2*(C_01*C_02 - C_12*C_00)*(C_11*C_00 - C_01**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*(C_12*C_02 - C_22*C_01)*H_0*(C_11*C_00 - C_01**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2) + H_2*(-1.0*H_2*(C_11*C_00 - C_01**2)**2/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*H_1*(C_01*C_02 - C_12*C_00)*(C_11*C_00 - C_01**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*(-C_11*C_02 + C_12*C_01)*H_0*(C_11*C_00 - C_01**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2)) - 3.14159265358979e-07*mu_r*(C_11*C_00 - C_01**2)*(H_0*(1.0*H_0*(C_22*C_11 - C_12**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(-C_11*C_02 + C_12*C_01)*H_2/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(C_12*C_02 - C_22*C_01)*H_1/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)) + H_1*(1.0*H_1*(C_22*C_00 - C_02**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_2*(C_01*C_02 - C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(C_12*C_02 - C_22*C_01)*H_0/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)) + H_2*(1.0*H_1*(C_01*C_02 - C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_2*(C_11*C_00 - C_01**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(-C_11*C_02 + C_12*C_01)*H_0/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)))/sqrt(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00))"), SE::parse("1.0*(0.0 + 0.25*kappa*(2*C_12*C_02 - 2*C_22*C_01 - 1.0*(2*C_12*C_02 - 2*C_22*C_01)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)) + 0.5*mu_e*(1.0 + erf(1.77245385090552*(H_0**2 + H_1**2 + H_2**2)/H_sat_0**2)*sf_sat_0)*(-0.333333333333333*(2*C_12*C_02 - 2*C_22*C_01)*C_00*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-1.33333333333333) - 0.333333333333333*(2*C_12*C_02 - 2*C_22*C_01)*C_11*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-1.33333333333333) - 0.333333333333333*(2*C_12*C_02 - 2*C_22*C_01)*C_22*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-1.33333333333333)) - 6.28318530717959e-07*mu_r*sqrt(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)*(H_0*(0.0 - 1.0*H_1*C_22/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_2*C_12/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) - 1.0*H_0*(C_22*C_11 - C_12**2)*(2.0*C_12*C_02 - 2*C_22*C_01)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*(-C_11*C_02 + C_12*C_01)*H_2*(2.0*C_12*C_02 - 2*C_22*C_01)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*(C_12*C_02 - C_22*C_01)*H_1*(2.0*C_12*C_02 - 2*C_22*C_01)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2) + H_1*(0.0 - 1.0*H_0*C_22/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_2*C_02/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) - 1.0*H_1*(C_22*C_00 - C_02**2)*(2.0*C_12*C_02 - 2*C_22*C_01)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*H_2*(C_01*C_02 - C_12*C_00)*(2.0*C_12*C_02 - 2*C_22*C_01)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*(C_12*C_02 - C_22*C_01)*H_0*(2.0*C_12*C_02 - 2*C_22*C_01)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2) + H_2*(0.0 + 1.0*H_0*C_12/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_1*C_02/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) - 2.0*H_2*C_01/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) - 1.0*H_1*(C_01*C_02 - C_12*C_00)*(2.0*C_12*C_02 - 2*C_22*C_01)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*H_2*(C_11*C_00 - C_01**2)*(2.0*C_12*C_02 - 2*C_22*C_01)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*(-C_11*C_02 + C_12*C_01)*H_0*(2.0*C_12*C_02 - 2*C_22*C_01)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2)) - 3.14159265358979e-07*(2*C_12*C_02 - 2*C_22*C_01)*mu_r*(H_0*(1.0*H_0*(C_22*C_11 - C_12**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(-C_11*C_02 + C_12*C_01)*H_2/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(C_12*C_02 - C_22*C_01)*H_1/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)) + H_1*(1.0*H_1*(C_22*C_00 - C_02**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_2*(C_01*C_02 - C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(C_12*C_02 - C_22*C_01)*H_0/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)) + H_2*(1.0*H_1*(C_01*C_02 - C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_2*(C_11*C_00 - C_01**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(-C_11*C_02 + C_12*C_01)*H_0/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)))/sqrt(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00))"), SE::parse("1.0*(0.0 + 0.25*kappa*(-2*C_11*C_02 + 2*C_12*C_01 - 1.0*(-2*C_11*C_02 + 2*C_12*C_01)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)) + 0.5*mu_e*(1.0 + erf(1.77245385090552*(H_0**2 + H_1**2 + H_2**2)/H_sat_0**2)*sf_sat_0)*(-0.333333333333333*(-2*C_11*C_02 + 2*C_12*C_01)*C_00*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-1.33333333333333) - 0.333333333333333*(-2*C_11*C_02 + 2*C_12*C_01)*C_11*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-1.33333333333333) - 0.333333333333333*(-2*C_11*C_02 + 2*C_12*C_01)*C_22*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-1.33333333333333)) - 6.28318530717959e-07*mu_r*sqrt(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)*(H_0*(0.0 + 1.0*H_1*C_12/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) - 1.0*H_2*C_11/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) - 1.0*H_0*(C_22*C_11 - C_12**2)*(-2*C_11*C_02 + 2.0*C_12*C_01)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*(-C_11*C_02 + C_12*C_01)*H_2*(-2*C_11*C_02 + 2.0*C_12*C_01)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*(C_12*C_02 - C_22*C_01)*H_1*(-2*C_11*C_02 + 2.0*C_12*C_01)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2) + H_1*(0.0 + 1.0*H_0*C_12/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) - 2.0*H_1*C_02/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_2*C_01/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) - 1.0*H_1*(C_22*C_00 - C_02**2)*(-2*C_11*C_02 + 2.0*C_12*C_01)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*H_2*(C_01*C_02 - C_12*C_00)*(-2*C_11*C_02 + 2.0*C_12*C_01)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*(C_12*C_02 - C_22*C_01)*H_0*(-2*C_11*C_02 + 2.0*C_12*C_01)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2) + H_2*(0.0 - 1.0*H_0*C_11/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_1*C_01/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) - 1.0*H_1*(C_01*C_02 - C_12*C_00)*(-2*C_11*C_02 + 2.0*C_12*C_01)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*H_2*(C_11*C_00 - C_01**2)*(-2*C_11*C_02 + 2.0*C_12*C_01)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*(-C_11*C_02 + C_12*C_01)*H_0*(-2*C_11*C_02 + 2.0*C_12*C_01)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2)) - 3.14159265358979e-07*(-2*C_11*C_02 + 2*C_12*C_01)*mu_r*(H_0*(1.0*H_0*(C_22*C_11 - C_12**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(-C_11*C_02 + C_12*C_01)*H_2/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(C_12*C_02 - C_22*C_01)*H_1/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)) + H_1*(1.0*H_1*(C_22*C_00 - C_02**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_2*(C_01*C_02 - C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(C_12*C_02 - C_22*C_01)*H_0/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)) + H_2*(1.0*H_1*(C_01*C_02 - C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_2*(C_11*C_00 - C_01**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(-C_11*C_02 + C_12*C_01)*H_0/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)))/sqrt(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00))"), SE::parse("1.0*(0.0 + 0.25*kappa*(2*C_01*C_02 - 2*C_12*C_00 - 1.0*(2*C_01*C_02 - 2*C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)) + 0.5*mu_e*(1.0 + erf(1.77245385090552*(H_0**2 + H_1**2 + H_2**2)/H_sat_0**2)*sf_sat_0)*(-0.333333333333333*(2*C_01*C_02 - 2*C_12*C_00)*C_00*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-1.33333333333333) - 0.333333333333333*(2*C_01*C_02 - 2*C_12*C_00)*C_11*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-1.33333333333333) - 0.333333333333333*(2*C_01*C_02 - 2*C_12*C_00)*C_22*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-1.33333333333333)) - 6.28318530717959e-07*mu_r*(H_0*(0.0 - 2.0*H_0*C_12/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_1*C_02/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_2*C_01/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) - 1.0*H_0*(C_22*C_11 - C_12**2)*(2.0*C_01*C_02 - 2*C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*(-C_11*C_02 + C_12*C_01)*H_2*(2.0*C_01*C_02 - 2*C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*(C_12*C_02 - C_22*C_01)*H_1*(2.0*C_01*C_02 - 2*C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2) + H_1*(0.0 + 1.0*H_0*C_02/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) - 1.0*H_2*C_00/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) - 1.0*H_1*(C_22*C_00 - C_02**2)*(2.0*C_01*C_02 - 2*C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*H_2*(C_01*C_02 - C_12*C_00)*(2.0*C_01*C_02 - 2*C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*(C_12*C_02 - C_22*C_01)*H_0*(2.0*C_01*C_02 - 2*C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2) + H_2*(0.0 + 1.0*H_0*C_01/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) - 1.0*H_1*C_00/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) - 1.0*H_1*(C_01*C_02 - C_12*C_00)*(2.0*C_01*C_02 - 2*C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*H_2*(C_11*C_00 - C_01**2)*(2.0*C_01*C_02 - 2*C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2 - 1.0*(-C_11*C_02 + C_12*C_01)*H_0*(2.0*C_01*C_02 - 2*C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)**2))*sqrt(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00) - 3.14159265358979e-07*(2*C_01*C_02 - 2*C_12*C_00)*mu_r*(H_0*(1.0*H_0*(C_22*C_11 - C_12**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(-C_11*C_02 + C_12*C_01)*H_2/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(C_12*C_02 - C_22*C_01)*H_1/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)) + H_1*(1.0*H_1*(C_22*C_00 - C_02**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_2*(C_01*C_02 - C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(C_12*C_02 - C_22*C_01)*H_0/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)) + H_2*(1.0*H_1*(C_01*C_02 - C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_2*(C_11*C_00 - C_01**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(-C_11*C_02 + C_12*C_01)*H_0/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)))/sqrt(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00))"), SE::parse("-(-6.28318530717959e-07*mu_r*(1.0*H_0*(C_22*C_11 - C_12**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(-C_11*C_02 + C_12*C_01)*H_2/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(C_12*C_02 - C_22*C_01)*H_1/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_0*(C_22*C_11 - C_12**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(-C_11*C_02 + C_12*C_01)*H_2/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(C_12*C_02 - C_22*C_01)*H_1/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00))*sqrt(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00) + 3.54490770181103*exp(-3.14159265358979*(H_0**2 + H_1**2 + H_2**2)**2/H_sat_0**4)*H_0*mu_e*(-3 + C_00*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-0.333333333333333) + C_11*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-0.333333333333333) + C_22*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-0.333333333333333))*sf_sat_0/(sqrt(pi)*H_sat_0**2))"), SE::parse("-(-6.28318530717959e-07*mu_r*sqrt(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)*(1.0*H_1*(C_22*C_00 - C_02**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_2*(C_01*C_02 - C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(C_12*C_02 - C_22*C_01)*H_0/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_1*(C_22*C_00 - C_02**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_2*(C_01*C_02 - C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(C_12*C_02 - C_22*C_01)*H_0/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)) + 3.54490770181103*exp(-3.14159265358979*(H_0**2 + H_1**2 + H_2**2)**2/H_sat_0**4)*H_1*mu_e*(-3 + C_00*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-0.333333333333333) + C_11*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-0.333333333333333) + C_22*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-0.333333333333333))*sf_sat_0/(sqrt(pi)*H_sat_0**2))"), SE::parse("-(-6.28318530717959e-07*mu_r*sqrt(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)*(1.0*H_1*(C_01*C_02 - C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_2*(C_11*C_00 - C_01**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(-C_11*C_02 + C_12*C_01)*H_0/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_1*(C_01*C_02 - C_12*C_00)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*H_2*(C_11*C_00 - C_01**2)/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00) + 1.0*(-C_11*C_02 + C_12*C_01)*H_0/(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2.0*C_12*C_01*C_02 + C_22*C_11*C_00)) + 3.54490770181103*exp(-3.14159265358979*(H_0**2 + H_1**2 + H_2**2)**2/H_sat_0**4)*H_2*mu_e*(-3 + C_00*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-0.333333333333333) + C_11*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-0.333333333333333) + C_22*(-C_11*C_02**2 - C_12**2*C_00 - C_22*C_01**2 + 2*C_12*C_01*C_02 + C_22*C_11*C_00)**(-0.333333333333333))*sf_sat_0/(sqrt(pi)*H_sat_0**2))")};

  std::vector<double> vals(independents.size());
  for (unsigned int i = 0; i < independents.size(); ++i)
    vals[i] = independents_vals[i];

  auto eval_symengine =
    [&independents, &dependents, &vals](const bool         with_cse,
                                        const unsigned int n_evals) {
      SE::LambdaRealDoubleVisitor l;
      l.init(independents, dependents, with_cse);
      std::vector<double> result(dependents.size());

      std::cout << "Explicit: eval_symengine: "
                << "  with_lambda_opt: " << (true) << "  with_cse: " << with_cse
                << "  n_evals: " << n_evals << std::endl;
      double res = 0.0;
      for (unsigned int r = 0; r < n_evals; ++r)
        {
          l.call(result.data(), vals.data());
          for (unsigned int i = 0; i < result.size(); ++i)
            {
              std::cout << "Result[" << i << "]: " << result[i] << std::endl;
              res += result[i];
            }
        }
      deallog << "res: " << std::setprecision(9) << res << std::endl;
    };

  auto eval_deal_II = [&independents,
                       &dependents,
                       &vals](const bool         with_lambda_opt,
                              const bool         with_cse,
                              const unsigned int n_evals) {
    // Configure optimiser
    SD::BatchOptimizer<double> optimiser;
    if (with_lambda_opt == false && with_cse == false)
      optimiser.set_optimization_method(
        SD::OptimizerType::dictionary, SD::OptimizationFlags::optimize_default);
    else if (with_lambda_opt == false && with_cse == true)
      optimiser.set_optimization_method(SD::OptimizerType::dictionary,
                                        SD::OptimizationFlags::optimize_cse);
    else if (with_lambda_opt == true && with_cse == false)
      optimiser.set_optimization_method(
        SD::OptimizerType::lambda, SD::OptimizationFlags::optimize_default);
    else // if (with_lambda_opt == true && with_cse == true)
      optimiser.set_optimization_method(SD::OptimizerType::lambda,
                                        SD::OptimizationFlags::optimize_cse);

    // Set independent and dependent variables, then finalise
    optimiser.register_symbols(independents);
    optimiser.register_functions(dependents);
    optimiser.optimize();

    // Substitute and get result
    std::cout << "Explicit: eval_deal_II: "
              << "  with_lambda_opt: " << with_lambda_opt
              << "  with_cse: " << with_cse << "  n_evals: " << n_evals
              << std::endl;
    double res = 0.0;
    for (unsigned int r = 0; r < n_evals; ++r)
      {
        optimiser.substitute(independents, vals);
        const std::vector<double> result = optimiser.evaluate();

        for (unsigned int i = 0; i < result.size(); ++i)
          {
            std::cout << "Result[" << i << "]: " << result[i] << std::endl;
            res += result[i];
          }
      }
    deallog << "res: " << std::setprecision(9) << res << std::endl;
  };

  // Perform more than one evaluation to check that all relevant internal
  // data is correctly reset/reinitialized upon the second and later calls
  // to call()/substitute().
  const unsigned int n_evals = 2;

  deallog.push("SymEngine: No CSE");
  {
    eval_symengine(false, n_evals);
  }
  deallog.pop();

  deallog.push("SymEngine: With CSE");
  {
    eval_symengine(true, n_evals);
  }
  deallog.pop();

  deallog.push("deal.II: Dict, No CSE");
  {
    eval_deal_II(false, false, n_evals);
  }
  deallog.pop();

  deallog.push("deal.II: Dict, With CSE");
  {
    eval_deal_II(false, true, n_evals);
  }
  deallog.pop();

  deallog.push("deal.II: Lambda, No CSE");
  {
    eval_deal_II(true, false, n_evals);
  }
  deallog.pop();

  deallog.push("deal.II: Lambda, With CSE");
  {
    eval_deal_II(true, true, n_evals);
  }
  deallog.pop();
}


int
main(int argc, char *argv[])
{
  initlog();

  test_explicit();
  deallog << "OK" << std::endl;

  return 0;
}
