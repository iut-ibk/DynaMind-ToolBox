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

#ifndef _lq_h
#define _lq_h

#include "ap.h"
#include "ialglib.h"

#include "reflections.h"


/*************************************************************************
LQ decomposition of a rectangular matrix of size MxN

Input parameters:
    A   -   matrix A whose indexes range within [0..M-1, 0..N-1].
    M   -   number of rows in matrix A.
    N   -   number of columns in matrix A.

Output parameters:
    A   -   matrices L and Q in compact form (see below)
    Tau -   array of scalar factors which are used to form
            matrix Q. Array whose index ranges within [0..Min(M,N)-1].

Matrix A is represented as A = LQ, where Q is an orthogonal matrix of size
MxM, L - lower triangular (or lower trapezoid) matrix of size M x N.

The elements of matrix L are located on and below  the  main  diagonal  of
matrix A. The elements which are located in Tau array and above  the  main
diagonal of matrix A are used to form matrix Q as follows:

Matrix Q is represented as a product of elementary reflections

Q = H(k-1)*H(k-2)*...*H(1)*H(0),

where k = min(m,n), and each H(i) is of the form

H(i) = 1 - tau * v * (v^T)

where tau is a scalar stored in Tau[I]; v - real vector, so that v(0:i-1)=0,
v(i) = 1, v(i+1:n-1) stored in A(i,i+1:n-1).

  -- ALGLIB --
     Copyright 2005-2007 by Bochkanov Sergey
*************************************************************************/
void rmatrixlq(ap::real_2d_array& a, int m, int n, ap::real_1d_array& tau);


/*************************************************************************
Partial unpacking of matrix Q from the LQ decomposition of a matrix A

Input parameters:
    A       -   matrices L and Q in compact form.
                Output of RMatrixLQ subroutine.
    M       -   number of rows in given matrix A. M>=0.
    N       -   number of columns in given matrix A. N>=0.
    Tau     -   scalar factors which are used to form Q.
                Output of the RMatrixLQ subroutine.
    QRows   -   required number of rows in matrix Q. N>=QRows>=0.

Output parameters:
    Q       -   first QRows rows of matrix Q. Array whose indexes range
                within [0..QRows-1, 0..N-1]. If QRows=0, the array remains
                unchanged.

  -- ALGLIB --
     Copyright 2005 by Bochkanov Sergey
*************************************************************************/
void rmatrixlqunpackq(const ap::real_2d_array& a,
     int m,
     int n,
     const ap::real_1d_array& tau,
     int qrows,
     ap::real_2d_array& q);


/*************************************************************************
Unpacking of matrix L from the LQ decomposition of a matrix A

Input parameters:
    A       -   matrices Q and L in compact form.
                Output of RMatrixLQ subroutine.
    M       -   number of rows in given matrix A. M>=0.
    N       -   number of columns in given matrix A. N>=0.

Output parameters:
    L       -   matrix L, array[0..M-1, 0..N-1].

  -- ALGLIB --
     Copyright 2005 by Bochkanov Sergey
*************************************************************************/
void rmatrixlqunpackl(const ap::real_2d_array& a,
     int m,
     int n,
     ap::real_2d_array& l);


void lqdecomposition(ap::real_2d_array& a,
     int m,
     int n,
     ap::real_1d_array& tau);


void unpackqfromlq(const ap::real_2d_array& a,
     int m,
     int n,
     const ap::real_1d_array& tau,
     int qrows,
     ap::real_2d_array& q);


void lqdecompositionunpacked(ap::real_2d_array a,
     int m,
     int n,
     ap::real_2d_array& l,
     ap::real_2d_array& q);


#endif

