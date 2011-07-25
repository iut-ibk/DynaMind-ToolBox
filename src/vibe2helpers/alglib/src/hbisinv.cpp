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
#include "hbisinv.h"

/*************************************************************************
Subroutine for finding the eigenvalues (and eigenvectors) of  a  Hermitian
matrix  in  a  given half-interval (A, B] by using a bisection and inverse
iteration

Input parameters:
    A       -   Hermitian matrix which is given  by  its  upper  or  lower
                triangular  part.  Array  whose   indexes   range   within
                [0..N-1, 0..N-1].
    N       -   size of matrix A.
    ZNeeded -   flag controlling whether the eigenvectors  are  needed  or
                not. If ZNeeded is equal to:
                 * 0, the eigenvectors are not returned;
                 * 1, the eigenvectors are returned.
    IsUpperA -  storage format of matrix A.
    B1, B2 -    half-interval (B1, B2] to search eigenvalues in.

Output parameters:
    M       -   number of eigenvalues found in a given half-interval, M>=0
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

    False, if the bisection method subroutine  wasn't  able  to  find  the
    eigenvalues  in  the  given  interval  or  if  the  inverse  iteration
    subroutine  wasn't  able  to  find all the corresponding eigenvectors.
    In that case, the eigenvalues and eigenvectors are not returned, M  is
    equal to 0.

Note:
    eigen vectors of Hermitian matrix are defined up to multiplication  by
    a complex number L, such as |L|=1.

  -- ALGLIB --
     Copyright 07.01.2006, 24.03.2007 by Bochkanov Sergey.
*************************************************************************/
bool hmatrixevdr(ap::complex_2d_array a,
     int n,
     int zneeded,
     bool isupper,
     double b1,
     double b2,
     int& m,
     ap::real_1d_array& w,
     ap::complex_2d_array& z)
{
    bool result;
    ap::complex_2d_array q;
    ap::real_2d_array t;
    ap::complex_1d_array tau;
    ap::real_1d_array e;
    ap::real_1d_array work;
    int i;
    int k;
    double v;

    ap::ap_error::make_assertion(zneeded==0||zneeded==1, "HermitianEigenValuesAndVectorsInInterval: incorrect ZNeeded");
    
    //
    // Reduce to tridiagonal form
    //
    hmatrixtd(a, n, isupper, tau, w, e);
    if( zneeded==1 )
    {
        hmatrixtdunpackq(a, n, isupper, tau, q);
        zneeded = 2;
    }
    
    //
    // Bisection and inverse iteration
    //
    result = smatrixtdevdr(w, e, n, zneeded, b1, b2, m, t);
    
    //
    // Eigenvectors are needed
    // Calculate Z = Q*T = Re(Q)*T + i*Im(Q)*T
    //
    if( result&&zneeded!=0&&m!=0 )
    {
        work.setbounds(0, m-1);
        z.setbounds(0, n-1, 0, m-1);
        for(i = 0; i <= n-1; i++)
        {
            
            //
            // Calculate real part
            //
            for(k = 0; k <= m-1; k++)
            {
                work(k) = 0;
            }
            for(k = 0; k <= n-1; k++)
            {
                v = q(i,k).x;
                ap::vadd(&work(0), &t(k, 0), ap::vlen(0,m-1), v);
            }
            for(k = 0; k <= m-1; k++)
            {
                z(i,k).x = work(k);
            }
            
            //
            // Calculate imaginary part
            //
            for(k = 0; k <= m-1; k++)
            {
                work(k) = 0;
            }
            for(k = 0; k <= n-1; k++)
            {
                v = q(i,k).y;
                ap::vadd(&work(0), &t(k, 0), ap::vlen(0,m-1), v);
            }
            for(k = 0; k <= m-1; k++)
            {
                z(i,k).y = work(k);
            }
        }
    }
    return result;
}


