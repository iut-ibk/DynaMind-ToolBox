/*************************************************************************
Copyright (c) 2006-2007, Sergey Bochkanov (ALGLIB project).

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
#include "leastsquares.h"

/*************************************************************************
Weighted approximation by arbitrary function basis in a space of arbitrary
dimension using linear least squares method.

Input parameters:
    Y   -   array[0..N-1]
            It contains a set  of  function  values  in  N  points.  Space
            dimension  and  points  don't  matter.  Procedure  works  with
            function values in these points and values of basis  functions
            only.

    W   -   array[0..N-1]
            It contains weights corresponding  to  function  values.  Each
            summand in square sum of approximation deviations  from  given
            values is multiplied by the square of corresponding weight.

    FMatrix-a table of basis functions values, array[0..N-1, 0..M-1].
            FMatrix[I, J] - value of J-th basis function in I-th point.

    N   -   number of points used. N>=1.
    M   -   number of basis functions, M>=1.

Output parameters:
    C   -   decomposition coefficients.
            Array of real numbers whose index goes from 0 to M-1.
            C[j] - j-th basis function coefficient.

  -- ALGLIB --
     Copyright by Bochkanov Sergey
*************************************************************************/
void buildgeneralleastsquares(const ap::real_1d_array& y,
     const ap::real_1d_array& w,
     const ap::real_2d_array& fmatrix,
     int n,
     int m,
     ap::real_1d_array& c)
{
    int i;
    int j;
    ap::real_2d_array a;
    ap::real_2d_array q;
    ap::real_2d_array vt;
    ap::real_1d_array b;
    ap::real_1d_array tau;
    ap::real_2d_array b2;
    ap::real_1d_array tauq;
    ap::real_1d_array taup;
    ap::real_1d_array d;
    ap::real_1d_array e;
    bool isuppera;
    int mi;
    int ni;
    double v;

    mi = n;
    ni = m;
    c.setbounds(0, ni-1);
    
    //
    // Initialize design matrix.
    // Here we are making MI>=NI.
    //
    a.setbounds(1, ni, 1, ap::maxint(mi, ni));
    b.setbounds(1, ap::maxint(mi, ni));
    for(i = 1; i <= mi; i++)
    {
        b(i) = w(i-1)*y(i-1);
    }
    for(i = mi+1; i <= ni; i++)
    {
        b(i) = 0;
    }
    for(j = 1; j <= ni; j++)
    {
        ap::vmove(a.getrow(j, 1, mi), fmatrix.getcolumn(j-1, 0, mi-1));
    }
    for(j = 1; j <= ni; j++)
    {
        for(i = mi+1; i <= ni; i++)
        {
            a(j,i) = 0;
        }
    }
    for(j = 1; j <= ni; j++)
    {
        for(i = 1; i <= mi; i++)
        {
            a(j,i) = a(j,i)*w(i-1);
        }
    }
    mi = ap::maxint(mi, ni);
    
    //
    // LQ-decomposition of A'
    // B2 := Q*B
    //
    lqdecomposition(a, ni, mi, tau);
    unpackqfromlq(a, ni, mi, tau, ni, q);
    b2.setbounds(1, 1, 1, ni);
    for(j = 1; j <= ni; j++)
    {
        b2(1,j) = 0;
    }
    for(i = 1; i <= ni; i++)
    {
        v = ap::vdotproduct(&b(1), &q(i, 1), ap::vlen(1,mi));
        b2(1,i) = v;
    }
    
    //
    // Back from A' to A
    // Making cols(A)=rows(A)
    //
    for(i = 1; i <= ni-1; i++)
    {
        ap::vmove(a.getrow(i, i+1, ni), a.getcolumn(i, i+1, ni));
    }
    for(i = 2; i <= ni; i++)
    {
        for(j = 1; j <= i-1; j++)
        {
            a(i,j) = 0;
        }
    }
    
    //
    // Bidiagonal decomposition of A
    // A = Q * d2 * P'
    // B2 := (Q'*B2')'
    //
    tobidiagonal(a, ni, ni, tauq, taup);
    multiplybyqfrombidiagonal(a, ni, ni, tauq, b2, 1, ni, true, false);
    unpackptfrombidiagonal(a, ni, ni, taup, ni, vt);
    unpackdiagonalsfrombidiagonal(a, ni, ni, isuppera, d, e);
    
    //
    // Singular value decomposition of A
    // A = U * d * V'
    // B2 := (U'*B2')'
    //
    if( !bidiagonalsvddecomposition(d, e, ni, isuppera, false, b2, 1, q, 0, vt, ni) )
    {
        for(i = 0; i <= ni-1; i++)
        {
            c(i) = 0;
        }
        return;
    }
    
    //
    // B2 := (d^(-1) * B2')'
    //
    if( ap::fp_neq(d(1),0) )
    {
        for(i = 1; i <= ni; i++)
        {
            if( ap::fp_greater(d(i),ap::machineepsilon*10*sqrt(double(ni))*d(1)) )
            {
                b2(1,i) = b2(1,i)/d(i);
            }
            else
            {
                b2(1,i) = 0;
            }
        }
    }
    
    //
    // B := (V * B2')'
    //
    for(i = 1; i <= ni; i++)
    {
        b(i) = 0;
    }
    for(i = 1; i <= ni; i++)
    {
        v = b2(1,i);
        ap::vadd(&b(1), &vt(i, 1), ap::vlen(1,ni), v);
    }
    
    //
    // Out
    //
    for(i = 1; i <= ni; i++)
    {
        c(i-1) = b(i);
    }
}


