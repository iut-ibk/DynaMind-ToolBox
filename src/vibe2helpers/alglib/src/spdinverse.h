/*************************************************************************
Copyright (c) 1992-2007 The University of Tennessee.  All rights reserved.

Contributors:
    * Sergey Bochkanov (ALGLIB project). Translation from FORTRAN to
      pseudocode.

See subroutines comments for additional copyrights.

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

#ifndef _spdinverse_h
#define _spdinverse_h

#include "ap.h"
#include "ialglib.h"

#include "cholesky.h"


/*************************************************************************
Inversion of a symmetric positive definite matrix which is given
by Cholesky decomposition.

Input parameters:
    A       -   Cholesky decomposition of the matrix to be inverted:
                A=U’*U or A = L*L'.
                Output of  CholeskyDecomposition subroutine.
                Array with elements [0..N-1, 0..N-1].
    N       -   size of matrix A.
    IsUpper –   storage format.
                If IsUpper = True, then matrix A is given as A = U'*U
                (matrix contains upper triangle).
                Similarly, if IsUpper = False, then A = L*L'.

Output parameters:
    A       -   upper or lower triangle of symmetric matrix A^-1, depending
                on the value of IsUpper.

Result:
    True, if the inversion succeeded.
    False, if matrix A contains zero elements on its main diagonal.
    Matrix A could not be inverted.

The algorithm is the modification of DPOTRI and DLAUU2 subroutines from
LAPACK library.
*************************************************************************/
bool spdmatrixcholeskyinverse(ap::real_2d_array& a, int n, bool isupper);


/*************************************************************************
Inversion of a symmetric positive definite matrix.

Given an upper or lower triangle of a symmetric positive definite matrix,
the algorithm generates matrix A^-1 and saves the upper or lower triangle
depending on the input.

Input parameters:
    A       -   matrix to be inverted (upper or lower triangle).
                Array with elements [0..N-1,0..N-1].
    N       -   size of matrix A.
    IsUpper -   storage format.
                If IsUpper = True, then the upper triangle of matrix A is
                given, otherwise the lower triangle is given.

Output parameters:
    A       -   inverse of matrix A.
                Array with elements [0..N-1,0..N-1].
                If IsUpper = True, then the upper triangle of matrix A^-1
                is used, and the elements below the main diagonal are not
                used nor changed. The same applies if IsUpper = False.

Result:
    True, if the matrix is positive definite.
    False, if the matrix is not positive definite (and it could not be
    inverted by this algorithm).
*************************************************************************/
bool spdmatrixinverse(ap::real_2d_array& a, int n, bool isupper);


bool inversecholesky(ap::real_2d_array& a, int n, bool isupper);


bool inversesymmetricpositivedefinite(ap::real_2d_array& a,
     int n,
     bool isupper);


#endif