/*************************************************************************
Subroutine for finding the eigenvalues and  eigenvectors  of  a  Hermitian
matrix with given indexes by using bisection and inverse iteration methods

Input parameters:
    A       -   Hermitian matrix which is given  by  its  upper  or  lower
                triangular part.
                Array whose indexes range within [0..N-1, 0..N-1].
    N       -   size of matrix A.
    ZNeeded -   flag controlling whether the eigenvectors  are  needed  or
                not. If ZNeeded is equal to:
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
                In  that  case,  the eigenvectors are stored in the matrix
                columns.

Result:
    True, if successful. W contains the eigenvalues, Z contains the
    eigenvectors (if needed).

    False, if the bisection method subroutine  wasn't  able  to  find  the
    eigenvalues  in  the  given  interval  or  if  the  inverse  iteration
    subroutine wasn't able to find  all  the  corresponding  eigenvectors.
    In that case, the eigenvalues and eigenvectors are not returned.

Note:
    eigen vectors of Hermitian matrix are defined up to multiplication  by
    a complex number L, such as |L|=1.

  -- ALGLIB --
     Copyright 07.01.2006, 24.03.2007 by Bochkanov Sergey.
*************************************************************************/
bool hmatrixevdi(ap::complex_2d_array a,
     int n,
     int zneeded,
     bool isupper,
     int i1,
     int i2,
     ap::real_1d_array& w,
     ap::complex_2d_array& z)
{
    bool result;
    ap::complex_2d_array q;
    ap::real_2d_array t;
    ap::complex_1d_array tau;
    ap::real_1d_array e;
    ap::real_1d_array work;
    int i;
    int k;
    double v;
    int m;

    ap::ap_error::make_assertion(zneeded==0||zneeded==1, "HermitianEigenValuesAndVectorsByIndexes: incorrect ZNeeded");
    
    //
    // Reduce to tridiagonal form
    //
    hmatrixtd(a, n, isupper, tau, w, e);
    if( zneeded==1 )
    {
        hmatrixtdunpackq(a, n, isupper, tau, q);
        zneeded = 2;
    }
    
    //
    // Bisection and inverse iteration
    //
    result = smatrixtdevdi(w, e, n, zneeded, i1, i2, t);
    
    //
    // Eigenvectors are needed
    // Calculate Z = Q*T = Re(Q)*T + i*Im(Q)*T
    //
    m = i2-i1+1;
    if( result&&zneeded!=0 )
    {
        work.setbounds(0, m-1);
        z.setbounds(0, n-1, 0, m-1);
        for(i = 0; i <= n-1; i++)
        {
            
            //
            // Calculate real part
            //
            for(k = 0; k <= m-1; k++)
            {
                work(k) = 0;
            }
            for(k = 0; k <= n-1; k++)
            {
                v = q(i,k).x;
                ap::vadd(&work(0), &t(k, 0), ap::vlen(0,m-1), v);
            }
            for(k = 0; k <= m-1; k++)
            {
                z(i,k).x = work(k);
            }
            
            //
            // Calculate imaginary part
            //
            for(k = 0; k <= m-1; k++)
            {
                work(k) = 0;
            }
            for(k = 0; k <= n-1; k++)
            {
                v = q(i,k).y;
                ap::vadd(&work(0), &t(k, 0), ap::vlen(0,m-1), v);
            }
            for(k = 0; k <= m-1; k++)
            {
                z(i,k).y = work(k);
            }
        }
    }
    return result;
}


