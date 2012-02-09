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

#include <stdafx.h>
#include "tdbisinv.h"

static void tdininternaldlagtf(const int& n,
     ap::real_1d_array& a,
     const double& lambda,
     ap::real_1d_array& b,
     ap::real_1d_array& c,
     const double& tol,
     ap::real_1d_array& d,
     ap::integer_1d_array& iin,
     int& info);
static void tdininternaldlagts(const int& n,
     const ap::real_1d_array& a,
     const ap::real_1d_array& b,
     const ap::real_1d_array& c,
     const ap::real_1d_array& d,
     const ap::integer_1d_array& iin,
     ap::real_1d_array& y,
     double& tol,
     int& info);
static void internaldlaebz(const int& ijob,
     const int& nitmax,
     const int& n,
     const int& mmax,
     const int& minp,
     const double& abstol,
     const double& reltol,
     const double& pivmin,
     const ap::real_1d_array& d,
     const ap::real_1d_array& e,
     const ap::real_1d_array& e2,
     ap::integer_1d_array& nval,
     ap::real_2d_array& ab,
     ap::real_1d_array& c,
     int& mout,
     ap::integer_2d_array& nab,
     ap::real_1d_array& work,
     ap::integer_1d_array& iwork,
     int& info);

/*************************************************************************
Subroutine for finding the tridiagonal matrix eigenvalues/vectors in a
given half-interval (A, B] by using bisection and inverse iteration.

Input parameters:
    D       -   the main diagonal of a tridiagonal matrix.
                Array whose index ranges within [0..N-1].
    E       -   the secondary diagonal of a tridiagonal matrix.
                Array whose index ranges within [0..N-2].
    N       -   size of matrix, N>=0.
    ZNeeded -   flag controlling whether the eigenvectors are needed or not.
                If ZNeeded is equal to:
                 * 0, the eigenvectors are not needed;
                 * 1, the eigenvectors of a tridiagonal matrix are multiplied
                   by the square matrix Z. It is used if the tridiagonal
                   matrix is obtained by the similarity transformation
                   of a symmetric matrix.
                 * 2, the eigenvectors of a tridiagonal matrix replace matrix Z.
    A, B    -   half-interval (A, B] to search eigenvalues in.
    Z       -   if ZNeeded is equal to:
                 * 0, Z isn't used and remains unchanged;
                 * 1, Z contains the square matrix (array whose indexes range
                   within [0..N-1, 0..N-1]) which reduces the given symmetric
                   matrix to tridiagonal form;
                 * 2, Z isn't used (but changed on the exit).

Output parameters:
    D       -   array of the eigenvalues found.
                Array whose index ranges within [0..M-1].
    M       -   number of eigenvalues found in the given half-interval (M>=0).
    Z       -   if ZNeeded is equal to:
                 * 0, doesn't contain any information;
                 * 1, contains the product of a given NxN matrix Z (from the
                   left) and NxM matrix of the eigenvectors found (from the
                   right). Array whose indexes range within [0..N-1, 0..M-1].
                 * 2, contains the matrix of the eigenvectors found.
                   Array whose indexes range within [0..N-1, 0..M-1].

Result:

    True, if successful. In that case, M contains the number of eigenvalues
    in the given half-interval (could be equal to 0), D contains the eigenvalues,
    Z contains the eigenvectors (if needed).
    It should be noted that the subroutine changes the size of arrays D and Z.

    False, if the bisection method subroutine wasn't able to find the
    eigenvalues in the given interval or if the inverse iteration subroutine
    wasn't able to find all the corresponding eigenvectors. In that case,
    the eigenvalues and eigenvectors are not returned, M is equal to 0.

  -- ALGLIB --
     Copyright 31.03.2008 by Bochkanov Sergey
*************************************************************************/
bool smatrixtdevdr(ap::real_1d_array& d,
     const ap::real_1d_array& e,
     int n,
     int zneeded,
     double a,
     double b,
     int& m,
     ap::real_2d_array& z)
{
    bool result;
    int errorcode;
    int nsplit;
    int i;
    int j;
    int k;
    int cr;
    ap::integer_1d_array iblock;
    ap::integer_1d_array isplit;
    ap::integer_1d_array ifail;
    ap::real_1d_array d1;
    ap::real_1d_array e1;
    ap::real_1d_array w;
    ap::real_2d_array z2;
    ap::real_2d_array z3;
    double v;

    ap::ap_error::make_assertion(zneeded>=0&&zneeded<=2, "SMatrixTDEVDR: incorrect ZNeeded!");
    
    //
    // Special cases
    //
    if( ap::fp_less_eq(b,a) )
    {
        m = 0;
        result = true;
        return result;
    }
    if( n<=0 )
    {
        m = 0;
        result = true;
        return result;
    }
    
    //
    // Copy D,E to D1, E1
    //
    d1.setbounds(1, n);
    ap::vmove(&d1(1), &d(0), ap::vlen(1,n));
    if( n>1 )
    {
        e1.setbounds(1, n-1);
        ap::vmove(&e1(1), &e(0), ap::vlen(1,n-1));
    }
    
    //
    // No eigen vectors
    //
    if( zneeded==0 )
    {
        result = internalbisectioneigenvalues(d1, e1, n, 2, 1, a, b, 0, 0, double(-1), w, m, nsplit, iblock, isplit, errorcode);
        if( !result||m==0 )
        {
            m = 0;
            return result;
        }
        d.setbounds(0, m-1);
        ap::vmove(&d(0), &w(1), ap::vlen(0,m-1));
        return result;
    }
    
    //
    // Eigen vectors are multiplied by Z
    //
    if( zneeded==1 )
    {
        
        //
        // Find eigen pairs
        //
        result = internalbisectioneigenvalues(d1, e1, n, 2, 2, a, b, 0, 0, double(-1), w, m, nsplit, iblock, isplit, errorcode);
        if( !result||m==0 )
        {
            m = 0;
            return result;
        }
        internaldstein(n, d1, e1, m, w, iblock, isplit, z2, ifail, cr);
        if( cr!=0 )
        {
            m = 0;
            result = false;
            return result;
        }
        
        //
        // Sort eigen values and vectors
        //
        for(i = 1; i <= m; i++)
        {
            k = i;
            for(j = i; j <= m; j++)
            {
                if( ap::fp_less(w(j),w(k)) )
                {
                    k = j;
                }
            }
            v = w(i);
            w(i) = w(k);
            w(k) = v;
            for(j = 1; j <= n; j++)
            {
                v = z2(j,i);
                z2(j,i) = z2(j,k);
                z2(j,k) = v;
            }
        }
        
        //
        // Transform Z2 and overwrite Z
        //
        z3.setbounds(1, m, 1, n);
        for(i = 1; i <= m; i++)
        {
            ap::vmove(z3.getrow(i, 1, n), z2.getcolumn(i, 1, n));
        }
        for(i = 1; i <= n; i++)
        {
            for(j = 1; j <= m; j++)
            {
                v = ap::vdotproduct(&z(i-1, 0), &z3(j, 1), ap::vlen(0,n-1));
                z2(i,j) = v;
            }
        }
        z.setbounds(0, n-1, 0, m-1);
        for(i = 1; i <= m; i++)
        {
            ap::vmove(z.getcolumn(i-1, 0, n-1), z2.getcolumn(i, 1, n));
        }
        
        //
        // Store W
        //
        d.setbounds(0, m-1);
        for(i = 1; i <= m; i++)
        {
            d(i-1) = w(i);
        }
        return result;
    }
    
    //
    // Eigen vectors are stored in Z
    //
    if( zneeded==2 )
    {
        
        //
        // Find eigen pairs
        //
        result = internalbisectioneigenvalues(d1, e1, n, 2, 2, a, b, 0, 0, double(-1), w, m, nsplit, iblock, isplit, errorcode);
        if( !result||m==0 )
        {
            m = 0;
            return result;
        }
        internaldstein(n, d1, e1, m, w, iblock, isplit, z2, ifail, cr);
        if( cr!=0 )
        {
            m = 0;
            result = false;
            return result;
        }
        
        //
        // Sort eigen values and vectors
        //
        for(i = 1; i <= m; i++)
        {
            k = i;
            for(j = i; j <= m; j++)
            {
                if( ap::fp_less(w(j),w(k)) )
                {
                    k = j;
                }
            }
            v = w(i);
            w(i) = w(k);
            w(k) = v;
            for(j = 1; j <= n; j++)
            {
                v = z2(j,i);
                z2(j,i) = z2(j,k);
                z2(j,k) = v;
            }
        }
        
        //
        // Store W
        //
        d.setbounds(0, m-1);
        for(i = 1; i <= m; i++)
        {
            d(i-1) = w(i);
        }
        z.setbounds(0, n-1, 0, m-1);
        for(i = 1; i <= m; i++)
        {
            ap::vmove(z.getcolumn(i-1, 0, n-1), z2.getcolumn(i, 1, n));
        }
        return result;
    }
    result = false;
    return result;
}


