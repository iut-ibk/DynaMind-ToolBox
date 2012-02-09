/*************************************************************************
Copyright (c) 1992-2007 The University of Tennessee. All rights reserved.

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
#include "clu.h"

/*************************************************************************
LU decomposition of a complex general matrix of size MxN

The subroutine calculates the LU decomposition of a rectangular general
matrix with partial pivoting (with row permutations).

Input parameters:
    A   -   matrix A whose indexes range within [0..M-1, 0..N-1].
    M   -   number of rows in matrix A.
    N   -   number of columns in matrix A.

Output parameters:
    A   -   matrices L and U in compact form (see below).
            Array whose indexes range within [0..M-1, 0..N-1].
    Pivots - permutation matrix in compact form (see below).
            Array whose index ranges within [0..Min(M-1,N-1)].

Matrix A is represented as A = P * L * U, where P is a permutation matrix,
matrix L - lower triangular (or lower trapezoid, if M>N) matrix,
U - upper triangular (or upper trapezoid, if M<N) matrix.

Let M be equal to 4 and N be equal to 3:

                   (  1          )    ( U11 U12 U13  )
A = P1 * P2 * P3 * ( L21  1      )  * (     U22 U23  )
                   ( L31 L32  1  )    (         U33  )
                   ( L41 L42 L43 )

Matrix L has size MxMin(M,N), matrix U has size Min(M,N)xN, matrix P(i) is
a permutation of the identity matrix of size MxM with numbers I and Pivots[I].

The algorithm returns array Pivots and the following matrix which replaces
matrix A and contains matrices L and U in compact form (the example applies
to M=4, N=3).

 ( U11 U12 U13 )
 ( L21 U22 U23 )
 ( L31 L32 U33 )
 ( L41 L42 L43 )

As we can see, the unit diagonal isn't stored.

  -- LAPACK routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     June 30, 1992
*************************************************************************/
void cmatrixlu(ap::complex_2d_array& a,
     int m,
     int n,
     ap::integer_1d_array& pivots)
{
    int i;
    int j;
    int jp;
    ap::complex_1d_array t1;
    ap::complex s;
    int i_;

    pivots.setbounds(0, ap::minint(m-1, n-1));
    t1.setbounds(0, ap::maxint(m-1, n-1));
    ap::ap_error::make_assertion(m>=0&&n>=0, "Error in LUDecomposition: incorrect function arguments");
    
    //
    // Quick return if possible
    //
    if( m==0||n==0 )
    {
        return;
    }
    for(j = 0; j <= ap::minint(m-1, n-1); j++)
    {
        
        //
        // Find pivot and test for singularity.
        //
        jp = j;
        for(i = j+1; i <= m-1; i++)
        {
            if( ap::fp_greater(ap::abscomplex(a(i,j)),ap::abscomplex(a(jp,j))) )
            {
                jp = i;
            }
        }
        pivots(j) = jp;
        if( a(jp,j)!=0 )
        {
            
            //
            //Apply the interchange to rows
            //
            if( jp!=j )
            {
                for(i_=0; i_<=n-1;i_++)
                {
                    t1(i_) = a(j,i_);
                }
                for(i_=0; i_<=n-1;i_++)
                {
                    a(j,i_) = a(jp,i_);
                }
                for(i_=0; i_<=n-1;i_++)
                {
                    a(jp,i_) = t1(i_);
                }
            }
            
            //
            //Compute elements J+1:M of J-th column.
            //
            if( j<m )
            {
                jp = j+1;
                s = 1/a(j,j);
                for(i_=jp; i_<=m-1;i_++)
                {
                    a(i_,j) = s*a(i_,j);
                }
            }
        }
        if( j<ap::minint(m, n)-1 )
        {
            
            //
            //Update trailing submatrix.
            //
            jp = j+1;
            for(i = j+1; i <= m-1; i++)
            {
                s = a(i,j);
                for(i_=jp; i_<=n-1;i_++)
                {
                    a(i,i_) = a(i,i_) - s*a(j,i_);
                }
            }
        }
    }
}


void complexludecomposition(ap::complex_2d_array& a,
     int m,
     int n,
     ap::integer_1d_array& pivots)
{
    int i;
    int j;
    int jp;
    ap::complex_1d_array t1;
    ap::complex s;
    int i_;

    pivots.setbounds(1, ap::minint(m, n));
    t1.setbounds(1, ap::maxint(m, n));
    ap::ap_error::make_assertion(m>=0&&n>=0, "Error in ComplexLUDecomposition: incorrect function arguments");
    
    //
    // Quick return if possible
    //
    if( m==0||n==0 )
    {
        return;
    }
    for(j = 1; j <= ap::minint(m, n); j++)
    {
        
        //
        // Find pivot and test for singularity.
        //
        jp = j;
        for(i = j+1; i <= m; i++)
        {
            if( ap::fp_greater(ap::abscomplex(a(i,j)),ap::abscomplex(a(jp,j))) )
            {
                jp = i;
            }
        }
        pivots(j) = jp;
        if( a(jp,j)!=0 )
        {
            
            //
            //Apply the interchange to rows
            //
            if( jp!=j )
            {
                for(i_=1; i_<=n;i_++)
                {
                    t1(i_) = a(j,i_);
                }
                for(i_=1; i_<=n;i_++)
                {
                    a(j,i_) = a(jp,i_);
                }
                for(i_=1; i_<=n;i_++)
                {
                    a(jp,i_) = t1(i_);
                }
            }
            
            //
            //Compute elements J+1:M of J-th column.
            //
            if( j<m )
            {
                jp = j+1;
                s = 1/a(j,j);
                for(i_=jp; i_<=m;i_++)
                {
                    a(i_,j) = s*a(i_,j);
                }
            }
        }
        if( j<ap::minint(m, n) )
        {
            
            //
            //Update trailing submatrix.
            //
            jp = j+1;
            for(i = j+1; i <= m; i++)
            {
                s = a(i,j);
                for(i_=jp; i_<=n;i_++)
                {
                    a(i,i_) = a(i,i_) - s*a(j,i_);
                }
            }
        }
    }
}


void complexludecompositionunpacked(ap::complex_2d_array a,
     int m,
     int n,
     ap::complex_2d_array& l,
     ap::complex_2d_array& u,
     ap::integer_1d_array& pivots)
{
    int i;
    int j;
    int minmn;

    if( m==0||n==0 )
    {
        return;
    }
    minmn = ap::minint(m, n);
    l.setbounds(1, m, 1, minmn);
    u.setbounds(1, minmn, 1, n);
    complexludecomposition(a, m, n, pivots);
    for(i = 1; i <= m; i++)
    {
        for(j = 1; j <= minmn; j++)
        {
            if( j>i )
            {
                l(i,j) = 0;
            }
            if( j==i )
            {
                l(i,j) = 1;
            }
            if( j<i )
            {
                l(i,j) = a(i,j);
            }
        }
    }
    for(i = 1; i <= minmn; i++)
    {
        for(j = 1; j <= n; j++)
        {
            if( j<i )
            {
                u(i,j) = 0;
            }
            if( j>=i )
            {
                u(i,j) = a(i,j);
            }
        }
    }
}