/*************************************************************************
Linear approximation using least squares method

The subroutine calculates coefficients of  the  line  approximating  given
function.

Input parameters:
    X   -   array[0..N-1], it contains a set of abscissas.
    Y   -   array[0..N-1], function values.
    N   -   number of points, N>=1

Output parameters:
    a, b-   coefficients of linear approximation a+b*t

  -- ALGLIB --
     Copyright by Bochkanov Sergey
*************************************************************************/
void buildlinearleastsquares(const ap::real_1d_array& x,
     const ap::real_1d_array& y,
     int n,
     double& a,
     double& b)
{
    double pp;
    double qq;
    double pq;
    double b1;
    double b2;
    double d1;
    double d2;
    double t1;
    double t2;
    double phi;
    double c;
    double s;
    double m;
    int i;

    pp = n;
    qq = 0;
    pq = 0;
    b1 = 0;
    b2 = 0;
    for(i = 0; i <= n-1; i++)
    {
        pq = pq+x(i);
        qq = qq+ap::sqr(x(i));
        b1 = b1+y(i);
        b2 = b2+x(i)*y(i);
    }
    phi = atan2(2*pq, qq-pp)/2;
    c = cos(phi);
    s = sin(phi);
    d1 = ap::sqr(c)*pp+ap::sqr(s)*qq-2*s*c*pq;
    d2 = ap::sqr(s)*pp+ap::sqr(c)*qq+2*s*c*pq;
    if( ap::fp_greater(fabs(d1),fabs(d2)) )
    {
        m = fabs(d1);
    }
    else
    {
        m = fabs(d2);
    }
    t1 = c*b1-s*b2;
    t2 = s*b1+c*b2;
    if( ap::fp_greater(fabs(d1),m*ap::machineepsilon*1000) )
    {
        t1 = t1/d1;
    }
    else
    {
        t1 = 0;
    }
    if( ap::fp_greater(fabs(d2),m*ap::machineepsilon*1000) )
    {
        t2 = t2/d2;
    }
    else
    {
        t2 = 0;
    }
    a = c*t1+s*t2;
    b = -s*t1+c*t2;
}