bool hermitianeigenvaluesandvectorsininterval(ap::complex_2d_array a,
     int n,
     int zneeded,
     bool isupper,
     double b1,
     double b2,
     int& m,
     ap::real_1d_array& w,
     ap::complex_2d_array& z)
{
    bool result;
    ap::complex_2d_array q;
    ap::real_2d_array t;
    ap::complex_1d_array tau;
    ap::real_1d_array e;
    ap::real_1d_array work;
    int i;
    int k;
    double v;

    ap::ap_error::make_assertion(zneeded==0||zneeded==1, "HermitianEigenValuesAndVectorsInInterval: incorrect ZNeeded");
    
    //
    // Reduce to tridiagonal form
    //
    hermitiantotridiagonal(a, n, isupper, tau, w, e);
    if( zneeded==1 )
    {
        unpackqfromhermitiantridiagonal(a, n, isupper, tau, q);
        zneeded = 2;
    }
    
    //
    // Bisection and inverse iteration
    //
    result = tridiagonaleigenvaluesandvectorsininterval(w, e, n, zneeded, b1, b2, m, t);
    
    //
    // Eigenvectors are needed
    // Calculate Z = Q*T = Re(Q)*T + i*Im(Q)*T
    //
    if( result&&zneeded!=0&&m!=0 )
    {
        work.setbounds(1, m);
        z.setbounds(1, n, 1, m);
        for(i = 1; i <= n; i++)
        {
            
            //
            // Calculate real part
            //
            for(k = 1; k <= m; k++)
            {
                work(k) = 0;
            }
            for(k = 1; k <= n; k++)
            {
                v = q(i,k).x;
                ap::vadd(&work(1), &t(k, 1), ap::vlen(1,m), v);
            }
            for(k = 1; k <= m; k++)
            {
                z(i,k).x = work(k);
            }
            
            //
            // Calculate imaginary part
            //
            for(k = 1; k <= m; k++)
            {
                work(k) = 0;
            }
            for(k = 1; k <= n; k++)
            {
                v = q(i,k).y;
                ap::vadd(&work(1), &t(k, 1), ap::vlen(1,m), v);
            }
            for(k = 1; k <= m; k++)
            {
                z(i,k).y = work(k);
            }
        }
    }
    return result;
}


bool hermitianeigenvaluesandvectorsbyindexes(ap::complex_2d_array a,
     int n,
     int zneeded,
     bool isupper,
     int i1,
     int i2,
     ap::real_1d_array& w,
     ap::complex_2d_array& z)
{
    bool result;
    ap::complex_2d_array q;
    ap::real_2d_array t;
    ap::complex_1d_array tau;
    ap::real_1d_array e;
    ap::real_1d_array work;
    int i;
    int k;
    double v;
    int m;

    ap::ap_error::make_assertion(zneeded==0||zneeded==1, "HermitianEigenValuesAndVectorsByIndexes: incorrect ZNeeded");
    
    //
    // Reduce to tridiagonal form
    //
    hermitiantotridiagonal(a, n, isupper, tau, w, e);
    if( zneeded==1 )
    {
        unpackqfromhermitiantridiagonal(a, n, isupper, tau, q);
        zneeded = 2;
    }
    
    //
    // Bisection and inverse iteration
    //
    result = tridiagonaleigenvaluesandvectorsbyindexes(w, e, n, zneeded, i1, i2, t);
    
    //
    // Eigenvectors are needed
    // Calculate Z = Q*T = Re(Q)*T + i*Im(Q)*T
    //
    m = i2-i1+1;
    if( result&&zneeded!=0 )
    {
        work.setbounds(1, m);
        z.setbounds(1, n, 1, m);
        for(i = 1; i <= n; i++)
        {
            
            //
            // Calculate real part
            //
            for(k = 1; k <= m; k++)
            {
                work(k) = 0;
            }
            for(k = 1; k <= n; k++)
            {
                v = q(i,k).x;
                ap::vadd(&work(1), &t(k, 1), ap::vlen(1,m), v);
            }
            for(k = 1; k <= m; k++)
            {
                z(i,k).x = work(k);
            }
            
            //
            // Calculate imaginary part
            //
            for(k = 1; k <= m; k++)
            {
                work(k) = 0;
            }
            for(k = 1; k <= n; k++)
            {
                v = q(i,k).y;
                ap::vadd(&work(1), &t(k, 1), ap::vlen(1,m), v);
            }
            for(k = 1; k <= m; k++)
            {
                z(i,k).y = work(k);
            }
        }
    }
    return result;
}