/*************************************************************************
Subroutine for finding tridiagonal matrix eigenvalues/vectors with given
indexes (in ascending order) by using the bisection and inverse iteraion.

Input parameters:
    D       -   the main diagonal of a tridiagonal matrix.
                Array whose index ranges within [0..N-1].
    E       -   the secondary diagonal of a tridiagonal matrix.
                Array whose index ranges within [0..N-2].
    N       -   size of matrix. N>=0.
    ZNeeded -   flag controlling whether the eigenvectors are needed or not.
                If ZNeeded is equal to:
                 * 0, the eigenvectors are not needed;
                 * 1, the eigenvectors of a tridiagonal matrix are multiplied
                   by the square matrix Z. It is used if the
                   tridiagonal matrix is obtained by the similarity transformation
                   of a symmetric matrix.
                 * 2, the eigenvectors of a tridiagonal matrix replace
                   matrix Z.
    I1, I2  -   index interval for searching (from I1 to I2).
                0 <= I1 <= I2 <= N-1.
    Z       -   if ZNeeded is equal to:
                 * 0, Z isn't used and remains unchanged;
                 * 1, Z contains the square matrix (array whose indexes range within [0..N-1, 0..N-1])
                   which reduces the given symmetric matrix to  tridiagonal form;
                 * 2, Z isn't used (but changed on the exit).

Output parameters:
    D       -   array of the eigenvalues found.
                Array whose index ranges within [0..I2-I1].
    Z       -   if ZNeeded is equal to:
                 * 0, doesn't contain any information;
                 * 1, contains the product of a given NxN matrix Z (from the left) and
                   Nx(I2-I1) matrix of the eigenvectors found (from the right).
                   Array whose indexes range within [0..N-1, 0..I2-I1].
                 * 2, contains the matrix of the eigenvalues found.
                   Array whose indexes range within [0..N-1, 0..I2-I1].


Result:

    True, if successful. In that case, D contains the eigenvalues,
    Z contains the eigenvectors (if needed).
    It should be noted that the subroutine changes the size of arrays D and Z.

    False, if the bisection method subroutine wasn't able to find the eigenvalues
    in the given interval or if the inverse iteration subroutine wasn't able
    to find all the corresponding eigenvectors. In that case, the eigenvalues
    and eigenvectors are not returned.

  -- ALGLIB --
     Copyright 25.12.2005 by Bochkanov Sergey
*************************************************************************/
bool smatrixtdevdi(ap::real_1d_array& d,
     const ap::real_1d_array& e,
     int n,
     int zneeded,
     int i1,
     int i2,
     ap::real_2d_array& z)
{
    bool result;
    int errorcode;
    int nsplit;
    int i;
    int j;
    int k;
    int m;
    int cr;
    ap::integer_1d_array iblock;
    ap::integer_1d_array isplit;
    ap::integer_1d_array ifail;
    ap::real_1d_array w;
    ap::real_1d_array d1;
    ap::real_1d_array e1;
    ap::real_2d_array z2;
    ap::real_2d_array z3;
    double v;

    ap::ap_error::make_assertion(0<=i1&&i1<=i2&&i2<n, "SMatrixTDEVDI: incorrect I1/I2!");
    
    //
    // Copy D,E to D1, E1
    //
    d1.setbounds(1, n);
    ap::vmove(&d1(1), &d(0), ap::vlen(1,n));
    if( n>1 )
    {
        e1.setbounds(1, n-1);
        ap::vmove(&e1(1), &e(0), ap::vlen(1,n-1));
    }
    
    //
    // No eigen vectors
    //
    if( zneeded==0 )
    {
        result = internalbisectioneigenvalues(d1, e1, n, 3, 1, double(0), double(0), i1+1, i2+1, double(-1), w, m, nsplit, iblock, isplit, errorcode);
        if( !result )
        {
            return result;
        }
        if( m!=i2-i1+1 )
        {
            result = false;
            return result;
        }
        d.setbounds(0, m-1);
        for(i = 1; i <= m; i++)
        {
            d(i-1) = w(i);
        }
        return result;
    }
    
    //
    // Eigen vectors are multiplied by Z
    //
    if( zneeded==1 )
    {
        
        //
        // Find eigen pairs
        //
        result = internalbisectioneigenvalues(d1, e1, n, 3, 2, double(0), double(0), i1+1, i2+1, double(-1), w, m, nsplit, iblock, isplit, errorcode);
        if( !result )
        {
            return result;
        }
        if( m!=i2-i1+1 )
        {
            result = false;
            return result;
        }
        internaldstein(n, d1, e1, m, w, iblock, isplit, z2, ifail, cr);
        if( cr!=0 )
        {
            result = false;
            return result;
        }
        
        //
        // Sort eigen values and vectors
        //
        for(i = 1; i <= m; i++)
        {
            k = i;
            for(j = i; j <= m; j++)
            {
                if( ap::fp_less(w(j),w(k)) )
                {
                    k = j;
                }
            }
            v = w(i);
            w(i) = w(k);
            w(k) = v;
            for(j = 1; j <= n; j++)
            {
                v = z2(j,i);
                z2(j,i) = z2(j,k);
                z2(j,k) = v;
            }
        }
        
        //
        // Transform Z2 and overwrite Z
        //
        z3.setbounds(1, m, 1, n);
        for(i = 1; i <= m; i++)
        {
            ap::vmove(z3.getrow(i, 1, n), z2.getcolumn(i, 1, n));
        }
        for(i = 1; i <= n; i++)
        {
            for(j = 1; j <= m; j++)
            {
                v = ap::vdotproduct(&z(i-1, 0), &z3(j, 1), ap::vlen(0,n-1));
                z2(i,j) = v;
            }
        }
        z.setbounds(0, n-1, 0, m-1);
        for(i = 1; i <= m; i++)
        {
            ap::vmove(z.getcolumn(i-1, 0, n-1), z2.getcolumn(i, 1, n));
        }
        
        //
        // Store W
        //
        d.setbounds(0, m-1);
        for(i = 1; i <= m; i++)
        {
            d(i-1) = w(i);
        }
        return result;
    }
    
    //
    // Eigen vectors are stored in Z
    //
    if( zneeded==2 )
    {
        
        //
        // Find eigen pairs
        //
        result = internalbisectioneigenvalues(d1, e1, n, 3, 2, double(0), double(0), i1+1, i2+1, double(-1), w, m, nsplit, iblock, isplit, errorcode);
        if( !result )
        {
            return result;
        }
        if( m!=i2-i1+1 )
        {
            result = false;
            return result;
        }
        internaldstein(n, d1, e1, m, w, iblock, isplit, z2, ifail, cr);
        if( cr!=0 )
        {
            result = false;
            return result;
        }
        
        //
        // Sort eigen values and vectors
        //
        for(i = 1; i <= m; i++)
        {
            k = i;
            for(j = i; j <= m; j++)
            {
                if( ap::fp_less(w(j),w(k)) )
                {
                    k = j;
                }
            }
            v = w(i);
            w(i) = w(k);
            w(k) = v;
            for(j = 1; j <= n; j++)
            {
                v = z2(j,i);
                z2(j,i) = z2(j,k);
                z2(j,k) = v;
            }
        }
        
        //
        // Store Z
        //
        z.setbounds(0, n-1, 0, m-1);
        for(i = 1; i <= m; i++)
        {
            ap::vmove(z.getcolumn(i-1, 0, n-1), z2.getcolumn(i, 1, n));
        }
        
        //
        // Store W
        //
        d.setbounds(0, m-1);
        for(i = 1; i <= m; i++)
        {
            d(i-1) = w(i);
        }
        return result;
    }
    result = false;
    return result;
}


