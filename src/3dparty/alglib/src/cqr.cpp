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
#include "cqr.h"

/*************************************************************************
QR decomposition of a rectangular complex matrix of size MxN

Input parameters:
    A   -   matrix A whose indexes range within [0..M-1, 0..N-1]
    M   -   number of rows in matrix A.
    N   -   number of columns in matrix A.

Output parameters:
    A   -   matrices Q and R in compact form
    Tau -   array of scalar factors which are used to form matrix Q. Array
            whose indexes range within [0.. Min(M,N)-1]

Matrix A is represented as A = QR, where Q is an orthogonal matrix of size
MxM, R - upper triangular (or upper trapezoid) matrix of size MxN.

  -- LAPACK routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     September 30, 1994
*************************************************************************/
void cmatrixqr(ap::complex_2d_array& a,
     int m,
     int n,
     ap::complex_1d_array& tau)
{
    ap::complex_1d_array work;
    ap::complex_1d_array t;
    int i;
    int k;
    int mmi;
    int minmn;
    ap::complex tmp;
    int i_;
    int i1_;

    minmn = ap::minint(m, n);
    if( minmn<=0 )
    {
        return;
    }
    work.setbounds(0, n-1);
    t.setbounds(1, m);
    tau.setbounds(0, minmn-1);
    
    //
    // Test the input arguments
    //
    k = ap::minint(m, n);
    for(i = 0; i <= k-1; i++)
    {
        
        //
        // Generate elementary reflector H(i) to annihilate A(i+1:m,i)
        //
        mmi = m-i;
        i1_ = (i) - (1);
        for(i_=1; i_<=mmi;i_++)
        {
            t(i_) = a(i_+i1_,i);
        }
        complexgeneratereflection(t, mmi, tmp);
        tau(i) = tmp;
        i1_ = (1) - (i);
        for(i_=i; i_<=m-1;i_++)
        {
            a(i_,i) = t(i_+i1_);
        }
        t(1) = 1;
        if( i<n-1 )
        {
            
            //
            // Apply H'(i) to A(i:m,i+1:n) from the left
            //
            complexapplyreflectionfromtheleft(a, ap::conj(tau(i)), t, i, m-1, i+1, n-1, work);
        }
    }
}


/*************************************************************************
Partial unpacking of matrix Q from QR decomposition of a complex matrix A.

Input parameters:
    QR          -   matrices Q and R in compact form.
                    Output of CMatrixQR subroutine .
    M           -   number of rows in matrix A. M>=0.
    N           -   number of rows in matrix A. N>=0.
    Tau         -   scalar factors which are used to form Q.
                    Output of CMatrixQR subroutine .
    QColumns    -   required number of columns in matrix Q. M>=QColumns>=0.

Output parameters:
    Q           -   first QColumns columns of matrix Q.
                    Array whose index ranges within [0..M-1, 0..QColumns-1].
                    If QColumns=0, array isn't changed.

  -- ALGLIB --
     Copyright 2005 by Bochkanov Sergey
*************************************************************************/
void cmatrixqrunpackq(const ap::complex_2d_array& qr,
     int m,
     int n,
     const ap::complex_1d_array& tau,
     int qcolumns,
     ap::complex_2d_array& q)
{
    int i;
    int j;
    int k;
    int minmn;
    ap::complex_1d_array v;
    ap::complex_1d_array work;
    int vm;
    int i_;
    int i1_;

    ap::ap_error::make_assertion(qcolumns<=m, "UnpackQFromQR: QColumns>M!");
    if( m<=0||n<=0||qcolumns<=0 )
    {
        return;
    }
    
    //
    // init
    //
    minmn = ap::minint(m, n);
    k = ap::minint(minmn, qcolumns);
    q.setbounds(0, m-1, 0, qcolumns-1);
    v.setbounds(1, m);
    work.setbounds(0, qcolumns-1);
    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= qcolumns-1; j++)
        {
            if( i==j )
            {
                q(i,j) = 1;
            }
            else
            {
                q(i,j) = 0;
            }
        }
    }
    
    //
    // unpack Q
    //
    for(i = k-1; i >= 0; i--)
    {
        
        //
        // Apply H(i)
        //
        vm = m-i;
        i1_ = (i) - (1);
        for(i_=1; i_<=vm;i_++)
        {
            v(i_) = qr(i_+i1_,i);
        }
        v(1) = 1;
        complexapplyreflectionfromtheleft(q, tau(i), v, i, m-1, 0, qcolumns-1, work);
    }
}


