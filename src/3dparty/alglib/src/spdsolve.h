/*************************************************************************
This file is a part of ALGLIB project.

>>> SOURCE LICENSE >>>
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation (www.fsf.org); either version 2 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

A copy of the GNU General Public License is available at
http://www.fsf.org/licensing/licenses

>>> END OF LICENSE >>>
*************************************************************************/

#ifndef _spdsolve_h
#define _spdsolve_h

#include "ap.h"
#include "ialglib.h"

#include "cholesky.h"


/*************************************************************************
Solving a system of linear equations with a system  matrix  given  by  its
Cholesky decomposition.

The algorithm solves systems with a square matrix only.

Input parameters:
    A       -   Cholesky decomposition of a system matrix (the result of
                the SMatrixCholesky subroutine).
    B       -   right side of a system.
                Array whose index ranges within [0..N-1].
    N       -   size of matrix A.
    IsUpper -   points to the triangle of matrix A in which the Cholesky
                decomposition is stored. If IsUpper=True,  the  Cholesky
                decomposition has the form of U'*U, and the upper triangle
                of matrix A stores matrix U (in  that  case,  the  lower
                triangle isn’t used and isn’t changed by the subroutine)
                Similarly, if IsUpper = False, the Cholesky decomposition
                has the form of L*L',  and  the  lower  triangle  stores
                matrix L.

Output parameters:
    X       -   solution of a system.
                Array whose index ranges within [0..N-1].

Result:
    True, if the system is not singular. X contains the solution.
    False, if the system is singular (there is a zero element on the main
diagonal). In this case, X doesn't contain a solution.

  -- ALGLIB --
     Copyright 2005-2008 by Bochkanov Sergey
*************************************************************************/
bool spdmatrixcholeskysolve(const ap::real_2d_array& a,
     ap::real_1d_array b,
     int n,
     bool isupper,
     ap::real_1d_array& x);


/*************************************************************************
Solving a system of linear equations with  a  symmetric  positive-definite
matrix by using the Cholesky decomposition.

The algorithm solves a system of linear equations whose matrix is symmetric
and positive-definite.

Input parameters:
    A       -   upper or lower triangle part of a symmetric system matrix.
                Array whose indexes range within [0..N-1, 0..N-1].
    B       -   right side of a system.
                Array whose index ranges within [0..N-1].
    N       -   size of matrix A.
    IsUpper -   points to the triangle of matrix A in which the matrix is stored.

Output parameters:
    X       -   solution of a system.
                Array whose index ranges within [0..N-1].

Result:
    True, if the system is not singular.
    False, if the system is singular. In this case, X doesn't contain a
solution.

  -- ALGLIB --
     Copyright 2005-2008 by Bochkanov Sergey
*************************************************************************/
bool spdmatrixsolve(ap::real_2d_array a,
     ap::real_1d_array b,
     int n,
     bool isupper,
     ap::real_1d_array& x);


bool solvesystemcholesky(const ap::real_2d_array& a,
     ap::real_1d_array b,
     int n,
     bool isupper,
     ap::real_1d_array& x);


bool solvespdsystem(ap::real_2d_array a,
     ap::real_1d_array b,
     int n,
     bool isupper,
     ap::real_1d_array& x);


#endif