bool tridiagonaleigenvaluesandvectorsininterval(ap::real_1d_array& d,
     const ap::real_1d_array& e,
     int n,
     int zneeded,
     double a,
     double b,
     int& m,
     ap::real_2d_array& z)
{
    bool result;
    int errorcode;
    int nsplit;
    int i;
    int j;
    int k;
    int cr;
    ap::integer_1d_array iblock;
    ap::integer_1d_array isplit;
    ap::integer_1d_array ifail;
    ap::real_1d_array w;
    ap::real_2d_array z2;
    ap::real_2d_array z3;
    double v;

    
    //
    // No eigen vectors
    //
    if( zneeded==0 )
    {
        result = internalbisectioneigenvalues(d, e, n, 2, 1, a, b, 0, 0, double(-1), w, m, nsplit, iblock, isplit, errorcode);
        if( !result||m==0 )
        {
            m = 0;
            return result;
        }
        d.setbounds(1, m);
        for(i = 1; i <= m; i++)
        {
            d(i) = w(i);
        }
        return result;
    }
    
    //
    // Eigen vectors are multiplied by Z
    //
    if( zneeded==1 )
    {
        
        //
        // Find eigen pairs
        //
        result = internalbisectioneigenvalues(d, e, n, 2, 2, a, b, 0, 0, double(-1), w, m, nsplit, iblock, isplit, errorcode);
        if( !result||m==0 )
        {
            m = 0;
            return result;
        }
        internaldstein(n, d, e, m, w, iblock, isplit, z2, ifail, cr);
        if( cr!=0 )
        {
            m = 0;
            result = false;
            return result;
        }
        
        //
        // Sort eigen values and vectors
        //
        for(i = 1; i <= m; i++)
        {
            k = i;
            for(j = i; j <= m; j++)
            {
                if( ap::fp_less(w(j),w(k)) )
                {
                    k = j;
                }
            }
            v = w(i);
            w(i) = w(k);
            w(k) = v;
            for(j = 1; j <= n; j++)
            {
                v = z2(j,i);
                z2(j,i) = z2(j,k);
                z2(j,k) = v;
            }
        }
        
        //
        // Transform Z2 and overwrite Z
        //
        z3.setbounds(1, m, 1, n);
        for(i = 1; i <= m; i++)
        {
            ap::vmove(z3.getrow(i, 1, n), z2.getcolumn(i, 1, n));
        }
        for(i = 1; i <= n; i++)
        {
            for(j = 1; j <= m; j++)
            {
                v = ap::vdotproduct(&z(i, 1), &z3(j, 1), ap::vlen(1,n));
                z2(i,j) = v;
            }
        }
        z.setbounds(1, n, 1, m);
        for(i = 1; i <= m; i++)
        {
            ap::vmove(z.getcolumn(i, 1, n), z2.getcolumn(i, 1, n));
        }
        
        //
        // Store W
        //
        d.setbounds(1, m);
        for(i = 1; i <= m; i++)
        {
            d(i) = w(i);
        }
        return result;
    }
    
    //
    // Eigen vectors are stored in Z
    //
    if( zneeded==2 )
    {
        
        //
        // Find eigen pairs
        //
        result = internalbisectioneigenvalues(d, e, n, 2, 2, a, b, 0, 0, double(-1), w, m, nsplit, iblock, isplit, errorcode);
        if( !result||m==0 )
        {
            m = 0;
            return result;
        }
        internaldstein(n, d, e, m, w, iblock, isplit, z, ifail, cr);
        if( cr!=0 )
        {
            m = 0;
            result = false;
            return result;
        }
        
        //
        // Sort eigen values and vectors
        //
        for(i = 1; i <= m; i++)
        {
            k = i;
            for(j = i; j <= m; j++)
            {
                if( ap::fp_less(w(j),w(k)) )
                {
                    k = j;
                }
            }
            v = w(i);
            w(i) = w(k);
            w(k) = v;
            for(j = 1; j <= n; j++)
            {
                v = z(j,i);
                z(j,i) = z(j,k);
                z(j,k) = v;
            }
        }
        
        //
        // Store W
        //
        d.setbounds(1, m);
        for(i = 1; i <= m; i++)
        {
            d(i) = w(i);
        }
        return result;
    }
    result = false;
    return result;
}


bool tridiagonaleigenvaluesandvectorsbyindexes(ap::real_1d_array& d,
     const ap::real_1d_array& e,
     int n,
     int zneeded,
     int i1,
     int i2,
     ap::real_2d_array& z)
{
    bool result;
    int errorcode;
    int nsplit;
    int i;
    int j;
    int k;
    int m;
    int cr;
    ap::integer_1d_array iblock;
    ap::integer_1d_array isplit;
    ap::integer_1d_array ifail;
    ap::real_1d_array w;
    ap::real_2d_array z2;
    ap::real_2d_array z3;
    double v;

    
    //
    // No eigen vectors
    //
    if( zneeded==0 )
    {
        result = internalbisectioneigenvalues(d, e, n, 3, 1, double(0), double(0), i1, i2, double(-1), w, m, nsplit, iblock, isplit, errorcode);
        if( !result )
        {
            return result;
        }
        d.setbounds(1, m);
        for(i = 1; i <= m; i++)
        {
            d(i) = w(i);
        }
        return result;
    }
    
    //
    // Eigen vectors are multiplied by Z
    //
    if( zneeded==1 )
    {
        
        //
        // Find eigen pairs
        //
        result = internalbisectioneigenvalues(d, e, n, 3, 2, double(0), double(0), i1, i2, double(-1), w, m, nsplit, iblock, isplit, errorcode);
        if( !result )
        {
            return result;
        }
        internaldstein(n, d, e, m, w, iblock, isplit, z2, ifail, cr);
        if( cr!=0 )
        {
            result = false;
            return result;
        }
        
        //
        // Sort eigen values and vectors
        //
        for(i = 1; i <= m; i++)
        {
            k = i;
            for(j = i; j <= m; j++)
            {
                if( ap::fp_less(w(j),w(k)) )
                {
                    k = j;
                }
            }
            v = w(i);
            w(i) = w(k);
            w(k) = v;
            for(j = 1; j <= n; j++)
            {
                v = z2(j,i);
                z2(j,i) = z2(j,k);
                z2(j,k) = v;
            }
        }
        
        //
        // Transform Z2 and overwrite Z
        //
        z3.setbounds(1, m, 1, n);
        for(i = 1; i <= m; i++)
        {
            ap::vmove(z3.getrow(i, 1, n), z2.getcolumn(i, 1, n));
        }
        for(i = 1; i <= n; i++)
        {
            for(j = 1; j <= m; j++)
            {
                v = ap::vdotproduct(&z(i, 1), &z3(j, 1), ap::vlen(1,n));
                z2(i,j) = v;
            }
        }
        z.setbounds(1, n, 1, m);
        for(i = 1; i <= m; i++)
        {
            ap::vmove(z.getcolumn(i, 1, n), z2.getcolumn(i, 1, n));
        }
        
        //
        // Store W
        //
        d.setbounds(1, m);
        for(i = 1; i <= m; i++)
        {
            d(i) = w(i);
        }
        return result;
    }
    
    //
    // Eigen vectors are stored in Z
    //
    if( zneeded==2 )
    {
        
        //
        // Find eigen pairs
        //
        result = internalbisectioneigenvalues(d, e, n, 3, 2, double(0), double(0), i1, i2, double(-1), w, m, nsplit, iblock, isplit, errorcode);
        if( !result )
        {
            return result;
        }
        internaldstein(n, d, e, m, w, iblock, isplit, z, ifail, cr);
        if( cr!=0 )
        {
            result = false;
            return result;
        }
        
        //
        // Sort eigen values and vectors
        //
        for(i = 1; i <= m; i++)
        {
            k = i;
            for(j = i; j <= m; j++)
            {
                if( ap::fp_less(w(j),w(k)) )
                {
                    k = j;
                }
            }
            v = w(i);
            w(i) = w(k);
            w(k) = v;
            for(j = 1; j <= n; j++)
            {
                v = z(j,i);
                z(j,i) = z(j,k);
                z(j,k) = v;
            }
        }
        
        //
        // Store W
        //
        d.setbounds(1, m);
        for(i = 1; i <= m; i++)
        {
            d(i) = w(i);
        }
        return result;
    }
    result = false;
    return result;
}