/*************************************************************************
Weighted cubic spline approximation using linear least squares

Input parameters:
    X   -   array[0..N-1], abscissas
    Y   -   array[0..N-1], function values
    W   -   array[0..N-1], weights.
    A, B-   interval to build splines in.
    N   -   number of points used. N>=1.
    M   -   number of basic splines, M>=2.

Output parameters:
    CTbl-   coefficients table to be used by SplineInterpolation function.
  -- ALGLIB --
     Copyright by Bochkanov Sergey
*************************************************************************/
void buildsplineleastsquares(const ap::real_1d_array& x,
     const ap::real_1d_array& y,
     const ap::real_1d_array& w,
     double a,
     double b,
     int n,
     int m,
     ap::real_1d_array& ctbl)
{
    int i;
    int j;
    ap::real_2d_array ma;
    ap::real_2d_array q;
    ap::real_2d_array vt;
    ap::real_1d_array mb;
    ap::real_1d_array tau;
    ap::real_2d_array b2;
    ap::real_1d_array tauq;
    ap::real_1d_array taup;
    ap::real_1d_array d;
    ap::real_1d_array e;
    bool isuppera;
    int mi;
    int ni;
    double v;
    ap::real_1d_array sx;
    ap::real_1d_array sy;

    ap::ap_error::make_assertion(m>=2, "BuildSplineLeastSquares: M is too small!");
    mi = n;
    ni = m;
    sx.setbounds(0, ni-1);
    sy.setbounds(0, ni-1);
    
    //
    // Initializing design matrix
    // Here we are making MI>=NI
    //
    ma.setbounds(1, ni, 1, ap::maxint(mi, ni));
    mb.setbounds(1, ap::maxint(mi, ni));
    for(j = 0; j <= ni-1; j++)
    {
        sx(j) = a+(b-a)*j/(ni-1);
    }
    for(j = 0; j <= ni-1; j++)
    {
        for(i = 0; i <= ni-1; i++)
        {
            sy(i) = 0;
        }
        sy(j) = 1;
        buildcubicspline(sx, sy, ni, 0, 0.0, 0, 0.0, ctbl);
        for(i = 0; i <= mi-1; i++)
        {
            ma(j+1,i+1) = w(i)*splineinterpolation(ctbl, x(i));
        }
    }
    for(j = 1; j <= ni; j++)
    {
        for(i = mi+1; i <= ni; i++)
        {
            ma(j,i) = 0;
        }
    }
    
    //
    // Initializing right part
    //
    for(i = 0; i <= mi-1; i++)
    {
        mb(i+1) = w(i)*y(i);
    }
    for(i = mi+1; i <= ni; i++)
    {
        mb(i) = 0;
    }
    mi = ap::maxint(mi, ni);
    
    //
    // LQ-decomposition of A'
    // B2 := Q*B
    //
    lqdecomposition(ma, ni, mi, tau);
    unpackqfromlq(ma, ni, mi, tau, ni, q);
    b2.setbounds(1, 1, 1, ni);
    for(j = 1; j <= ni; j++)
    {
        b2(1,j) = 0;
    }
    for(i = 1; i <= ni; i++)
    {
        v = ap::vdotproduct(&mb(1), &q(i, 1), ap::vlen(1,mi));
        b2(1,i) = v;
    }
    
    //
    // Back from A' to A
    // Making cols(A)=rows(A)
    //
    for(i = 1; i <= ni-1; i++)
    {
        ap::vmove(ma.getrow(i, i+1, ni), ma.getcolumn(i, i+1, ni));
    }
    for(i = 2; i <= ni; i++)
    {
        for(j = 1; j <= i-1; j++)
        {
            ma(i,j) = 0;
        }
    }
    
    //
    // Bidiagonal decomposition of A
    // A = Q * d2 * P'
    // B2 := (Q'*B2')'
    //
    tobidiagonal(ma, ni, ni, tauq, taup);
    multiplybyqfrombidiagonal(ma, ni, ni, tauq, b2, 1, ni, true, false);
    unpackptfrombidiagonal(ma, ni, ni, taup, ni, vt);
    unpackdiagonalsfrombidiagonal(ma, ni, ni, isuppera, d, e);
    
    //
    // Singular value decomposition of A
    // A = U * d * V'
    // B2 := (U'*B2')'
    //
    if( !bidiagonalsvddecomposition(d, e, ni, isuppera, false, b2, 1, q, 0, vt, ni) )
    {
        for(i = 1; i <= ni; i++)
        {
            d(i) = 0;
            b2(1,i) = 0;
            for(j = 1; j <= ni; j++)
            {
                if( i==j )
                {
                    vt(i,j) = 1;
                }
                else
                {
                    vt(i,j) = 0;
                }
            }
        }
        b2(1,1) = 1;
    }
    
    //
    // B2 := (d^(-1) * B2')'
    //
    for(i = 1; i <= ni; i++)
    {
        if( ap::fp_greater(d(i),ap::machineepsilon*10*sqrt(double(ni))*d(1)) )
        {
            b2(1,i) = b2(1,i)/d(i);
        }
        else
        {
            b2(1,i) = 0;
        }
    }
    
    //
    // B := (V * B2')'
    //
    for(i = 1; i <= ni; i++)
    {
        mb(i) = 0;
    }
    for(i = 1; i <= ni; i++)
    {
        v = b2(1,i);
        ap::vadd(&mb(1), &vt(i, 1), ap::vlen(1,ni), v);
    }
    
    //
    // Forming result spline
    //
    for(i = 0; i <= ni-1; i++)
    {
        sy(i) = mb(i+1);
    }
    buildcubicspline(sx, sy, ni, 0, 0.0, 0, 0.0, ctbl);
}


