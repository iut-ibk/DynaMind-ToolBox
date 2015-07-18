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
#include "inv.h"

/*************************************************************************
Inversion of a matrix given by its LU decomposition.

Input parameters:
    A       -   LU decomposition of the matrix (output of RMatrixLU subroutine).
    Pivots  -   table of permutations which were made during the LU decomposition
                (the output of RMatrixLU subroutine).
    N       -   size of matrix A.

Output parameters:
    A       -   inverse of matrix A.
                Array whose indexes range within [0..N-1, 0..N-1].

Result:
    True, if the matrix is not singular.
    False, if the matrix is singular.

  -- LAPACK routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     February 29, 1992
*************************************************************************/
bool rmatrixluinverse(ap::real_2d_array& a,
     const ap::integer_1d_array& pivots,
     int n)
{
    bool result;
    ap::real_1d_array work;
    int i;
    int iws;
    int j;
    int jb;
    int jj;
    int jp;
    double v;

    result = true;
    
    //
    // Quick return if possible
    //
    if( n==0 )
    {
        return result;
    }
    work.setbounds(0, n-1);
    
    //
    // Form inv(U)
    //
    if( !rmatrixtrinverse(a, n, true, false) )
    {
        result = false;
        return result;
    }
    
    //
    // Solve the equation inv(A)*L = inv(U) for inv(A).
    //
    for(j = n-1; j >= 0; j--)
    {
        
        //
        // Copy current column of L to WORK and replace with zeros.
        //
        for(i = j+1; i <= n-1; i++)
        {
            work(i) = a(i,j);
            a(i,j) = 0;
        }
        
        //
        // Compute current column of inv(A).
        //
        if( j<n-1 )
        {
            for(i = 0; i <= n-1; i++)
            {
                v = ap::vdotproduct(&a(i, j+1), &work(j+1), ap::vlen(j+1,n-1));
                a(i,j) = a(i,j)-v;
            }
        }
    }
    
    //
    // Apply column interchanges.
    //
    for(j = n-2; j >= 0; j--)
    {
        jp = pivots(j);
        if( jp!=j )
        {
            ap::vmove(work.getvector(0, n-1), a.getcolumn(j, 0, n-1));
            ap::vmove(a.getcolumn(j, 0, n-1), a.getcolumn(jp, 0, n-1));
            ap::vmove(a.getcolumn(jp, 0, n-1), work.getvector(0, n-1));
        }
    }
    return result;
}


/*************************************************************************
Inversion of a general matrix.

Input parameters:
    A   -   matrix. Array whose indexes range within [0..N-1, 0..N-1].
    N   -   size of matrix A.

Output parameters:
    A   -   inverse of matrix A.
            Array whose indexes range within [0..N-1, 0..N-1].

Result:
    True, if the matrix is not singular.
    False, if the matrix is singular.

  -- ALGLIB --
     Copyright 2005 by Bochkanov Sergey
*************************************************************************/
bool rmatrixinverse(ap::real_2d_array& a, int n)
{
    bool result;
    ap::integer_1d_array pivots;

    rmatrixlu(a, n, n, pivots);
    result = rmatrixluinverse(a, pivots, n);
    return result;
}


bool inverselu(ap::real_2d_array& a,
     const ap::integer_1d_array& pivots,
     int n)
{
    bool result;
    ap::real_1d_array work;
    int i;
    int iws;
    int j;
    int jb;
    int jj;
    int jp;
    int jp1;
    double v;

    result = true;
    
    //
    // Quick return if possible
    //
    if( n==0 )
    {
        return result;
    }
    work.setbounds(1, n);
    
    //
    // Form inv(U)
    //
    if( !invtriangular(a, n, true, false) )
    {
        result = false;
        return result;
    }
    
    //
    // Solve the equation inv(A)*L = inv(U) for inv(A).
    //
    for(j = n; j >= 1; j--)
    {
        
        //
        // Copy current column of L to WORK and replace with zeros.
        //
        for(i = j+1; i <= n; i++)
        {
            work(i) = a(i,j);
            a(i,j) = 0;
        }
        
        //
        // Compute current column of inv(A).
        //
        if( j<n )
        {
            jp1 = j+1;
            for(i = 1; i <= n; i++)
            {
                v = ap::vdotproduct(&a(i, jp1), &work(jp1), ap::vlen(jp1,n));
                a(i,j) = a(i,j)-v;
            }
        }
    }
    
    //
    // Apply column interchanges.
    //
    for(j = n-1; j >= 1; j--)
    {
        jp = pivots(j);
        if( jp!=j )
        {
            ap::vmove(work.getvector(1, n), a.getcolumn(j, 1, n));
            ap::vmove(a.getcolumn(j, 1, n), a.getcolumn(jp, 1, n));
            ap::vmove(a.getcolumn(jp, 1, n), work.getvector(1, n));
        }
    }
    return result;
}


bool inverse(ap::real_2d_array& a, int n)
{
    bool result;
    ap::integer_1d_array pivots;

    ludecomposition(a, n, n, pivots);
    result = inverselu(a, pivots, n);
    return result;
}




