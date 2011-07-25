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
#include "spdinverse.h"

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
bool spdmatrixcholeskyinverse(ap::real_2d_array& a, int n, bool isupper)
{
    bool result;
    int i;
    int j;
    int k;
    double v;
    double ajj;
    double aii;
    ap::real_1d_array t;
    ap::real_2d_array a1;

    result = true;
    
    //
    // Test the input parameters.
    //
    t.setbounds(0, n-1);
    if( isupper )
    {
        
        //
        // Compute inverse of upper triangular matrix.
        //
        for(j = 0; j <= n-1; j++)
        {
            if( ap::fp_eq(a(j,j),0) )
            {
                result = false;
                return result;
            }
            a(j,j) = 1/a(j,j);
            ajj = -a(j,j);
            
            //
            // Compute elements 1:j-1 of j-th column.
            //
            ap::vmove(t.getvector(0, j-1), a.getcolumn(j, 0, j-1));
            for(i = 0; i <= j-1; i++)
            {
                v = ap::vdotproduct(&a(i, i), &t(i), ap::vlen(i,j-1));
                a(i,j) = v;
            }
            ap::vmul(a.getcolumn(j, 0, j-1), ajj);
        }
        
        //
        // InvA = InvU * InvU'
        //
        for(i = 0; i <= n-1; i++)
        {
            aii = a(i,i);
            if( i<n-1 )
            {
                v = ap::vdotproduct(&a(i, i), &a(i, i), ap::vlen(i,n-1));
                a(i,i) = v;
                for(k = 0; k <= i-1; k++)
                {
                    v = ap::vdotproduct(&a(k, i+1), &a(i, i+1), ap::vlen(i+1,n-1));
                    a(k,i) = a(k,i)*aii+v;
                }
            }
            else
            {
                ap::vmul(a.getcolumn(i, 0, i), aii);
            }
        }
    }
    else
    {
        
        //
        // Compute inverse of lower triangular matrix.
        //
        for(j = n-1; j >= 0; j--)
        {
            if( ap::fp_eq(a(j,j),0) )
            {
                result = false;
                return result;
            }
            a(j,j) = 1/a(j,j);
            ajj = -a(j,j);
            if( j<n-1 )
            {
                
                //
                // Compute elements j+1:n of j-th column.
                //
                ap::vmove(t.getvector(j+1, n-1), a.getcolumn(j, j+1, n-1));
                for(i = j+1+1; i <= n; i++)
                {
                    v = ap::vdotproduct(&a(i-1, j+1), &t(j+1), ap::vlen(j+1,i-1));
                    a(i-1,j) = v;
                }
                ap::vmul(a.getcolumn(j, j+1, n-1), ajj);
            }
        }
        
        //
        // InvA = InvL' * InvL
        //
        for(i = 0; i <= n-1; i++)
        {
            aii = a(i,i);
            if( i<n-1 )
            {
                v = ap::vdotproduct(a.getcolumn(i, i, n-1), a.getcolumn(i, i, n-1));
                a(i,i) = v;
                for(k = 0; k <= i-1; k++)
                {
                    v = ap::vdotproduct(a.getcolumn(k, i+1, n-1), a.getcolumn(i, i+1, n-1));
                    a(i,k) = aii*a(i,k)+v;
                }
            }
            else
            {
                ap::vmul(&a(i, 0), ap::vlen(0,i), aii);
            }
        }
    }
    return result;
}


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
bool spdmatrixinverse(ap::real_2d_array& a, int n, bool isupper)
{
    bool result;

    result = false;
    if( spdmatrixcholesky(a, n, isupper) )
    {
        if( spdmatrixcholeskyinverse(a, n, isupper) )
        {
            result = true;
        }
    }
    return result;
}


bool inversecholesky(ap::real_2d_array& a, int n, bool isupper)
{
    bool result;
    int i;
    int j;
    int k;
    int nmj;
    int jm1;
    int jp1;
    int ip1;
    double v;
    double ajj;
    double aii;
    ap::real_1d_array t;
    ap::real_1d_array d;

    result = true;
    
    //
    // Test the input parameters.
    //
    t.setbounds(1, n);
    d.setbounds(1, n);
    if( isupper )
    {
        
        //
        // Compute inverse of upper triangular matrix.
        //
        for(j = 1; j <= n; j++)
        {
            if( ap::fp_eq(a(j,j),0) )
            {
                result = false;
                return result;
            }
            jm1 = j-1;
            a(j,j) = 1/a(j,j);
            ajj = -a(j,j);
            
            //
            // Compute elements 1:j-1 of j-th column.
            //
            ap::vmove(t.getvector(1, jm1), a.getcolumn(j, 1, jm1));
            for(i = 1; i <= j-1; i++)
            {
                v = ap::vdotproduct(a.getrow(i, i, jm1), a.getcolumn(j, i, jm1));
                a(i,j) = v;
            }
            ap::vmul(a.getcolumn(j, 1, jm1), ajj);
        }
        
        //
        // InvA = InvU * InvU'
        //
        for(i = 1; i <= n; i++)
        {
            aii = a(i,i);
            if( i<n )
            {
                v = ap::vdotproduct(&a(i, i), &a(i, i), ap::vlen(i,n));
                a(i,i) = v;
                ip1 = i+1;
                for(k = 1; k <= i-1; k++)
                {
                    v = ap::vdotproduct(&a(k, ip1), &a(i, ip1), ap::vlen(ip1,n));
                    a(k,i) = a(k,i)*aii+v;
                }
            }
            else
            {
                ap::vmul(a.getcolumn(i, 1, i), aii);
            }
        }
    }
    else
    {
        
        //
        // Compute inverse of lower triangular matrix.
        //
        for(j = n; j >= 1; j--)
        {
            if( ap::fp_eq(a(j,j),0) )
            {
                result = false;
                return result;
            }
            a(j,j) = 1/a(j,j);
            ajj = -a(j,j);
            if( j<n )
            {
                
                //
                // Compute elements j+1:n of j-th column.
                //
                nmj = n-j;
                jp1 = j+1;
                ap::vmove(t.getvector(jp1, n), a.getcolumn(j, jp1, n));
                for(i = j+1; i <= n; i++)
                {
                    v = ap::vdotproduct(&a(i, jp1), &t(jp1), ap::vlen(jp1,i));
                    a(i,j) = v;
                }
                ap::vmul(a.getcolumn(j, jp1, n), ajj);
            }
        }
        
        //
        // InvA = InvL' * InvL
        //
        for(i = 1; i <= n; i++)
        {
            aii = a(i,i);
            if( i<n )
            {
                v = ap::vdotproduct(a.getcolumn(i, i, n), a.getcolumn(i, i, n));
                a(i,i) = v;
                ip1 = i+1;
                for(k = 1; k <= i-1; k++)
                {
                    v = ap::vdotproduct(a.getcolumn(k, ip1, n), a.getcolumn(i, ip1, n));
                    a(i,k) = aii*a(i,k)+v;
                }
            }
            else
            {
                ap::vmul(&a(i, 1), ap::vlen(1,i), aii);
            }
        }
    }
    return result;
}


bool inversesymmetricpositivedefinite(ap::real_2d_array& a,
     int n,
     bool isupper)
{
    bool result;

    result = false;
    if( choleskydecomposition(a, n, isupper) )
    {
        if( inversecholesky(a, n, isupper) )
        {
            result = true;
        }
    }
    return result;
}




