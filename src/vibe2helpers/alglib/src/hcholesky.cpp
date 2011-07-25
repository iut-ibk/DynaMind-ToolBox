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
#include "hcholesky.h"

/*************************************************************************
Cholesky decomposition

The algorithm computes Cholesky decomposition  of  a  Hermitian  positive-
definite matrix.

The result of an algorithm is a representation of matrix A as A = U'*U  or
A = L*L' (here X' detones conj(X^T)).

Input parameters:
    A       -   upper or lower triangle of a factorized matrix.
                array with elements [0..N-1, 0..N-1].
    N       -   size of matrix A.
    IsUpper -   if IsUpper=True, then A contains an upper triangle of
                a symmetric matrix, otherwise A contains a lower one.

Output parameters:
    A       -   the result of factorization. If IsUpper=True, then
                the upper triangle contains matrix U, so that A = U'*U,
                and the elements below the main diagonal are not modified.
                Similarly, if IsUpper = False.

Result:
    If the matrix is positive-definite, the function returns True.
    Otherwise, the function returns False. This means that the
    factorization could not be carried out.

  -- LAPACK routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     February 29, 1992
*************************************************************************/
bool hmatrixcholesky(ap::complex_2d_array& a, int n, bool isupper)
{
    bool result;
    int j;
    double ajj;
    ap::complex v;
    double r;
    ap::complex_1d_array t;
    ap::complex_1d_array t2;
    ap::complex_1d_array t3;
    int i;
    ap::complex_2d_array a1;
    int i_;

    if( !isupper )
    {
        a1.setbounds(1, n, 1, n);
        for(i = 1; i <= n; i++)
        {
            for(j = 1; j <= n; j++)
            {
                a1(i,j) = a(i-1,j-1);
            }
        }
        result = hermitiancholeskydecomposition(a1, n, isupper);
        for(i = 1; i <= n; i++)
        {
            for(j = 1; j <= n; j++)
            {
                a(i-1,j-1) = a1(i,j);
            }
        }
        return result;
    }
    t.setbounds(0, n-1);
    t2.setbounds(0, n-1);
    t3.setbounds(0, n-1);
    result = true;
    if( n<0 )
    {
        result = false;
        return result;
    }
    
    //
    // Quick return if possible
    //
    if( n==0 )
    {
        return result;
    }
    if( isupper )
    {
        
        //
        // Compute the Cholesky factorization A = U'*U.
        //
        for(j = 0; j <= n-1; j++)
        {
            
            //
            // Compute U(J,J) and test for non-positive-definiteness.
            //
            v = 0.0;
            for(i_=0; i_<=j-1;i_++)
            {
                v += ap::conj(a(i_,j))*a(i_,j);
            }
            ajj = (a(j,j)-v).x;
            if( ap::fp_less_eq(ajj,0) )
            {
                a(j,j) = ajj;
                result = false;
                return result;
            }
            ajj = sqrt(ajj);
            a(j,j) = ajj;
            
            //
            // Compute elements J+1:N-1 of row J.
            //
            if( j<n-1 )
            {
                for(i_=0; i_<=j-1;i_++)
                {
                    t2(i_) = ap::conj(a(i_,j));
                }
                for(i_=j+1; i_<=n-1;i_++)
                {
                    t3(i_) = a(j,i_);
                }
                complexmatrixvectormultiply(a, 0, j-1, j+1, n-1, true, false, t2, 0, j-1, -1.0, t3, j+1, n-1, 1.0, t);
                for(i_=j+1; i_<=n-1;i_++)
                {
                    a(j,i_) = t3(i_);
                }
                r = 1/ajj;
                for(i_=j+1; i_<=n-1;i_++)
                {
                    a(j,i_) = r*a(j,i_);
                }
            }
        }
    }
    else
    {
        
        //
        // Compute the Cholesky factorization A = L*L'.
        //
        for(j = 0; j <= n-1; j++)
        {
            
            //
            // Compute L(J+1,J+1) and test for non-positive-definiteness.
            //
            v = 0.0;
            for(i_=0; i_<=j-1;i_++)
            {
                v += ap::conj(a(j,i_))*a(j,i_);
            }
            ajj = (a(j,j)-v).x;
            if( ap::fp_less_eq(ajj,0) )
            {
                a(j,j) = ajj;
                result = false;
                return result;
            }
            ajj = sqrt(ajj);
            a(j,j) = ajj;
            
            //
            // Compute elements J+1:N of column J.
            //
            if( j<n-1 )
            {
                for(i_=0; i_<=j-1;i_++)
                {
                    t2(i_) = ap::conj(a(j,i_));
                }
                for(i_=j+1; i_<=n-1;i_++)
                {
                    t3(i_) = a(i_,j);
                }
                complexmatrixvectormultiply(a, j+1, n-1, 0, j-1, false, false, t2, 0, j-1, -1.0, t3, j+1, n-1, 1.0, t);
                for(i_=j+1; i_<=n-1;i_++)
                {
                    a(i_,j) = t3(i_);
                }
                r = 1/ajj;
                for(i_=j+1; i_<=n-1;i_++)
                {
                    a(i_,j) = r*a(i_,j);
                }
            }
        }
    }
    return result;
}