/*************************************************************************
Polynomial approximation using least squares method

The subroutine calculates coefficients  of  the  polynomial  approximating
given function. It is recommended to use this function only if you need to
obtain coefficients of approximation polynomial. If you have to build  and
calculate polynomial approximation (NOT coefficients), it's better to  use
BuildChebyshevLeastSquares      subroutine     in     combination     with
CalculateChebyshevLeastSquares   subroutine.   The   result  of  Chebyshev
polynomial approximation is equivalent to the result obtained using powers
of X, but has higher  accuracy  due  to  better  numerical  properties  of
Chebyshev polynomials.

Input parameters:
    X   -   array[0..N-1], abscissas
    Y   -   array[0..N-1], function values
    N   -   number of points, N>=1
    M   -   order of polynomial required, M>=0

Output parameters:
    C   -   approximating polynomial coefficients, array[0..M],
            C[i] - coefficient at X^i.

  -- ALGLIB --
     Copyright by Bochkanov Sergey
*************************************************************************/
void buildpolynomialleastsquares(const ap::real_1d_array& x,
     const ap::real_1d_array& y,
     int n,
     int m,
     ap::real_1d_array& c)
{
    ap::real_1d_array ctbl;
    ap::real_1d_array w;
    ap::real_1d_array c1;
    double maxx;
    double minx;
    int i;
    int j;
    int k;
    double e;
    double d;
    double l1;
    double l2;
    ap::real_1d_array z2;
    ap::real_1d_array z1;

    
    //
    // Initialize
    //
    maxx = x(0);
    minx = x(0);
    for(i = 1; i <= n-1; i++)
    {
        if( ap::fp_greater(x(i),maxx) )
        {
            maxx = x(i);
        }
        if( ap::fp_less(x(i),minx) )
        {
            minx = x(i);
        }
    }
    if( ap::fp_eq(minx,maxx) )
    {
        minx = minx-0.5;
        maxx = maxx+0.5;
    }
    w.setbounds(0, n-1);
    for(i = 0; i <= n-1; i++)
    {
        w(i) = 1;
    }
    
    //
    // Build Chebyshev approximation
    //
    buildchebyshevleastsquares(x, y, w, minx, maxx, n, m, ctbl);
    
    //
    // From Chebyshev to powers of X
    //
    c1.setbounds(0, m);
    for(i = 0; i <= m; i++)
    {
        c1(i) = 0;
    }
    d = 0;
    for(i = 0; i <= m; i++)
    {
        for(k = i; k <= m; k++)
        {
            e = c1(k);
            c1(k) = 0;
            if( i<=1&&k==i )
            {
                c1(k) = 1;
            }
            else
            {
                if( i!=0 )
                {
                    c1(k) = 2*d;
                }
                if( k>i+1 )
                {
                    c1(k) = c1(k)-c1(k-2);
                }
            }
            d = e;
        }
        d = c1(i);
        e = 0;
        k = i;
        while(k<=m)
        {
            e = e+c1(k)*ctbl(k);
            k = k+2;
        }
        c1(i) = e;
    }
    
    //
    // Linear translation
    //
    l1 = 2/(ctbl(m+2)-ctbl(m+1));
    l2 = -2*ctbl(m+1)/(ctbl(m+2)-ctbl(m+1))-1;
    c.setbounds(0, m);
    z2.setbounds(0, m);
    z1.setbounds(0, m);
    c(0) = c1(0);
    z1(0) = 1;
    z2(0) = 1;
    for(i = 1; i <= m; i++)
    {
        z2(i) = 1;
        z1(i) = l2*z1(i-1);
        c(0) = c(0)+c1(i)*z1(i);
    }
    for(j = 1; j <= m; j++)
    {
        z2(0) = l1*z2(0);
        c(j) = c1(j)*z2(0);
        for(i = j+1; i <= m; i++)
        {
            k = i-j;
            z2(k) = l1*z2(k)+z2(k-1);
            c(j) = c(j)+c1(i)*z2(k)*z1(k);
        }
    }
}