/*************************************************************************
Unpacking of matrix R from the QR decomposition of a matrix A

Input parameters:
    A       -   matrices Q and R in compact form.
                Output of CMatrixQR subroutine.
    M       -   number of rows in given matrix A. M>=0.
    N       -   number of columns in given matrix A. N>=0.

Output parameters:
    R       -   matrix R, array[0..M-1, 0..N-1].

  -- ALGLIB --
     Copyright 2005 by Bochkanov Sergey
*************************************************************************/
void cmatrixqrunpackr(const ap::complex_2d_array& a,
     int m,
     int n,
     ap::complex_2d_array& r)
{
    int i;
    int k;
    int i_;

    if( m<=0||n<=0 )
    {
        return;
    }
    k = ap::minint(m, n);
    r.setbounds(0, m-1, 0, n-1);
    for(i = 0; i <= n-1; i++)
    {
        r(0,i) = 0;
    }
    for(i = 1; i <= m-1; i++)
    {
        for(i_=0; i_<=n-1;i_++)
        {
            r(i,i_) = r(0,i_);
        }
    }
    for(i = 0; i <= k-1; i++)
    {
        for(i_=i; i_<=n-1;i_++)
        {
            r(i,i_) = a(i,i_);
        }
    }
}


void complexqrdecomposition(ap::complex_2d_array& a,
     int m,
     int n,
     ap::complex_1d_array& tau)
{
    ap::complex_1d_array work;
    ap::complex_1d_array t;
    int i;
    int k;
    int mmip1;
    int minmn;
    ap::complex tmp;
    int i_;
    int i1_;

    minmn = ap::minint(m, n);
    work.setbounds(1, n);
    t.setbounds(1, m);
    tau.setbounds(1, minmn);
    
    //
    // Test the input arguments
    //
    k = ap::minint(m, n);
    for(i = 1; i <= k; i++)
    {
        
        //
        // Generate elementary reflector H(i) to annihilate A(i+1:m,i)
        //
        mmip1 = m-i+1;
        i1_ = (i) - (1);
        for(i_=1; i_<=mmip1;i_++)
        {
            t(i_) = a(i_+i1_,i);
        }
        complexgeneratereflection(t, mmip1, tmp);
        tau(i) = tmp;
        i1_ = (1) - (i);
        for(i_=i; i_<=m;i_++)
        {
            a(i_,i) = t(i_+i1_);
        }
        t(1) = 1;
        if( i<n )
        {
            
            //
            // Apply H'(i) to A(i:m,i+1:n) from the left
            //
            complexapplyreflectionfromtheleft(a, ap::conj(tau(i)), t, i, m, i+1, n, work);
        }
    }
}


void complexunpackqfromqr(const ap::complex_2d_array& qr,
     int m,
     int n,
     const ap::complex_1d_array& tau,
     int qcolumns,
     ap::complex_2d_array& q)
{
    int i;
    int j;
    int k;
    int minmn;
    ap::complex_1d_array v;
    ap::complex_1d_array work;
    int vm;
    int i_;
    int i1_;

    ap::ap_error::make_assertion(qcolumns<=m, "UnpackQFromQR: QColumns>M!");
    if( m==0||n==0||qcolumns==0 )
    {
        return;
    }
    
    //
    // init
    //
    minmn = ap::minint(m, n);
    k = ap::minint(minmn, qcolumns);
    q.setbounds(1, m, 1, qcolumns);
    v.setbounds(1, m);
    work.setbounds(1, qcolumns);
    for(i = 1; i <= m; i++)
    {
        for(j = 1; j <= qcolumns; j++)
        {
            if( i==j )
            {
                q(i,j) = 1;
            }
            else
            {
                q(i,j) = 0;
            }
        }
    }
    
    //
    // unpack Q
    //
    for(i = k; i >= 1; i--)
    {
        
        //
        // Apply H(i)
        //
        vm = m-i+1;
        i1_ = (i) - (1);
        for(i_=1; i_<=vm;i_++)
        {
            v(i_) = qr(i_+i1_,i);
        }
        v(1) = 1;
        complexapplyreflectionfromtheleft(q, tau(i), v, i, m, 1, qcolumns, work);
    }
}


void complexqrdecompositionunpacked(ap::complex_2d_array a,
     int m,
     int n,
     ap::complex_2d_array& q,
     ap::complex_2d_array& r)
{
    int i;
    int j;
    int k;
    int l;
    int vm;
    ap::complex_1d_array tau;
    ap::complex_1d_array work;
    ap::complex_1d_array v;
    double tmp;
    int i_;

    k = ap::minint(m, n);
    if( n<=0 )
    {
        return;
    }
    work.setbounds(1, m);
    v.setbounds(1, m);
    q.setbounds(1, m, 1, m);
    r.setbounds(1, m, 1, n);
    
    //
    // QRDecomposition
    //
    complexqrdecomposition(a, m, n, tau);
    
    //
    // R
    //
    for(i = 1; i <= n; i++)
    {
        r(1,i) = 0;
    }
    for(i = 2; i <= m; i++)
    {
        for(i_=1; i_<=n;i_++)
        {
            r(i,i_) = r(1,i_);
        }
    }
    for(i = 1; i <= k; i++)
    {
        for(i_=i; i_<=n;i_++)
        {
            r(i,i_) = a(i,i_);
        }
    }
    
    //
    // Q
    //
    complexunpackqfromqr(a, m, n, tau, m, q);
}