bool internalbisectioneigenvalues(ap::real_1d_array d,
     ap::real_1d_array e,
     int n,
     int irange,
     int iorder,
     double vl,
     double vu,
     int il,
     int iu,
     double abstol,
     ap::real_1d_array& w,
     int& m,
     int& nsplit,
     ap::integer_1d_array& iblock,
     ap::integer_1d_array& isplit,
     int& errorcode)
{
    bool result;
    double fudge;
    double relfac;
    bool ncnvrg;
    bool toofew;
    int ib;
    int ibegin;
    int idiscl;
    int idiscu;
    int ie;
    int iend;
    int iinfo;
    int im;
    int iin;
    int ioff;
    int iout;
    int itmax;
    int iw;
    int iwoff;
    int j;
    int itmp1;
    int jb;
    int jdisc;
    int je;
    int nwl;
    int nwu;
    double atoli;
    double bnorm;
    double gl;
    double gu;
    double pivmin;
    double rtoli;
    double safemn;
    double tmp1;
    double tmp2;
    double tnorm;
    double ulp;
    double wkill;
    double wl;
    double wlu;
    double wu;
    double wul;
    double scalefactor;
    double t;
    ap::integer_1d_array idumma;
    ap::real_1d_array work;
    ap::integer_1d_array iwork;
    ap::integer_1d_array ia1s2;
    ap::real_1d_array ra1s2;
    ap::real_2d_array ra1s2x2;
    ap::integer_2d_array ia1s2x2;
    ap::real_1d_array ra1siin;
    ap::real_1d_array ra2siin;
    ap::real_1d_array ra3siin;
    ap::real_1d_array ra4siin;
    ap::real_2d_array ra1siinx2;
    ap::integer_2d_array ia1siinx2;
    ap::integer_1d_array iworkspace;
    ap::real_1d_array rworkspace;
    int tmpi;

    
    //
    // Quick return if possible
    //
    m = 0;
    if( n==0 )
    {
        result = true;
        return result;
    }
    
    //
    // Get machine constants
    // NB is the minimum vector length for vector bisection, or 0
    // if only scalar is to be done.
    //
    fudge = 2;
    relfac = 2;
    safemn = ap::minrealnumber;
    ulp = 2*ap::machineepsilon;
    rtoli = ulp*relfac;
    idumma.setbounds(1, 1);
    work.setbounds(1, 4*n);
    iwork.setbounds(1, 3*n);
    w.setbounds(1, n);
    iblock.setbounds(1, n);
    isplit.setbounds(1, n);
    ia1s2.setbounds(1, 2);
    ra1s2.setbounds(1, 2);
    ra1s2x2.setbounds(1, 2, 1, 2);
    ia1s2x2.setbounds(1, 2, 1, 2);
    ra1siin.setbounds(1, n);
    ra2siin.setbounds(1, n);
    ra3siin.setbounds(1, n);
    ra4siin.setbounds(1, n);
    ra1siinx2.setbounds(1, n, 1, 2);
    ia1siinx2.setbounds(1, n, 1, 2);
    iworkspace.setbounds(1, n);
    rworkspace.setbounds(1, n);
    
    //
    // Check for Errors
    //
    result = false;
    errorcode = 0;
    if( irange<=0||irange>=4 )
    {
        errorcode = -4;
    }
    if( iorder<=0||iorder>=3 )
    {
        errorcode = -5;
    }
    if( n<0 )
    {
        errorcode = -3;
    }
    if( irange==2&&ap::fp_greater_eq(vl,vu) )
    {
        errorcode = -6;
    }
    if( irange==3&&(il<1||il>ap::maxint(1, n)) )
    {
        errorcode = -8;
    }
    if( irange==3&&(iu<ap::minint(n, il)||iu>n) )
    {
        errorcode = -9;
    }
    if( errorcode!=0 )
    {
        return result;
    }
    
    //
    // Initialize error flags
    //
    ncnvrg = false;
    toofew = false;
    
    //
    // Simplifications:
    //
    if( irange==3&&il==1&&iu==n )
    {
        irange = 1;
    }
    
    //
    // Special Case when N=1
    //
    if( n==1 )
    {
        nsplit = 1;
        isplit(1) = 1;
        if( irange==2&&(ap::fp_greater_eq(vl,d(1))||ap::fp_less(vu,d(1))) )
        {
            m = 0;
        }
        else
        {
            w(1) = d(1);
            iblock(1) = 1;
            m = 1;
        }
        result = true;
        return result;
    }
    
    //
    // Scaling
    //
    t = fabs(d(n));
    for(j = 1; j <= n-1; j++)
    {
        t = ap::maxreal(t, fabs(d(j)));
        t = ap::maxreal(t, fabs(e(j)));
    }
    scalefactor = 1;
    if( ap::fp_neq(t,0) )
    {
        if( ap::fp_greater(t,sqrt(sqrt(ap::minrealnumber))*sqrt(ap::maxrealnumber)) )
        {
            scalefactor = t;
        }
        if( ap::fp_less(t,sqrt(sqrt(ap::maxrealnumber))*sqrt(ap::minrealnumber)) )
        {
            scalefactor = t;
        }
        for(j = 1; j <= n-1; j++)
        {
            d(j) = d(j)/scalefactor;
            e(j) = e(j)/scalefactor;
        }
        d(n) = d(n)/scalefactor;
    }
    
    //
    // Compute Splitting Points
    //
    nsplit = 1;
    work(n) = 0;
    pivmin = 1;
    for(j = 2; j <= n; j++)
    {
        tmp1 = ap::sqr(e(j-1));
        if( ap::fp_greater(fabs(d(j)*d(j-1))*ap::sqr(ulp)+safemn,tmp1) )
        {
            isplit(nsplit) = j-1;
            nsplit = nsplit+1;
            work(j-1) = 0;
        }
        else
        {
            work(j-1) = tmp1;
            pivmin = ap::maxreal(pivmin, tmp1);
        }
    }
    isplit(nsplit) = n;
    pivmin = pivmin*safemn;
    
    //
    // Compute Interval and ATOLI
    //
    if( irange==3 )
    {
        
        //
        // RANGE='I': Compute the interval containing eigenvalues
        //     IL through IU.
        //
        // Compute Gershgorin interval for entire (split) matrix
        // and use it as the initial interval
        //
        gu = d(1);
        gl = d(1);
        tmp1 = 0;
        for(j = 1; j <= n-1; j++)
        {
            tmp2 = sqrt(work(j));
            gu = ap::maxreal(gu, d(j)+tmp1+tmp2);
            gl = ap::minreal(gl, d(j)-tmp1-tmp2);
            tmp1 = tmp2;
        }
        gu = ap::maxreal(gu, d(n)+tmp1);
        gl = ap::minreal(gl, d(n)-tmp1);
        tnorm = ap::maxreal(fabs(gl), fabs(gu));
        gl = gl-fudge*tnorm*ulp*n-fudge*2*pivmin;
        gu = gu+fudge*tnorm*ulp*n+fudge*pivmin;
        
        //
        // Compute Iteration parameters
        //
        itmax = ap::iceil((log(tnorm+pivmin)-log(pivmin))/log(double(2)))+2;
        if( ap::fp_less_eq(abstol,0) )
        {
            atoli = ulp*tnorm;
        }
        else
        {
            atoli = abstol;
        }
        work(n+1) = gl;
        work(n+2) = gl;
        work(n+3) = gu;
        work(n+4) = gu;
        work(n+5) = gl;
        work(n+6) = gu;
        iwork(1) = -1;
        iwork(2) = -1;
        iwork(3) = n+1;
        iwork(4) = n+1;
        iwork(5) = il-1;
        iwork(6) = iu;
        
        //
        // Calling DLAEBZ
        //
        // DLAEBZ( 3, ITMAX, N, 2, 2, NB, ATOLI, RTOLI, PIVMIN, D, E,
        //    WORK, IWORK( 5 ), WORK( N+1 ), WORK( N+5 ), IOUT,
        //    IWORK, W, IBLOCK, IINFO )
        //
        ia1s2(1) = iwork(5);
        ia1s2(2) = iwork(6);
        ra1s2(1) = work(n+5);
        ra1s2(2) = work(n+6);
        ra1s2x2(1,1) = work(n+1);
        ra1s2x2(2,1) = work(n+2);
        ra1s2x2(1,2) = work(n+3);
        ra1s2x2(2,2) = work(n+4);
        ia1s2x2(1,1) = iwork(1);
        ia1s2x2(2,1) = iwork(2);
        ia1s2x2(1,2) = iwork(3);
        ia1s2x2(2,2) = iwork(4);
        internaldlaebz(3, itmax, n, 2, 2, atoli, rtoli, pivmin, d, e, work, ia1s2, ra1s2x2, ra1s2, iout, ia1s2x2, w, iblock, iinfo);
        iwork(5) = ia1s2(1);
        iwork(6) = ia1s2(2);
        work(n+5) = ra1s2(1);
        work(n+6) = ra1s2(2);
        work(n+1) = ra1s2x2(1,1);
        work(n+2) = ra1s2x2(2,1);
        work(n+3) = ra1s2x2(1,2);
        work(n+4) = ra1s2x2(2,2);
        iwork(1) = ia1s2x2(1,1);
        iwork(2) = ia1s2x2(2,1);
        iwork(3) = ia1s2x2(1,2);
        iwork(4) = ia1s2x2(2,2);
        if( iwork(6)==iu )
        {
            wl = work(n+1);
            wlu = work(n+3);
            nwl = iwork(1);
            wu = work(n+4);
            wul = work(n+2);
            nwu = iwork(4);
        }
        else
        {
            wl = work(n+2);
            wlu = work(n+4);
            nwl = iwork(2);
            wu = work(n+3);
            wul = work(n+1);
            nwu = iwork(3);
        }
        if( nwl<0||nwl>=n||nwu<1||nwu>n )
        {
            errorcode = 4;
            result = false;
            return result;
        }
    }
    else
    {
        
        //
        // RANGE='A' or 'V' -- Set ATOLI
        //
        tnorm = ap::maxreal(fabs(d(1))+fabs(e(1)), fabs(d(n))+fabs(e(n-1)));
        for(j = 2; j <= n-1; j++)
        {
            tnorm = ap::maxreal(tnorm, fabs(d(j))+fabs(e(j-1))+fabs(e(j)));
        }
        if( ap::fp_less_eq(abstol,0) )
        {
            atoli = ulp*tnorm;
        }
        else
        {
            atoli = abstol;
        }
        if( irange==2 )
        {
            wl = vl;
            wu = vu;
        }
        else
        {
            wl = 0;
            wu = 0;
        }
    }
    
    //
    // Find Eigenvalues -- Loop Over Blocks and recompute NWL and NWU.
    // NWL accumulates the number of eigenvalues .le. WL,
    // NWU accumulates the number of eigenvalues .le. WU
    //
    m = 0;
    iend = 0;
    errorcode = 0;
    nwl = 0;
    nwu = 0;
    for(jb = 1; jb <= nsplit; jb++)
    {
        ioff = iend;
        ibegin = ioff+1;
        iend = isplit(jb);
        iin = iend-ioff;
        if( iin==1 )
        {
            
            //
            // Special Case -- IIN=1
            //
            if( irange==1||ap::fp_greater_eq(wl,d(ibegin)-pivmin) )
            {
                nwl = nwl+1;
            }
            if( irange==1||ap::fp_greater_eq(wu,d(ibegin)-pivmin) )
            {
                nwu = nwu+1;
            }
            if( irange==1||ap::fp_less(wl,d(ibegin)-pivmin)&&ap::fp_greater_eq(wu,d(ibegin)-pivmin) )
            {
                m = m+1;
                w(m) = d(ibegin);
                iblock(m) = jb;
            }
        }
        else
        {
            
            //
            // General Case -- IIN > 1
            //
            // Compute Gershgorin Interval
            // and use it as the initial interval
            //
            gu = d(ibegin);
            gl = d(ibegin);
            tmp1 = 0;
            for(j = ibegin; j <= iend-1; j++)
            {
                tmp2 = fabs(e(j));
                gu = ap::maxreal(gu, d(j)+tmp1+tmp2);
                gl = ap::minreal(gl, d(j)-tmp1-tmp2);
                tmp1 = tmp2;
            }
            gu = ap::maxreal(gu, d(iend)+tmp1);
            gl = ap::minreal(gl, d(iend)-tmp1);
            bnorm = ap::maxreal(fabs(gl), fabs(gu));
            gl = gl-fudge*bnorm*ulp*iin-fudge*pivmin;
            gu = gu+fudge*bnorm*ulp*iin+fudge*pivmin;
            
            //
            // Compute ATOLI for the current submatrix
            //
            if( ap::fp_less_eq(abstol,0) )
            {
                atoli = ulp*ap::maxreal(fabs(gl), fabs(gu));
            }
            else
            {
                atoli = abstol;
            }
            if( irange>1 )
            {
                if( ap::fp_less(gu,wl) )
                {
                    nwl = nwl+iin;
                    nwu = nwu+iin;
                    continue;
                }
                gl = ap::maxreal(gl, wl);
                gu = ap::minreal(gu, wu);
                if( ap::fp_greater_eq(gl,gu) )
                {
                    continue;
                }
            }
            
            //
            // Set Up Initial Interval
            //
            work(n+1) = gl;
            work(n+iin+1) = gu;
            
            //
            // Calling DLAEBZ
            //
            // CALL DLAEBZ( 1, 0, IN, IN, 1, NB, ATOLI, RTOLI, PIVMIN,
            //    D( IBEGIN ), E( IBEGIN ), WORK( IBEGIN ),
            //    IDUMMA, WORK( N+1 ), WORK( N+2*IN+1 ), IM,
            //    IWORK, W( M+1 ), IBLOCK( M+1 ), IINFO )
            //
            for(tmpi = 1; tmpi <= iin; tmpi++)
            {
                ra1siin(tmpi) = d(ibegin-1+tmpi);
                if( ibegin-1+tmpi<n )
                {
                    ra2siin(tmpi) = e(ibegin-1+tmpi);
                }
                ra3siin(tmpi) = work(ibegin-1+tmpi);
                ra1siinx2(tmpi,1) = work(n+tmpi);
                ra1siinx2(tmpi,2) = work(n+tmpi+iin);
                ra4siin(tmpi) = work(n+2*iin+tmpi);
                rworkspace(tmpi) = w(m+tmpi);
                iworkspace(tmpi) = iblock(m+tmpi);
                ia1siinx2(tmpi,1) = iwork(tmpi);
                ia1siinx2(tmpi,2) = iwork(tmpi+iin);
            }
            internaldlaebz(1, 0, iin, iin, 1, atoli, rtoli, pivmin, ra1siin, ra2siin, ra3siin, idumma, ra1siinx2, ra4siin, im, ia1siinx2, rworkspace, iworkspace, iinfo);
            for(tmpi = 1; tmpi <= iin; tmpi++)
            {
                work(n+tmpi) = ra1siinx2(tmpi,1);
                work(n+tmpi+iin) = ra1siinx2(tmpi,2);
                work(n+2*iin+tmpi) = ra4siin(tmpi);
                w(m+tmpi) = rworkspace(tmpi);
                iblock(m+tmpi) = iworkspace(tmpi);
                iwork(tmpi) = ia1siinx2(tmpi,1);
                iwork(tmpi+iin) = ia1siinx2(tmpi,2);
            }
            nwl = nwl+iwork(1);
            nwu = nwu+iwork(iin+1);
            iwoff = m-iwork(1);
            
            //
            // Compute Eigenvalues
            //
            itmax = ap::iceil((log(gu-gl+pivmin)-log(pivmin))/log(double(2)))+2;
            
            //
            // Calling DLAEBZ
            //
            //CALL DLAEBZ( 2, ITMAX, IN, IN, 1, NB, ATOLI, RTOLI, PIVMIN,
            //    D( IBEGIN ), E( IBEGIN ), WORK( IBEGIN ),
            //    IDUMMA, WORK( N+1 ), WORK( N+2*IN+1 ), IOUT,
            //    IWORK, W( M+1 ), IBLOCK( M+1 ), IINFO )
            //
            for(tmpi = 1; tmpi <= iin; tmpi++)
            {
                ra1siin(tmpi) = d(ibegin-1+tmpi);
                if( ibegin-1+tmpi<n )
                {
                    ra2siin(tmpi) = e(ibegin-1+tmpi);
                }
                ra3siin(tmpi) = work(ibegin-1+tmpi);
                ra1siinx2(tmpi,1) = work(n+tmpi);
                ra1siinx2(tmpi,2) = work(n+tmpi+iin);
                ra4siin(tmpi) = work(n+2*iin+tmpi);
                rworkspace(tmpi) = w(m+tmpi);
                iworkspace(tmpi) = iblock(m+tmpi);
                ia1siinx2(tmpi,1) = iwork(tmpi);
                ia1siinx2(tmpi,2) = iwork(tmpi+iin);
            }
            internaldlaebz(2, itmax, iin, iin, 1, atoli, rtoli, pivmin, ra1siin, ra2siin, ra3siin, idumma, ra1siinx2, ra4siin, iout, ia1siinx2, rworkspace, iworkspace, iinfo);
            for(tmpi = 1; tmpi <= iin; tmpi++)
            {
                work(n+tmpi) = ra1siinx2(tmpi,1);
                work(n+tmpi+iin) = ra1siinx2(tmpi,2);
                work(n+2*iin+tmpi) = ra4siin(tmpi);
                w(m+tmpi) = rworkspace(tmpi);
                iblock(m+tmpi) = iworkspace(tmpi);
                iwork(tmpi) = ia1siinx2(tmpi,1);
                iwork(tmpi+iin) = ia1siinx2(tmpi,2);
            }
            
            //
            // Copy Eigenvalues Into W and IBLOCK
            // Use -JB for block number for unconverged eigenvalues.
            //
            for(j = 1; j <= iout; j++)
            {
                tmp1 = 0.5*(work(j+n)+work(j+iin+n));
                
                //
                // Flag non-convergence.
                //
                if( j>iout-iinfo )
                {
                    ncnvrg = true;
                    ib = -jb;
                }
                else
                {
                    ib = jb;
                }
                for(je = iwork(j)+1+iwoff; je <= iwork(j+iin)+iwoff; je++)
                {
                    w(je) = tmp1;
                    iblock(je) = ib;
                }
            }
            m = m+im;
        }
    }
    
    //
    // If RANGE='I', then (WL,WU) contains eigenvalues NWL+1,...,NWU
    // If NWL+1 < IL or NWU > IU, discard extra eigenvalues.
    //
    if( irange==3 )
    {
        im = 0;
        idiscl = il-1-nwl;
        idiscu = nwu-iu;
        if( idiscl>0||idiscu>0 )
        {
            for(je = 1; je <= m; je++)
            {
                if( ap::fp_less_eq(w(je),wlu)&&idiscl>0 )
                {
                    idiscl = idiscl-1;
                }
                else
                {
                    if( ap::fp_greater_eq(w(je),wul)&&idiscu>0 )
                    {
                        idiscu = idiscu-1;
                    }
                    else
                    {
                        im = im+1;
                        w(im) = w(je);
                        iblock(im) = iblock(je);
                    }
                }
            }
            m = im;
        }
        if( idiscl>0||idiscu>0 )
        {
            
            //
            // Code to deal with effects of bad arithmetic:
            // Some low eigenvalues to be discarded are not in (WL,WLU],
            // or high eigenvalues to be discarded are not in (WUL,WU]
            // so just kill off the smallest IDISCL/largest IDISCU
            // eigenvalues, by simply finding the smallest/largest
            // eigenvalue(s).
            //
            // (If N(w) is monotone non-decreasing, this should never
            //  happen.)
            //
            if( idiscl>0 )
            {
                wkill = wu;
                for(jdisc = 1; jdisc <= idiscl; jdisc++)
                {
                    iw = 0;
                    for(je = 1; je <= m; je++)
                    {
                        if( iblock(je)!=0&&(ap::fp_less(w(je),wkill)||iw==0) )
                        {
                            iw = je;
                            wkill = w(je);
                        }
                    }
                    iblock(iw) = 0;
                }
            }
            if( idiscu>0 )
            {
                wkill = wl;
                for(jdisc = 1; jdisc <= idiscu; jdisc++)
                {
                    iw = 0;
                    for(je = 1; je <= m; je++)
                    {
                        if( iblock(je)!=0&&(ap::fp_greater(w(je),wkill)||iw==0) )
                        {
                            iw = je;
                            wkill = w(je);
                        }
                    }
                    iblock(iw) = 0;
                }
            }
            im = 0;
            for(je = 1; je <= m; je++)
            {
                if( iblock(je)!=0 )
                {
                    im = im+1;
                    w(im) = w(je);
                    iblock(im) = iblock(je);
                }
            }
            m = im;
        }
        if( idiscl<0||idiscu<0 )
        {
            toofew = true;
        }
    }
    
    //
    // If ORDER='B', do nothing -- the eigenvalues are already sorted
    //    by block.
    // If ORDER='E', sort the eigenvalues from smallest to largest
    //
    if( iorder==1&&nsplit>1 )
    {
        for(je = 1; je <= m-1; je++)
        {
            ie = 0;
            tmp1 = w(je);
            for(j = je+1; j <= m; j++)
            {
                if( ap::fp_less(w(j),tmp1) )
                {
                    ie = j;
                    tmp1 = w(j);
                }
            }
            if( ie!=0 )
            {
                itmp1 = iblock(ie);
                w(ie) = w(je);
                iblock(ie) = iblock(je);
                w(je) = tmp1;
                iblock(je) = itmp1;
            }
        }
    }
    for(j = 1; j <= m; j++)
    {
        w(j) = w(j)*scalefactor;
    }
    errorcode = 0;
    if( ncnvrg )
    {
        errorcode = errorcode+1;
    }
    if( toofew )
    {
        errorcode = errorcode+2;
    }
    result = errorcode==0;
    return result;
}


