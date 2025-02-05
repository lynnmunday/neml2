// Copyright 2023, UChicago Argonne, LLC
// All Rights Reserved
// Software Name: NEML2 -- the New Engineering material Model Library, version 2
// By: Argonne National Laboratory
// OPEN SOURCE LICENSE (MIT)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "neml2/solvers/NewtonNonlinearSolver.h"
#include <iomanip>

namespace neml2
{
register_NEML2_object(NewtonNonlinearSolver);

OptionSet
NewtonNonlinearSolver::expected_options()
{
  OptionSet options = NonlinearSolver::expected_options();
  return options;
}

BatchTensor
NewtonNonlinearSolver::solve(const NonlinearSystem & system, const BatchTensor & x0) const
{
  // Setup initial guess and initial residual
  auto x = x0.clone();
  auto R = system.residual(x);
  auto nR0 = torch::linalg::vector_norm(R, 2, -1, false, c10::nullopt);

  // Check for initial convergence
  if (converged(0, nR0, nR0))
    return x;

  // Begin iterating
  BatchTensor J = system.Jacobian(x);
  update(x, R, J);

  // Continuing iterating until one of:
  // 1. nR < atol (success)
  // 2. nR / nR0 < rtol (success)
  // 3. i > miters (failure)
  for (size_t i = 1; i < miters; i++)
  {
    // Update R and the norm of R
    std::tie(R, J) = system.residual_and_Jacobian(x);
    auto nR = torch::linalg::vector_norm(R, 2, -1, false, c10::nullopt);

    // Check for initial convergence
    if (converged(i, nR, nR0))
      return x;

    update(x, R, J);
  }

  // Throw if we exceeded miters
  throw NEMLException("Nonlinear solver exceeded miters!");

  return x;
}

void
NewtonNonlinearSolver::update(BatchTensor & x, const BatchTensor & R, const BatchTensor & J) const
{
#if (TORCH_VERSION_MAJOR > 1 || TORCH_VERSION_MINOR > 12)
  x -= torch::linalg::solve(J, R, true);
#else
  x -= torch::linalg::solve(J, R);
#endif
}

bool
NewtonNonlinearSolver::converged(size_t itr,
                                 const torch::Tensor & nR,
                                 const torch::Tensor & nR0) const
{
  // LCOV_EXCL_START
  if (verbose)
    std::cout << "ITERATION " << std::setw(3) << itr << ", |R| = " << std::scientific
              << torch::mean(nR).item<double>() << ", |R0| = " << std::scientific
              << torch::mean(nR0).item<double>() << std::endl;
  // LCOV_EXCL_STOP

  return torch::all(nR < atol).item<bool>() || torch::all(nR / nR0 < rtol).item<bool>();
}
} // namespace neml2
