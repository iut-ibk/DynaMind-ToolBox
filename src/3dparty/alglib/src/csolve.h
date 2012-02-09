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

#ifndef _csolve_h
#define _csolve_h

#include "ap.h"
#include "ialglib.h"

#include "clu.h"


/*************************************************************************
Solving a system of linear equations with a system matrix given by its
LU decomposition.

The algorithm solves a system of linear equations whose matrix is given by
its LU decomposition. In case of a singular matrix, the algorithm  returns
False.

The algorithm solves systems with a square matrix only.

Input parameters:
    A       -   LU decomposition of a system matrix in compact  form  (the
                result of the RMatrixLU subroutine).
    Pivots  -   row permutation table (the result of a
                RMatrixLU subroutine).
    B       -   right side of a system.
                Array whose index ranges within [0..N-1].
    N       -   size of matrix A.

Output parameters:
    X       -   solution of a system.
                Array whose index ranges within [0..N-1].

Result:
    True, if the matrix is not singular.
    False, if the matrux is singular. In this case, X doesn't contain a
solution.

  -- ALGLIB --
     Copyright 2005-2008 by Bochkanov Sergey
*************************************************************************/
bool cmatrixlusolve(const ap::complex_2d_array& a,
     const ap::integer_1d_array& pivots,
     ap::complex_1d_array b,
     int n,
     ap::complex_1d_array& x);


/*************************************************************************
Solving a system of linear equations.

The algorithm solves a system of linear equations by using the
LU decomposition. The algorithm solves systems with a square matrix only.

Input parameters:
    A   -   system matrix.
            Array whose indexes range within [0..N-1, 0..N-1].
    B   -   right side of a system.
            Array whose indexes range within [0..N-1].
    N   -   size of matrix A.

Output parameters:
    X   -   solution of a system.
            Array whose index ranges within [0..N-1].

Result:
    True, if the matrix is not singular.
    False, if the matrix is singular. In this case, X doesn't contain a
solution.

  -- ALGLIB --
     Copyright 2005-2008 by Bochkanov Sergey
*************************************************************************/
bool cmatrixsolve(ap::complex_2d_array a,
     ap::complex_1d_array b,
     int n,
     ap::complex_1d_array& x);


bool complexsolvesystemlu(const ap::complex_2d_array& a,
     const ap::integer_1d_array& pivots,
     ap::complex_1d_array b,
     int n,
     ap::complex_1d_array& x);


bool complexsolvesystem(ap::complex_2d_array a,
     ap::complex_1d_array b,
     int n,
     ap::complex_1d_array& x);


#endif