void internaldstein(const int& n,
     const ap::real_1d_array& d,
     ap::real_1d_array e,
     const int& m,
     ap::real_1d_array w,
     const ap::integer_1d_array& iblock,
     const ap::integer_1d_array& isplit,
     ap::real_2d_array& z,
     ap::integer_1d_array& ifail,
     int& info)
{
    int maxits;
    int extra;
    int b1;
    int blksiz;
    int bn;
    int gpind;
    int i;
    int iinfo;
    int its;
    int j;
    int j1;
    int jblk;
    int jmax;
    int nblk;
    int nrmchk;
    double dtpcrt;
    double eps;
    double eps1;
    double nrm;
    double onenrm;
    double ortol;
    double pertol;
    double scl;
    double sep;
    double tol;
    double xj;
    double xjm;
    double ztr;
    ap::real_1d_array work1;
    ap::real_1d_array work2;
    ap::real_1d_array work3;
    ap::real_1d_array work4;
    ap::real_1d_array work5;
    ap::integer_1d_array iwork;
    bool tmpcriterion;
    int ti;
    int i1;
    int i2;
    double v;

    maxits = 5;
    extra = 2;
    work1.setbounds(1, ap::maxint(n, 1));
    work2.setbounds(1, ap::maxint(n-1, 1));
    work3.setbounds(1, ap::maxint(n, 1));
    work4.setbounds(1, ap::maxint(n, 1));
    work5.setbounds(1, ap::maxint(n, 1));
    iwork.setbounds(1, ap::maxint(n, 1));
    ifail.setbounds(1, ap::maxint(m, 1));
    z.setbounds(1, ap::maxint(n, 1), 1, ap::maxint(m, 1));
    
    //
    // Test the input parameters.
    //
    info = 0;
    for(i = 1; i <= m; i++)
    {
        ifail(i) = 0;
    }
    if( n<0 )
    {
        info = -1;
        return;
    }
    if( m<0||m>n )
    {
        info = -4;
        return;
    }
    for(j = 2; j <= m; j++)
    {
        if( iblock(j)<iblock(j-1) )
        {
            info = -6;
            break;
        }
        if( iblock(j)==iblock(j-1)&&ap::fp_less(w(j),w(j-1)) )
        {
            info = -5;
            break;
        }
    }
    if( info!=0 )
    {
        return;
    }
    
    //
    // Quick return if possible
    //
    if( n==0||m==0 )
    {
        return;
    }
    if( n==1 )
    {
        z(1,1) = 1;
        return;
    }
    
    //
    // Some preparations
    //
    ti = n-1;
    ap::vmove(&work1(1), &e(1), ap::vlen(1,ti));
    e.setbounds(1, n);
    ap::vmove(&e(1), &work1(1), ap::vlen(1,ti));
    ap::vmove(&work1(1), &w(1), ap::vlen(1,m));
    w.setbounds(1, n);
    ap::vmove(&w(1), &work1(1), ap::vlen(1,m));
    
    //
    // Get machine constants.
    //
    eps = ap::machineepsilon;
    
    //
    // Compute eigenvectors of matrix blocks.
    //
    j1 = 1;
    for(nblk = 1; nblk <= iblock(m); nblk++)
    {
        
        //
        // Find starting and ending indices of block nblk.
        //
        if( nblk==1 )
        {
            b1 = 1;
        }
        else
        {
            b1 = isplit(nblk-1)+1;
        }
        bn = isplit(nblk);
        blksiz = bn-b1+1;
        if( blksiz!=1 )
        {
            
            //
            // Compute reorthogonalization criterion and stopping criterion.
            //
            gpind = b1;
            onenrm = fabs(d(b1))+fabs(e(b1));
            onenrm = ap::maxreal(onenrm, fabs(d(bn))+fabs(e(bn-1)));
            for(i = b1+1; i <= bn-1; i++)
            {
                onenrm = ap::maxreal(onenrm, fabs(d(i))+fabs(e(i-1))+fabs(e(i)));
            }
            ortol = 0.001*onenrm;
            dtpcrt = sqrt(0.1/blksiz);
        }
        
        //
        // Loop through eigenvalues of block nblk.
        //
        jblk = 0;
        for(j = j1; j <= m; j++)
        {
            if( iblock(j)!=nblk )
            {
                j1 = j;
                break;
            }
            jblk = jblk+1;
            xj = w(j);
            if( blksiz==1 )
            {
                
                //
                // Skip all the work if the block size is one.
                //
                work1(1) = 1;
            }
            else
            {
                
                //
                // If eigenvalues j and j-1 are too close, add a relatively
                // small perturbation.
                //
                if( jblk>1 )
                {
                    eps1 = fabs(eps*xj);
                    pertol = 10*eps1;
                    sep = xj-xjm;
                    if( ap::fp_less(sep,pertol) )
                    {
                        xj = xjm+pertol;
                    }
                }
                its = 0;
                nrmchk = 0;
                
                //
                // Get random starting vector.
                //
                for(ti = 1; ti <= blksiz; ti++)
                {
                    work1(ti) = 2*ap::randomreal()-1;
                }
                
                //
                // Copy the matrix T so it won't be destroyed in factorization.
                //
                for(ti = 1; ti <= blksiz-1; ti++)
                {
                    work2(ti) = e(b1+ti-1);
                    work3(ti) = e(b1+ti-1);
                    work4(ti) = d(b1+ti-1);
                }
                work4(blksiz) = d(b1+blksiz-1);
                
                //
                // Compute LU factors with partial pivoting  ( PT = LU )
                //
                tol = 0;
                tdininternaldlagtf(blksiz, work4, xj, work2, work3, tol, work5, iwork, iinfo);
                
                //
                // Update iteration count.
                //
                do
                {
                    its = its+1;
                    if( its>maxits )
                    {
                        
                        //
                        // If stopping criterion was not satisfied, update info and
                        // store eigenvector number in array ifail.
                        //
                        info = info+1;
                        ifail(info) = j;
                        break;
                    }
                    
                    //
                    // Normalize and scale the righthand side vector Pb.
                    //
                    v = 0;
                    for(ti = 1; ti <= blksiz; ti++)
                    {
                        v = v+fabs(work1(ti));
                    }
                    scl = blksiz*onenrm*ap::maxreal(eps, fabs(work4(blksiz)))/v;
                    ap::vmul(&work1(1), ap::vlen(1,blksiz), scl);
                    
                    //
                    // Solve the system LU = Pb.
                    //
                    tdininternaldlagts(blksiz, work4, work2, work3, work5, iwork, work1, tol, iinfo);
                    
                    //
                    // Reorthogonalize by modified Gram-Schmidt if eigenvalues are
                    // close enough.
                    //
                    if( jblk!=1 )
                    {
                        if( ap::fp_greater(fabs(xj-xjm),ortol) )
                        {
                            gpind = j;
                        }
                        if( gpind!=j )
                        {
                            for(i = gpind; i <= j-1; i++)
                            {
                                i1 = b1;
                                i2 = b1+blksiz-1;
                                ztr = ap::vdotproduct(work1.getvector(1, blksiz), z.getcolumn(i, i1, i2));
                                ap::vsub(work1.getvector(1, blksiz), z.getcolumn(i, i1, i2), ztr);
                            }
                        }
                    }
                    
                    //
                    // Check the infinity norm of the iterate.
                    //
                    jmax = vectoridxabsmax(work1, 1, blksiz);
                    nrm = fabs(work1(jmax));
                    
                    //
                    // Continue for additional iterations after norm reaches
                    // stopping criterion.
                    //
                    tmpcriterion = false;
                    if( ap::fp_less(nrm,dtpcrt) )
                    {
                        tmpcriterion = true;
                    }
                    else
                    {
                        nrmchk = nrmchk+1;
                        if( nrmchk<extra+1 )
                        {
                            tmpcriterion = true;
                        }
                    }
                }
                while(tmpcriterion);
                
                //
                // Accept iterate as jth eigenvector.
                //
                scl = 1/vectornorm2(work1, 1, blksiz);
                jmax = vectoridxabsmax(work1, 1, blksiz);
                if( ap::fp_less(work1(jmax),0) )
                {
                    scl = -scl;
                }
                ap::vmul(&work1(1), ap::vlen(1,blksiz), scl);
            }
            for(i = 1; i <= n; i++)
            {
                z(i,j) = 0;
            }
            for(i = 1; i <= blksiz; i++)
            {
                z(b1+i-1,j) = work1(i);
            }
            
            //
            // Save the shift to check eigenvalue spacing at next
            // iteration.
            //
            xjm = xj;
        }
    }
}