/*************************************************************************
Chebyshev polynomial approximation using least squares method.

The algorithm reduces interval [A, B] to the interval [-1,1], then  builds
least squares approximation using Chebyshev polynomials.

Input parameters:
    X   -   array[0..N-1], abscissas
    Y   -   array[0..N-1], function values
    W   -   array[0..N-1], weights
    A, B-   interval to build approximating polynomials in.
    N   -   number of points used. N>=1.
    M   -   order of polynomial, M>=0. This parameter is passed into
            CalculateChebyshevLeastSquares function.

Output parameters:
    CTbl - coefficient table. This parameter is passed into
            CalculateChebyshevLeastSquares function.
  -- ALGLIB --
     Copyright by Bochkanov Sergey
*************************************************************************/
void buildchebyshevleastsquares(const ap::real_1d_array& x,
     const ap::real_1d_array& y,
     const ap::real_1d_array& w,
     double a,
     double b,
     int n,
     int m,
     ap::real_1d_array& ctbl)
{
    int i;
    int j;
    ap::real_2d_array ma;
    ap::real_2d_array q;
    ap::real_2d_array vt;
    ap::real_1d_array mb;
    ap::real_1d_array tau;
    ap::real_2d_array b2;
    ap::real_1d_array tauq;
    ap::real_1d_array taup;
    ap::real_1d_array d;
    ap::real_1d_array e;
    bool isuppera;
    int mi;
    int ni;
    double v;

    mi = n;
    ni = m+1;
    
    //
    // Initializing design matrix
    // Here we are making MI>=NI
    //
    ma.setbounds(1, ni, 1, ap::maxint(mi, ni));
    mb.setbounds(1, ap::maxint(mi, ni));
    for(j = 1; j <= ni; j++)
    {
        for(i = 1; i <= mi; i++)
        {
            v = 2*(x(i-1)-a)/(b-a)-1;
            if( j==1 )
            {
                ma(j,i) = 1.0;
            }
            if( j==2 )
            {
                ma(j,i) = v;
            }
            if( j>2 )
            {
                ma(j,i) = 2.0*v*ma(j-1,i)-ma(j-2,i);
            }
        }
    }
    for(j = 1; j <= ni; j++)
    {
        for(i = 1; i <= mi; i++)
        {
            ma(j,i) = w(i-1)*ma(j,i);
        }
    }
    for(j = 1; j <= ni; j++)
    {
        for(i = mi+1; i <= ni; i++)
        {
            ma(j,i) = 0;
        }
    }
    
    //
    // Initializing right part
    //
    for(i = 0; i <= mi-1; i++)
    {
        mb(i+1) = w(i)*y(i);
    }
    for(i = mi+1; i <= ni; i++)
    {
        mb(i) = 0;
    }
    mi = ap::maxint(mi, ni);
    
    //
    // LQ-decomposition of A'
    // B2 := Q*B
    //
    lqdecomposition(ma, ni, mi, tau);
    unpackqfromlq(ma, ni, mi, tau, ni, q);
    b2.setbounds(1, 1, 1, ni);
    for(j = 1; j <= ni; j++)
    {
        b2(1,j) = 0;
    }
    for(i = 1; i <= ni; i++)
    {
        v = ap::vdotproduct(&mb(1), &q(i, 1), ap::vlen(1,mi));
        b2(1,i) = v;
    }
    
    //
    // Back from A' to A
    // Making cols(A)=rows(A)
    //
    for(i = 1; i <= ni-1; i++)
    {
        ap::vmove(ma.getrow(i, i+1, ni), ma.getcolumn(i, i+1, ni));
    }
    for(i = 2; i <= ni; i++)
    {
        for(j = 1; j <= i-1; j++)
        {
            ma(i,j) = 0;
        }
    }
    
    //
    // Bidiagonal decomposition of A
    // A = Q * d2 * P'
    // B2 := (Q'*B2')'
    //
    tobidiagonal(ma, ni, ni, tauq, taup);
    multiplybyqfrombidiagonal(ma, ni, ni, tauq, b2, 1, ni, true, false);
    unpackptfrombidiagonal(ma, ni, ni, taup, ni, vt);
    unpackdiagonalsfrombidiagonal(ma, ni, ni, isuppera, d, e);
    
    //
    // Singular value decomposition of A
    // A = U * d * V'
    // B2 := (U'*B2')'
    //
    if( !bidiagonalsvddecomposition(d, e, ni, isuppera, false, b2, 1, q, 0, vt, ni) )
    {
        for(i = 1; i <= ni; i++)
        {
            d(i) = 0;
            b2(1,i) = 0;
            for(j = 1; j <= ni; j++)
            {
                if( i==j )
                {
                    vt(i,j) = 1;
                }
                else
                {
                    vt(i,j) = 0;
                }
            }
        }
        b2(1,1) = 1;
    }
    
    //
    // B2 := (d^(-1) * B2')'
    //
    for(i = 1; i <= ni; i++)
    {
        if( ap::fp_greater(d(i),ap::machineepsilon*10*sqrt(double(ni))*d(1)) )
        {
            b2(1,i) = b2(1,i)/d(i);
        }
        else
        {
            b2(1,i) = 0;
        }
    }
    
    //
    // B := (V * B2')'
    //
    for(i = 1; i <= ni; i++)
    {
        mb(i) = 0;
    }
    for(i = 1; i <= ni; i++)
    {
        v = b2(1,i);
        ap::vadd(&mb(1), &vt(i, 1), ap::vlen(1,ni), v);
    }
    
    //
    // Forming result
    //
    ctbl.setbounds(0, ni+1);
    for(i = 1; i <= ni; i++)
    {
        ctbl(i-1) = mb(i);
    }
    ctbl(ni) = a;
    ctbl(ni+1) = b;
}


