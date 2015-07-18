/*************************************************************************
Copyright (c) 2005-2007, Sergey Bochkanov (ALGLIB project).

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

#ifndef _hevd_h
#define _hevd_h

#include "ap.h"
#include "ialglib.h"

#include "blas.h"
#include "rotations.h"
#include "tdevd.h"
#include "cblas.h"
#include "creflections.h"
#include "hblas.h"
#include "htridiagonal.h"


/*************************************************************************
Finding the eigenvalues and eigenvectors of a Hermitian matrix

The algorithm finds eigen pairs of a Hermitian matrix by  reducing  it  to
real tridiagonal form and using the QL/QR algorithm.

Input parameters:
    A       -   Hermitian matrix which is given  by  its  upper  or  lower
                triangular part.
                Array whose indexes range within [0..N-1, 0..N-1].
    N       -   size of matrix A.
    IsUpper -   storage format.
    ZNeeded -   flag controlling whether the eigenvectors  are  needed  or
                not. If ZNeeded is equal to:
                 * 0, the eigenvectors are not returned;
                 * 1, the eigenvectors are returned.

Output parameters:
    D       -   eigenvalues in ascending order.
                Array whose index ranges within [0..N-1].
    Z       -   if ZNeeded is equal to:
                 * 0, Z hasn’t changed;
                 * 1, Z contains the eigenvectors.
                Array whose indexes range within [0..N-1, 0..N-1].
                The eigenvectors are stored in the matrix columns.

Result:
    True, if the algorithm has converged.
    False, if the algorithm hasn't converged (rare case).

Note:
    eigen vectors of Hermitian matrix are defined up to multiplication  by
    a complex number L, such as |L|=1.

  -- ALGLIB --
     Copyright 2005, 23 March 2007 by Bochkanov Sergey
*************************************************************************/
bool hmatrixevd(ap::complex_2d_array a,
     int n,
     int zneeded,
     bool isupper,
     ap::real_1d_array& d,
     ap::complex_2d_array& z);


/*************************************************************************

  -- ALGLIB --
     Copyright 2005, 23 March 2007 by Bochkanov Sergey
*************************************************************************/
bool hermitianevd(ap::complex_2d_array a,
     int n,
     int zneeded,
     bool isupper,
     ap::real_1d_array& d,
     ap::complex_2d_array& z);


#endif