static void tdininternaldlagtf(const int& n,
     ap::real_1d_array& a,
     const double& lambda,
     ap::real_1d_array& b,
     ap::real_1d_array& c,
     const double& tol,
     ap::real_1d_array& d,
     ap::integer_1d_array& iin,
     int& info)
{
    int k;
    double eps;
    double mult;
    double piv1;
    double piv2;
    double scale1;
    double scale2;
    double temp;
    double tl;

    info = 0;
    if( n<0 )
    {
        info = -1;
        return;
    }
    if( n==0 )
    {
        return;
    }
    a(1) = a(1)-lambda;
    iin(n) = 0;
    if( n==1 )
    {
        if( ap::fp_eq(a(1),0) )
        {
            iin(1) = 1;
        }
        return;
    }
    eps = ap::machineepsilon;
    tl = ap::maxreal(tol, eps);
    scale1 = fabs(a(1))+fabs(b(1));
    for(k = 1; k <= n-1; k++)
    {
        a(k+1) = a(k+1)-lambda;
        scale2 = fabs(c(k))+fabs(a(k+1));
        if( k<n-1 )
        {
            scale2 = scale2+fabs(b(k+1));
        }
        if( ap::fp_eq(a(k),0) )
        {
            piv1 = 0;
        }
        else
        {
            piv1 = fabs(a(k))/scale1;
        }
        if( ap::fp_eq(c(k),0) )
        {
            iin(k) = 0;
            piv2 = 0;
            scale1 = scale2;
            if( k<n-1 )
            {
                d(k) = 0;
            }
        }
        else
        {
            piv2 = fabs(c(k))/scale2;
            if( ap::fp_less_eq(piv2,piv1) )
            {
                iin(k) = 0;
                scale1 = scale2;
                c(k) = c(k)/a(k);
                a(k+1) = a(k+1)-c(k)*b(k);
                if( k<n-1 )
                {
                    d(k) = 0;
                }
            }
            else
            {
                iin(k) = 1;
                mult = a(k)/c(k);
                a(k) = c(k);
                temp = a(k+1);
                a(k+1) = b(k)-mult*temp;
                if( k<n-1 )
                {
                    d(k) = b(k+1);
                    b(k+1) = -mult*d(k);
                }
                b(k) = temp;
                c(k) = mult;
            }
        }
        if( ap::fp_less_eq(ap::maxreal(piv1, piv2),tl)&&iin(n)==0 )
        {
            iin(n) = k;
        }
    }
    if( ap::fp_less_eq(fabs(a(n)),scale1*tl)&&iin(n)==0 )
    {
        iin(n) = n;
    }
}