/*************************************************************************
Weighted Chebyshev polynomial constrained least squares approximation.

The algorithm reduces [A,B] to [-1,1] and builds the Chebyshev polynomials
series by approximating a given function using the least squares method.

Input parameters:
    X   -   abscissas, array[0..N-1]
    Y   -   function values, array[0..N-1]
    W   -   weights, array[0..N-1].  Each  item  in  the  squared  sum  of
            deviations from given values is  multiplied  by  a  square  of
            corresponding weight.
    A, B-   interval in which the approximating polynomials are built.
    N   -   number of points, N>0.
    XC, YC, DC-
            constraints (see description below)., array[0..NC-1]
    NC  -   number of constraints. 0 <= NC < M+1.
    M   -   degree of polynomial, M>=0. This parameter is passed into  the
            CalculateChebyshevLeastSquares subroutine.

Output parameters:
    CTbl-   coefficient  table.  This  parameter  is   passed   into   the
            CalculateChebyshevLeastSquares subroutine.

Result:
    True, if the algorithm succeeded.
    False, if the internal singular value decomposition subroutine  hasn't
converged or the given constraints could not be met  simultaneously  (e.g.
P(0)=0 è P(0)=1).

Specifying constraints:
    This subroutine can solve  the  problem  having  constrained  function
values or its derivatives in several points. NC specifies  the  number  of
constraints, DC - the type of constraints, XC and YC - constraints as such.
Thus, for each i from 0 to NC-1 the following constraint is given:
    P(xc[i]) = yc[i],       if DC[i]=0
or
    d/dx(P(xc[i])) = yc[i], if DC[i]=1
(here P(x) is approximating polynomial).
    This version of the subroutine supports only either polynomial or  its
derivative value constraints.  If  DC[i]  is  not  equal  to  0 and 1, the
subroutine will be aborted. The number of constraints should be less  than
the number of degrees of freedom of approximating  polynomial  -  M+1  (at
that, it could be equal to 0).

  -- ALGLIB --
     Copyright by Bochkanov Sergey
*************************************************************************/
bool buildchebyshevleastsquaresconstrained(const ap::real_1d_array& x,
     const ap::real_1d_array& y,
     const ap::real_1d_array& w,
     double a,
     double b,
     int n,
     const ap::real_1d_array& xc,
     const ap::real_1d_array& yc,
     const ap::integer_1d_array& dc,
     int nc,
     int m,
     ap::real_1d_array& ctbl)
{
    bool result;
    int i;
    int j;
    int reducedsize;
    ap::real_2d_array designmatrix;
    ap::real_1d_array rightpart;
    ap::real_2d_array cmatrix;
    ap::real_2d_array c;
    ap::real_2d_array u;
    ap::real_2d_array vt;
    ap::real_1d_array d;
    ap::real_1d_array cr;
    ap::real_1d_array ws;
    ap::real_1d_array tj;
    ap::real_1d_array uj;
    ap::real_1d_array dtj;
    ap::real_1d_array tmp;
    ap::real_1d_array tmp2;
    ap::real_2d_array tmpmatrix;
    double v;

    ap::ap_error::make_assertion(n>0, "");
    ap::ap_error::make_assertion(m>=0, "");
    ap::ap_error::make_assertion(nc>=0&&nc<m+1, "");
    result = true;
    
    //
    // Initialize design matrix and right part.
    // Add fictional rows if needed to ensure that N>=M+1.
    //
    designmatrix.setbounds(1, ap::maxint(n, m+1), 1, m+1);
    rightpart.setbounds(1, ap::maxint(n, m+1));
    for(i = 1; i <= n; i++)
    {
        for(j = 1; j <= m+1; j++)
        {
            v = 2*(x(i-1)-a)/(b-a)-1;
            if( j==1 )
            {
                designmatrix(i,j) = 1.0;
            }
            if( j==2 )
            {
                designmatrix(i,j) = v;
            }
            if( j>2 )
            {
                designmatrix(i,j) = 2.0*v*designmatrix(i,j-1)-designmatrix(i,j-2);
            }
        }
    }
    for(i = 1; i <= n; i++)
    {
        for(j = 1; j <= m+1; j++)
        {
            designmatrix(i,j) = w(i-1)*designmatrix(i,j);
        }
    }
    for(i = n+1; i <= m+1; i++)
    {
        for(j = 1; j <= m+1; j++)
        {
            designmatrix(i,j) = 0;
        }
    }
    for(i = 0; i <= n-1; i++)
    {
        rightpart(i+1) = w(i)*y(i);
    }
    for(i = n+1; i <= m+1; i++)
    {
        rightpart(i) = 0;
    }
    n = ap::maxint(n, m+1);
    
    //
    // Now N>=M+1 and we are ready to the next stage.
    // Handle constraints.
    // Represent feasible set of coefficients as x = C*t + d
    //
    c.setbounds(1, m+1, 1, m+1);
    d.setbounds(1, m+1);
    if( nc==0 )
    {
        
        //
        // No constraints
        //
        for(i = 1; i <= m+1; i++)
        {
            for(j = 1; j <= m+1; j++)
            {
                c(i,j) = 0;
            }
            d(i) = 0;
        }
        for(i = 1; i <= m+1; i++)
        {
            c(i,i) = 1;
        }
        reducedsize = m+1;
    }
    else
    {
        
        //
        // Constraints are present.
        // Fill constraints matrix CMatrix and solve CMatrix*x = cr.
        //
        cmatrix.setbounds(1, nc, 1, m+1);
        cr.setbounds(1, nc);
        tj.setbounds(0, m);
        uj.setbounds(0, m);
        dtj.setbounds(0, m);
        for(i = 0; i <= nc-1; i++)
        {
            v = 2*(xc(i)-a)/(b-a)-1;
            for(j = 0; j <= m; j++)
            {
                if( j==0 )
                {
                    tj(j) = 1;
                    uj(j) = 1;
                    dtj(j) = 0;
                }
                if( j==1 )
                {
                    tj(j) = v;
                    uj(j) = 2*v;
                    dtj(j) = 1;
                }
                if( j>1 )
                {
                    tj(j) = 2*v*tj(j-1)-tj(j-2);
                    uj(j) = 2*v*uj(j-1)-uj(j-2);
                    dtj(j) = j*uj(j-1);
                }
                ap::ap_error::make_assertion(dc(i)==0||dc(i)==1, "");
                if( dc(i)==0 )
                {
                    cmatrix(i+1,j+1) = tj(j);
                }
                if( dc(i)==1 )
                {
                    cmatrix(i+1,j+1) = dtj(j);
                }
            }
            cr(i+1) = yc(i);
        }
        
        //
        // Solve CMatrix*x = cr.
        // Fill C and d:
        // 1. SVD: CMatrix = U * WS * V^T
        // 2. C := V[1:M+1,NC+1:M+1]
        // 3. tmp := WS^-1 * U^T * cr
        // 4. d := V[1:M+1,1:NC] * tmp
        //
        if( !svddecomposition(cmatrix, nc, m+1, 2, 2, 2, ws, u, vt) )
        {
            result = false;
            return result;
        }
        if( ap::fp_eq(ws(1),0)||ap::fp_less_eq(ws(nc),ap::machineepsilon*10*sqrt(double(nc))*ws(1)) )
        {
            result = false;
            return result;
        }
        c.setbounds(1, m+1, 1, m+1-nc);
        d.setbounds(1, m+1);
        for(i = 1; i <= m+1-nc; i++)
        {
            ap::vmove(c.getcolumn(i, 1, m+1), vt.getrow(nc+i, 1, m+1));
        }
        tmp.setbounds(1, nc);
        for(i = 1; i <= nc; i++)
        {
            v = ap::vdotproduct(u.getcolumn(i, 1, nc), cr.getvector(1, nc));
            tmp(i) = v/ws(i);
        }
        for(i = 1; i <= m+1; i++)
        {
            d(i) = 0;
        }
        for(i = 1; i <= nc; i++)
        {
            v = tmp(i);
            ap::vadd(&d(1), &vt(i, 1), ap::vlen(1,m+1), v);
        }
        
        //
        // Reduce problem:
        // 1. RightPart := RightPart - DesignMatrix*d
        // 2. DesignMatrix := DesignMatrix*C
        //
        for(i = 1; i <= n; i++)
        {
            v = ap::vdotproduct(&designmatrix(i, 1), &d(1), ap::vlen(1,m+1));
            rightpart(i) = rightpart(i)-v;
        }
        reducedsize = m+1-nc;
        tmpmatrix.setbounds(1, n, 1, reducedsize);
        tmp.setbounds(1, n);
        matrixmatrixmultiply(designmatrix, 1, n, 1, m+1, false, c, 1, m+1, 1, reducedsize, false, 1.0, tmpmatrix, 1, n, 1, reducedsize, 0.0, tmp);
        copymatrix(tmpmatrix, 1, n, 1, reducedsize, designmatrix, 1, n, 1, reducedsize);
    }
    
    //
    // Solve reduced problem DesignMatrix*t = RightPart.
    //
    if( !svddecomposition(designmatrix, n, reducedsize, 1, 1, 2, ws, u, vt) )
    {
        result = false;
        return result;
    }
    tmp.setbounds(1, reducedsize);
    tmp2.setbounds(1, reducedsize);
    for(i = 1; i <= reducedsize; i++)
    {
        tmp(i) = 0;
    }
    for(i = 1; i <= n; i++)
    {
        v = rightpart(i);
        ap::vadd(&tmp(1), &u(i, 1), ap::vlen(1,reducedsize), v);
    }
    for(i = 1; i <= reducedsize; i++)
    {
        if( ap::fp_neq(ws(i),0)&&ap::fp_greater(ws(i),ap::machineepsilon*10*sqrt(double(nc))*ws(1)) )
        {
            tmp(i) = tmp(i)/ws(i);
        }
        else
        {
            tmp(i) = 0;
        }
    }
    for(i = 1; i <= reducedsize; i++)
    {
        tmp2(i) = 0;
    }
    for(i = 1; i <= reducedsize; i++)
    {
        v = tmp(i);
        ap::vadd(&tmp2(1), &vt(i, 1), ap::vlen(1,reducedsize), v);
    }
    
    //
    // Solution is in the tmp2.
    // Transform it from t to x.
    //
    ctbl.setbounds(0, m+2);
    for(i = 1; i <= m+1; i++)
    {
        v = ap::vdotproduct(&c(i, 1), &tmp2(1), ap::vlen(1,reducedsize));
        ctbl(i-1) = v+d(i);
    }
    ctbl(m+1) = a;
    ctbl(m+2) = b;
    return result;
}


/*************************************************************************
Calculation of a Chebyshev  polynomial  obtained   during  least  squares
approximaion at the given point.

Input parameters:
    M   -   order of polynomial (parameter of the
            BuildChebyshevLeastSquares function).
    A   -   coefficient table.
            A[0..M] contains coefficients of the i-th Chebyshev polynomial.
            A[M+1] contains left boundary of approximation interval.
            A[M+2] contains right boundary of approximation interval.
    X   -   point to perform calculations in.

The result is the value at the given point.

It should be noted that array A contains coefficients  of  the  Chebyshev
polynomials defined on interval [-1,1].   Argument  is  reduced  to  this
interval before calculating polynomial value.
  -- ALGLIB --
     Copyright by Bochkanov Sergey
*************************************************************************/
double calculatechebyshevleastsquares(const int& m,
     const ap::real_1d_array& a,
     double x)
{
    double result;
    double b1;
    double b2;
    int i;

    x = 2*(x-a(m+1))/(a(m+2)-a(m+1))-1;
    b1 = 0;
    b2 = 0;
    i = m;
    do
    {
        result = 2*x*b1-b2+a(i);
        b2 = b1;
        b1 = result;
        i = i-1;
    }
    while(i>=0);
    result = result-x*b2;
    return result;
}




