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

#include <stdafx.h>
#include "spdsolve.h"

/*************************************************************************
Solving a system of linear equations with a system  matrix  given  by  its
Cholesky decomposition.

The algorithm solves systems with a square matrix only.

Input parameters:
    A       -   Cholesky decomposition of a system matrix (the result of
                the SMatrixCholesky subroutine).
    B       -   right side of a system.
                Array whose index ranges within [0..N-1].
    N       -   size of matrix A.
    IsUpper -   points to the triangle of matrix A in which the Cholesky
                decomposition is stored. If IsUpper=True,  the  Cholesky
                decomposition has the form of U'*U, and the upper triangle
                of matrix A stores matrix U (in  that  case,  the  lower
                triangle isn’t used and isn’t changed by the subroutine)
                Similarly, if IsUpper = False, the Cholesky decomposition
                has the form of L*L',  and  the  lower  triangle  stores
                matrix L.

Output parameters:
    X       -   solution of a system.
                Array whose index ranges within [0..N-1].

Result:
    True, if the system is not singular. X contains the solution.
    False, if the system is singular (there is a zero element on the main
diagonal). In this case, X doesn't contain a solution.

  -- ALGLIB --
     Copyright 2005-2008 by Bochkanov Sergey
*************************************************************************/
bool spdmatrixcholeskysolve(const ap::real_2d_array& a,
     ap::real_1d_array b,
     int n,
     bool isupper,
     ap::real_1d_array& x)
{
    bool result;
    int i;
    double v;

    ap::ap_error::make_assertion(n>0, "Error: N<=0 in SolveSystemCholesky");
    
    //
    // det(A)=0?
    //
    result = true;
    for(i = 0; i <= n-1; i++)
    {
        if( ap::fp_eq(a(i,i),0) )
        {
            result = false;
            return result;
        }
    }
    
    //
    // det(A)<>0
    //
    x.setbounds(0, n-1);
    if( isupper )
    {
        
        //
        // A = U'*U, solve U'*y = b first
        //
        b(0) = b(0)/a(0,0);
        for(i = 1; i <= n-1; i++)
        {
            v = ap::vdotproduct(a.getcolumn(i, 0, i-1), b.getvector(0, i-1));
            b(i) = (b(i)-v)/a(i,i);
        }
        
        //
        // Solve U*x = y
        //
        b(n-1) = b(n-1)/a(n-1,n-1);
        for(i = n-2; i >= 0; i--)
        {
            v = ap::vdotproduct(&a(i, i+1), &b(i+1), ap::vlen(i+1,n-1));
            b(i) = (b(i)-v)/a(i,i);
        }
        ap::vmove(&x(0), &b(0), ap::vlen(0,n-1));
    }
    else
    {
        
        //
        // A = L*L', solve L'*y = b first
        //
        b(0) = b(0)/a(0,0);
        for(i = 1; i <= n-1; i++)
        {
            v = ap::vdotproduct(&a(i, 0), &b(0), ap::vlen(0,i-1));
            b(i) = (b(i)-v)/a(i,i);
        }
        
        //
        // Solve L'*x = y
        //
        b(n-1) = b(n-1)/a(n-1,n-1);
        for(i = n-2; i >= 0; i--)
        {
            v = ap::vdotproduct(a.getcolumn(i, i+1, n-1), b.getvector(i+1, n-1));
            b(i) = (b(i)-v)/a(i,i);
        }
        ap::vmove(&x(0), &b(0), ap::vlen(0,n-1));
    }
    return result;
}


/*************************************************************************
Solving a system of linear equations with  a  symmetric  positive-definite
matrix by using the Cholesky decomposition.

The algorithm solves a system of linear equations whose matrix is symmetric
and positive-definite.

Input parameters:
    A       -   upper or lower triangle part of a symmetric system matrix.
                Array whose indexes range within [0..N-1, 0..N-1].
    B       -   right side of a system.
                Array whose index ranges within [0..N-1].
    N       -   size of matrix A.
    IsUpper -   points to the triangle of matrix A in which the matrix is stored.

Output parameters:
    X       -   solution of a system.
                Array whose index ranges within [0..N-1].

Result:
    True, if the system is not singular.
    False, if the system is singular. In this case, X doesn't contain a
solution.

  -- ALGLIB --
     Copyright 2005-2008 by Bochkanov Sergey
*************************************************************************/
bool spdmatrixsolve(ap::real_2d_array a,
     ap::real_1d_array b,
     int n,
     bool isupper,
     ap::real_1d_array& x)
{
    bool result;

    result = spdmatrixcholesky(a, n, isupper);
    if( !result )
    {
        return result;
    }
    result = spdmatrixcholeskysolve(a, b, n, isupper, x);
    return result;
}


bool solvesystemcholesky(const ap::real_2d_array& a,
     ap::real_1d_array b,
     int n,
     bool isupper,
     ap::real_1d_array& x)
{
    bool result;
    int i;
    int im1;
    int ip1;
    double v;

    ap::ap_error::make_assertion(n>0, "Error: N<=0 in SolveSystemCholesky");
    
    //
    // det(A)=0?
    //
    result = true;
    for(i = 1; i <= n; i++)
    {
        if( ap::fp_eq(a(i,i),0) )
        {
            result = false;
            return result;
        }
    }
    
    //
    // det(A)<>0
    //
    x.setbounds(1, n);
    if( isupper )
    {
        
        //
        // A = U'*U, solve U'*y = b first
        //
        b(1) = b(1)/a(1,1);
        for(i = 2; i <= n; i++)
        {
            im1 = i-1;
            v = ap::vdotproduct(a.getcolumn(i, 1, im1), b.getvector(1, im1));
            b(i) = (b(i)-v)/a(i,i);
        }
        
        //
        // Solve U*x = y
        //
        b(n) = b(n)/a(n,n);
        for(i = n-1; i >= 1; i--)
        {
            ip1 = i+1;
            v = ap::vdotproduct(&a(i, ip1), &b(ip1), ap::vlen(ip1,n));
            b(i) = (b(i)-v)/a(i,i);
        }
        ap::vmove(&x(1), &b(1), ap::vlen(1,n));
    }
    else
    {
        
        //
        // A = L*L', solve L'*y = b first
        //
        b(1) = b(1)/a(1,1);
        for(i = 2; i <= n; i++)
        {
            im1 = i-1;
            v = ap::vdotproduct(&a(i, 1), &b(1), ap::vlen(1,im1));
            b(i) = (b(i)-v)/a(i,i);
        }
        
        //
        // Solve L'*x = y
        //
        b(n) = b(n)/a(n,n);
        for(i = n-1; i >= 1; i--)
        {
            ip1 = i+1;
            v = ap::vdotproduct(a.getcolumn(i, ip1, n), b.getvector(ip1, n));
            b(i) = (b(i)-v)/a(i,i);
        }
        ap::vmove(&x(1), &b(1), ap::vlen(1,n));
    }
    return result;
}


bool solvespdsystem(ap::real_2d_array a,
     ap::real_1d_array b,
     int n,
     bool isupper,
     ap::real_1d_array& x)
{
    bool result;

    result = choleskydecomposition(a, n, isupper);
    if( !result )
    {
        return result;
    }
    result = solvesystemcholesky(a, b, n, isupper, x);
    return result;
}