static void tdininternaldlagts(const int& n,
     const ap::real_1d_array& a,
     const ap::real_1d_array& b,
     const ap::real_1d_array& c,
     const ap::real_1d_array& d,
     const ap::integer_1d_array& iin,
     ap::real_1d_array& y,
     double& tol,
     int& info)
{
    int k;
    double absak;
    double ak;
    double bignum;
    double eps;
    double pert;
    double sfmin;
    double temp;

    info = 0;
    if( n<0 )
    {
        info = -1;
        return;
    }
    if( n==0 )
    {
        return;
    }
    eps = ap::machineepsilon;
    sfmin = ap::minrealnumber;
    bignum = 1/sfmin;
    if( ap::fp_less_eq(tol,0) )
    {
        tol = fabs(a(1));
        if( n>1 )
        {
            tol = ap::maxreal(tol, ap::maxreal(fabs(a(2)), fabs(b(1))));
        }
        for(k = 3; k <= n; k++)
        {
            tol = ap::maxreal(tol, ap::maxreal(fabs(a(k)), ap::maxreal(fabs(b(k-1)), fabs(d(k-2)))));
        }
        tol = tol*eps;
        if( ap::fp_eq(tol,0) )
        {
            tol = eps;
        }
    }
    for(k = 2; k <= n; k++)
    {
        if( iin(k-1)==0 )
        {
            y(k) = y(k)-c(k-1)*y(k-1);
        }
        else
        {
            temp = y(k-1);
            y(k-1) = y(k);
            y(k) = temp-c(k-1)*y(k);
        }
    }
    for(k = n; k >= 1; k--)
    {
        if( k<=n-2 )
        {
            temp = y(k)-b(k)*y(k+1)-d(k)*y(k+2);
        }
        else
        {
            if( k==n-1 )
            {
                temp = y(k)-b(k)*y(k+1);
            }
            else
            {
                temp = y(k);
            }
        }
        ak = a(k);
        pert = fabs(tol);
        if( ap::fp_less(ak,0) )
        {
            pert = -pert;
        }
        while(true)
        {
            absak = fabs(ak);
            if( ap::fp_less(absak,1) )
            {
                if( ap::fp_less(absak,sfmin) )
                {
                    if( ap::fp_eq(absak,0)||ap::fp_greater(fabs(temp)*sfmin,absak) )
                    {
                        ak = ak+pert;
                        pert = 2*pert;
                        continue;
                    }
                    else
                    {
                        temp = temp*bignum;
                        ak = ak*bignum;
                    }
                }
                else
                {
                    if( ap::fp_greater(fabs(temp),absak*bignum) )
                    {
                        ak = ak+pert;
                        pert = 2*pert;
                        continue;
                    }
                }
            }
            break;
        }
        y(k) = temp/ak;
    }
}


