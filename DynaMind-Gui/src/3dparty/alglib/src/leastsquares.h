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

#ifndef _leastsquares_h
#define _leastsquares_h

#include "ap.h"
#include "ialglib.h"

#include "spline3.h"
#include "reflections.h"
#include "lq.h"
#include "bidiagonal.h"
#include "rotations.h"
#include "bdsvd.h"
#include "qr.h"
#include "blas.h"
#include "svd.h"


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
     ap::real_1d_array& c);


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
     double& b);


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
     ap::real_1d_array& ctbl);


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
     ap::real_1d_array& c);


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
     ap::real_1d_array& ctbl);


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
     ap::real_1d_array& ctbl);


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
     double x);


#endif

