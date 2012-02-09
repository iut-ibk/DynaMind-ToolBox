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

#include <stdafx.h>
#include "sbisinv.h"

/*************************************************************************
Subroutine for finding the eigenvalues (and eigenvectors) of  a  symmetric
matrix  in  a  given half open interval (A, B] by using  a  bisection  and
inverse iteration

Input parameters:
    A       -   symmetric matrix which is given by its upper or lower
                triangular part. Array [0..N-1, 0..N-1].
    N       -   size of matrix A.
    ZNeeded -   flag controlling whether the eigenvectors are needed or not.
                If ZNeeded is equal to:
                 * 0, the eigenvectors are not returned;
                 * 1, the eigenvectors are returned.
    IsUpperA -  storage format of matrix A.
    B1, B2 -    half open interval (B1, B2] to search eigenvalues in.

Output parameters:
    M       -   number of eigenvalues found in a given half-interval (M>=0).
    W       -   array of the eigenvalues found.
                Array whose index ranges within [0..M-1].
    Z       -   if ZNeeded is equal to:
                 * 0, Z hasn’t changed;
                 * 1, Z contains eigenvectors.
                Array whose indexes range within [0..N-1, 0..M-1].
                The eigenvectors are stored in the matrix columns.

Result:
    True, if successful. M contains the number of eigenvalues in the given
    half-interval (could be equal to 0), W contains the eigenvalues,
    Z contains the eigenvectors (if needed).

    False, if the bisection method subroutine wasn't able to find the
    eigenvalues in the given interval or if the inverse iteration subroutine
    wasn't able to find all the corresponding eigenvectors.
    In that case, the eigenvalues and eigenvectors are not returned,
    M is equal to 0.

  -- ALGLIB --
     Copyright 07.01.2006 by Bochkanov Sergey
*************************************************************************/
bool smatrixevdr(ap::real_2d_array a,
     int n,
     int zneeded,
     bool isupper,
     double b1,
     double b2,
     int& m,
     ap::real_1d_array& w,
     ap::real_2d_array& z)
{
    bool result;
    ap::real_1d_array tau;
    ap::real_1d_array e;

    ap::ap_error::make_assertion(zneeded==0||zneeded==1, "SMatrixTDEVDR: incorrect ZNeeded");
    smatrixtd(a, n, isupper, tau, w, e);
    if( zneeded==1 )
    {
        smatrixtdunpackq(a, n, isupper, tau, z);
    }
    result = smatrixtdevdr(w, e, n, zneeded, b1, b2, m, z);
    return result;
}


/*************************************************************************
Subroutine for finding the eigenvalues and  eigenvectors  of  a  symmetric
matrix with given indexes by using bisection and inverse iteration methods.

Input parameters:
    A       -   symmetric matrix which is given by its upper or lower
                triangular part. Array whose indexes range within [0..N-1, 0..N-1].
    N       -   size of matrix A.
    ZNeeded -   flag controlling whether the eigenvectors are needed or not.
                If ZNeeded is equal to:
                 * 0, the eigenvectors are not returned;
                 * 1, the eigenvectors are returned.
    IsUpperA -  storage format of matrix A.
    I1, I2 -    index interval for searching (from I1 to I2).
                0 <= I1 <= I2 <= N-1.

Output parameters:
    W       -   array of the eigenvalues found.
                Array whose index ranges within [0..I2-I1].
    Z       -   if ZNeeded is equal to:
                 * 0, Z hasn’t changed;
                 * 1, Z contains eigenvectors.
                Array whose indexes range within [0..N-1, 0..I2-I1].
                In that case, the eigenvectors are stored in the matrix columns.

Result:
    True, if successful. W contains the eigenvalues, Z contains the
    eigenvectors (if needed).

    False, if the bisection method subroutine wasn't able to find the
    eigenvalues in the given interval or if the inverse iteration subroutine
    wasn't able to find all the corresponding eigenvectors.
    In that case, the eigenvalues and eigenvectors are not returned.

  -- ALGLIB --
     Copyright 07.01.2006 by Bochkanov Sergey
*************************************************************************/
bool smatrixevdi(ap::real_2d_array a,
     int n,
     int zneeded,
     bool isupper,
     int i1,
     int i2,
     ap::real_1d_array& w,
     ap::real_2d_array& z)
{
    bool result;
    ap::real_1d_array tau;
    ap::real_1d_array e;

    ap::ap_error::make_assertion(zneeded==0||zneeded==1, "SMatrixEVDI: incorrect ZNeeded");
    smatrixtd(a, n, isupper, tau, w, e);
    if( zneeded==1 )
    {
        smatrixtdunpackq(a, n, isupper, tau, z);
    }
    result = smatrixtdevdi(w, e, n, zneeded, i1, i2, z);
    return result;
}


bool symmetriceigenvaluesandvectorsininterval(ap::real_2d_array a,
     int n,
     int zneeded,
     bool isupper,
     double b1,
     double b2,
     int& m,
     ap::real_1d_array& w,
     ap::real_2d_array& z)
{
    bool result;
    ap::real_1d_array tau;
    ap::real_1d_array e;

    ap::ap_error::make_assertion(zneeded==0||zneeded==1, "SymmetricEigenValuesAndVectorsInInterval: incorrect ZNeeded");
    totridiagonal(a, n, isupper, tau, w, e);
    if( zneeded==1 )
    {
        unpackqfromtridiagonal(a, n, isupper, tau, z);
    }
    result = tridiagonaleigenvaluesandvectorsininterval(w, e, n, zneeded, b1, b2, m, z);
    return result;
}


bool symmetriceigenvaluesandvectorsbyindexes(ap::real_2d_array a,
     int n,
     int zneeded,
     bool isupper,
     int i1,
     int i2,
     ap::real_1d_array& w,
     ap::real_2d_array& z)
{
    bool result;
    ap::real_1d_array tau;
    ap::real_1d_array e;

    ap::ap_error::make_assertion(zneeded==0||zneeded==1, "SymmetricEigenValuesAndVectorsInInterval: incorrect ZNeeded");
    totridiagonal(a, n, isupper, tau, w, e);
    if( zneeded==1 )
    {
        unpackqfromtridiagonal(a, n, isupper, tau, z);
    }
    result = tridiagonaleigenvaluesandvectorsbyindexes(w, e, n, zneeded, i1, i2, z);
    return result;
}