static void internaldlaebz(const int& ijob,
     const int& nitmax,
     const int& n,
     const int& mmax,
     const int& minp,
     const double& abstol,
     const double& reltol,
     const double& pivmin,
     const ap::real_1d_array& d,
     const ap::real_1d_array& e,
     const ap::real_1d_array& e2,
     ap::integer_1d_array& nval,
     ap::real_2d_array& ab,
     ap::real_1d_array& c,
     int& mout,
     ap::integer_2d_array& nab,
     ap::real_1d_array& work,
     ap::integer_1d_array& iwork,
     int& info)
{
    int itmp1;
    int itmp2;
    int j;
    int ji;
    int jit;
    int jp;
    int kf;
    int kfnew;
    int kl;
    int klnew;
    double tmp1;
    double tmp2;

    info = 0;
    if( ijob<1||ijob>3 )
    {
        info = -1;
        return;
    }
    
    //
    // Initialize NAB
    //
    if( ijob==1 )
    {
        
        //
        // Compute the number of eigenvalues in the initial intervals.
        //
        mout = 0;
        
        //
        //DIR$ NOVECTOR
        //
        for(ji = 1; ji <= minp; ji++)
        {
            for(jp = 1; jp <= 2; jp++)
            {
                tmp1 = d(1)-ab(ji,jp);
                if( ap::fp_less(fabs(tmp1),pivmin) )
                {
                    tmp1 = -pivmin;
                }
                nab(ji,jp) = 0;
                if( ap::fp_less_eq(tmp1,0) )
                {
                    nab(ji,jp) = 1;
                }
                for(j = 2; j <= n; j++)
                {
                    tmp1 = d(j)-e2(j-1)/tmp1-ab(ji,jp);
                    if( ap::fp_less(fabs(tmp1),pivmin) )
                    {
                        tmp1 = -pivmin;
                    }
                    if( ap::fp_less_eq(tmp1,0) )
                    {
                        nab(ji,jp) = nab(ji,jp)+1;
                    }
                }
            }
            mout = mout+nab(ji,2)-nab(ji,1);
        }
        return;
    }
    
    //
    // Initialize for loop
    //
    // KF and KL have the following meaning:
    //   Intervals 1,...,KF-1 have converged.
    //   Intervals KF,...,KL  still need to be refined.
    //
    kf = 1;
    kl = minp;
    
    //
    // If IJOB=2, initialize C.
    // If IJOB=3, use the user-supplied starting point.
    //
    if( ijob==2 )
    {
        for(ji = 1; ji <= minp; ji++)
        {
            c(ji) = 0.5*(ab(ji,1)+ab(ji,2));
        }
    }
    
    //
    // Iteration loop
    //
    for(jit = 1; jit <= nitmax; jit++)
    {
        
        //
        // Loop over intervals
        //
        //
        // Serial Version of the loop
        //
        klnew = kl;
        for(ji = kf; ji <= kl; ji++)
        {
            
            //
            // Compute N(w), the number of eigenvalues less than w
            //
            tmp1 = c(ji);
            tmp2 = d(1)-tmp1;
            itmp1 = 0;
            if( ap::fp_less_eq(tmp2,pivmin) )
            {
                itmp1 = 1;
                tmp2 = ap::minreal(tmp2, -pivmin);
            }
            
            //
            // A series of compiler directives to defeat vectorization
            // for the next loop
            //
            //*$PL$ CMCHAR=' '
            //CDIR$          NEXTSCALAR
            //C$DIR          SCALAR
            //CDIR$          NEXT SCALAR
            //CVD$L          NOVECTOR
            //CDEC$          NOVECTOR
            //CVD$           NOVECTOR
            //*VDIR          NOVECTOR
            //*VOCL          LOOP,SCALAR
            //CIBM           PREFER SCALAR
            //*$PL$ CMCHAR='*'
            //
            for(j = 2; j <= n; j++)
            {
                tmp2 = d(j)-e2(j-1)/tmp2-tmp1;
                if( ap::fp_less_eq(tmp2,pivmin) )
                {
                    itmp1 = itmp1+1;
                    tmp2 = ap::minreal(tmp2, -pivmin);
                }
            }
            if( ijob<=2 )
            {
                
                //
                // IJOB=2: Choose all intervals containing eigenvalues.
                //
                // Insure that N(w) is monotone
                //
                itmp1 = ap::minint(nab(ji,2), ap::maxint(nab(ji,1), itmp1));
                
                //
                // Update the Queue -- add intervals if both halves
                // contain eigenvalues.
                //
                if( itmp1==nab(ji,2) )
                {
                    
                    //
                    // No eigenvalue in the upper interval:
                    // just use the lower interval.
                    //
                    ab(ji,2) = tmp1;
                }
                else
                {
                    if( itmp1==nab(ji,1) )
                    {
                        
                        //
                        // No eigenvalue in the lower interval:
                        // just use the upper interval.
                        //
                        ab(ji,1) = tmp1;
                    }
                    else
                    {
                        if( klnew<mmax )
                        {
                            
                            //
                            // Eigenvalue in both intervals -- add upper to queue.
                            //
                            klnew = klnew+1;
                            ab(klnew,2) = ab(ji,2);
                            nab(klnew,2) = nab(ji,2);
                            ab(klnew,1) = tmp1;
                            nab(klnew,1) = itmp1;
                            ab(ji,2) = tmp1;
                            nab(ji,2) = itmp1;
                        }
                        else
                        {
                            info = mmax+1;
                            return;
                        }
                    }
                }
            }
            else
            {
                
                //
                // IJOB=3: Binary search.  Keep only the interval
                // containing  w  s.t. N(w) = NVAL
                //
                if( itmp1<=nval(ji) )
                {
                    ab(ji,1) = tmp1;
                    nab(ji,1) = itmp1;
                }
                if( itmp1>=nval(ji) )
                {
                    ab(ji,2) = tmp1;
                    nab(ji,2) = itmp1;
                }
            }
        }
        kl = klnew;
        
        //
        // Check for convergence
        //
        kfnew = kf;
        for(ji = kf; ji <= kl; ji++)
        {
            tmp1 = fabs(ab(ji,2)-ab(ji,1));
            tmp2 = ap::maxreal(fabs(ab(ji,2)), fabs(ab(ji,1)));
            if( ap::fp_less(tmp1,ap::maxreal(abstol, ap::maxreal(pivmin, reltol*tmp2)))||nab(ji,1)>=nab(ji,2) )
            {
                
                //
                // Converged -- Swap with position KFNEW,
                // then increment KFNEW
                //
                if( ji>kfnew )
                {
                    tmp1 = ab(ji,1);
                    tmp2 = ab(ji,2);
                    itmp1 = nab(ji,1);
                    itmp2 = nab(ji,2);
                    ab(ji,1) = ab(kfnew,1);
                    ab(ji,2) = ab(kfnew,2);
                    nab(ji,1) = nab(kfnew,1);
                    nab(ji,2) = nab(kfnew,2);
                    ab(kfnew,1) = tmp1;
                    ab(kfnew,2) = tmp2;
                    nab(kfnew,1) = itmp1;
                    nab(kfnew,2) = itmp2;
                    if( ijob==3 )
                    {
                        itmp1 = nval(ji);
                        nval(ji) = nval(kfnew);
                        nval(kfnew) = itmp1;
                    }
                }
                kfnew = kfnew+1;
            }
        }
        kf = kfnew;
        
        //
        // Choose Midpoints
        //
        for(ji = kf; ji <= kl; ji++)
        {
            c(ji) = 0.5*(ab(ji,1)+ab(ji,2));
        }
        
        //
        // If no more intervals to refine, quit.
        //
        if( kf>kl )
        {
            break;
        }
    }
    
    //
    // Converged
    //
    info = ap::maxint(kl+1-kf, 0);
    mout = kl;
}