bool hermitiancholeskydecomposition(ap::complex_2d_array& a,
     int n,
     bool isupper)
{
    bool result;
    int j;
    double ajj;
    ap::complex v;
    double r;
    ap::complex_1d_array t;
    ap::complex_1d_array t2;
    ap::complex_1d_array t3;
    int i_;

    t.setbounds(1, n);
    t2.setbounds(1, n);
    t3.setbounds(1, n);
    result = true;
    if( n<0 )
    {
        result = false;
        return result;
    }
    
    //
    // Quick return if possible
    //
    if( n==0 )
    {
        return result;
    }
    if( isupper )
    {
        
        //
        // Compute the Cholesky factorization A = U'*U.
        //
        for(j = 1; j <= n; j++)
        {
            
            //
            // Compute U(J,J) and test for non-positive-definiteness.
            //
            v = 0.0;
            for(i_=1; i_<=j-1;i_++)
            {
                v += ap::conj(a(i_,j))*a(i_,j);
            }
            ajj = (a(j,j)-v).x;
            if( ap::fp_less_eq(ajj,0) )
            {
                a(j,j) = ajj;
                result = false;
                return result;
            }
            ajj = sqrt(ajj);
            a(j,j) = ajj;
            
            //
            // Compute elements J+1:N of row J.
            //
            if( j<n )
            {
                for(i_=1; i_<=j-1;i_++)
                {
                    a(i_,j) = ap::conj(a(i_,j));
                }
                for(i_=1; i_<=j-1;i_++)
                {
                    t2(i_) = a(i_,j);
                }
                for(i_=j+1; i_<=n;i_++)
                {
                    t3(i_) = a(j,i_);
                }
                complexmatrixvectormultiply(a, 1, j-1, j+1, n, true, false, t2, 1, j-1, -1.0, t3, j+1, n, 1.0, t);
                for(i_=j+1; i_<=n;i_++)
                {
                    a(j,i_) = t3(i_);
                }
                for(i_=1; i_<=j-1;i_++)
                {
                    a(i_,j) = ap::conj(a(i_,j));
                }
                r = 1/ajj;
                for(i_=j+1; i_<=n;i_++)
                {
                    a(j,i_) = r*a(j,i_);
                }
            }
        }
    }
    else
    {
        
        //
        // Compute the Cholesky factorization A = L*L'.
        //
        for(j = 1; j <= n; j++)
        {
            
            //
            // Compute L(J,J) and test for non-positive-definiteness.
            //
            v = 0.0;
            for(i_=1; i_<=j-1;i_++)
            {
                v += ap::conj(a(j,i_))*a(j,i_);
            }
            ajj = (a(j,j)-v).x;
            if( ap::fp_less_eq(ajj,0) )
            {
                a(j,j) = ajj;
                result = false;
                return result;
            }
            ajj = sqrt(ajj);
            a(j,j) = ajj;
            
            //
            // Compute elements J+1:N of column J.
            //
            if( j<n )
            {
                for(i_=1; i_<=j-1;i_++)
                {
                    a(j,i_) = ap::conj(a(j,i_));
                }
                for(i_=1; i_<=j-1;i_++)
                {
                    t2(i_) = a(j,i_);
                }
                for(i_=j+1; i_<=n;i_++)
                {
                    t3(i_) = a(i_,j);
                }
                complexmatrixvectormultiply(a, j+1, n, 1, j-1, false, false, t2, 1, j-1, -1.0, t3, j+1, n, 1.0, t);
                for(i_=j+1; i_<=n;i_++)
                {
                    a(i_,j) = t3(i_);
                }
                for(i_=1; i_<=j-1;i_++)
                {
                    a(j,i_) = ap::conj(a(j,i_));
                }
                r = 1/ajj;
                for(i_=j+1; i_<=n;i_++)
                {
                    a(i_,j) = r*a(i_,j);
                }
            }
        }
    }
    return result;
}




