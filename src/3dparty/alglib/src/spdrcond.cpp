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
#include "spdrcond.h"

/*************************************************************************
Condition number estimate of a symmetric positive definite matrix.

The algorithm calculates a lower bound of the condition number. In this case,
the algorithm does not return a lower bound of the condition number, but an
inverse number (to avoid an overflow in case of a singular matrix).

It should be noted that 1-norm and inf-norm of condition numbers of symmetric
matrices are equal, so the algorithm doesn't take into account the
differences between these types of norms.

Input parameters:
    A       -   symmetric positive definite matrix which is given by its
                upper or lower triangle depending on the value of
                IsUpper. Array with elements [0..N-1, 0..N-1].
    N       -   size of matrix A.
    IsUpper -   storage format.

Result:
    1/LowerBound(cond(A)), if matrix A is positive definite,
   -1, if matrix A is not positive definite, and its condition number
    could not be found by this algorithm.
*************************************************************************/
double spdmatrixrcond(const ap::real_2d_array& a, int n, bool isupper)
{
    double result;
    ap::real_2d_array a1;
    int i;
    int j;
    int im;
    int jm;
    double v;
    double nrm;
    ap::integer_1d_array pivots;

    a1.setbounds(1, n, 1, n);
    for(i = 1; i <= n; i++)
    {
        if( isupper )
        {
            for(j = i; j <= n; j++)
            {
                a1(i,j) = a(i-1,j-1);
            }
        }
        else
        {
            for(j = 1; j <= i; j++)
            {
                a1(i,j) = a(i-1,j-1);
            }
        }
    }
    nrm = 0;
    for(j = 1; j <= n; j++)
    {
        v = 0;
        for(i = 1; i <= n; i++)
        {
            im = i;
            jm = j;
            if( isupper&&j<i )
            {
                im = j;
                jm = i;
            }
            if( !isupper&&j>i )
            {
                im = j;
                jm = i;
            }
            v = v+fabs(a1(im,jm));
        }
        nrm = ap::maxreal(nrm, v);
    }
    if( choleskydecomposition(a1, n, isupper) )
    {
        internalcholeskyrcond(a1, n, isupper, true, nrm, v);
        result = v;
    }
    else
    {
        result = -1;
    }
    return result;
}


/*************************************************************************
Condition number estimate of a symmetric positive definite matrix given by
Cholesky decomposition.

The algorithm calculates a lower bound of the condition number. In this
case, the algorithm does not return a lower bound of the condition number,
but an inverse number (to avoid an overflow in case of a singular matrix).

It should be noted that 1-norm and inf-norm condition numbers of symmetric
matrices are equal, so the algorithm doesn't take into account the
differences between these types of norms.

Input parameters:
    CD  - Cholesky decomposition of matrix A,
          output of SMatrixCholesky subroutine.
    N   - size of matrix A.

Result: 1/LowerBound(cond(A))
*************************************************************************/
double spdmatrixcholeskyrcond(const ap::real_2d_array& a,
     int n,
     bool isupper)
{
    double result;
    ap::real_2d_array a1;
    int i;
    int j;
    double v;

    a1.setbounds(1, n, 1, n);
    for(i = 1; i <= n; i++)
    {
        if( isupper )
        {
            for(j = i; j <= n; j++)
            {
                a1(i,j) = a(i-1,j-1);
            }
        }
        else
        {
            for(j = 1; j <= i; j++)
            {
                a1(i,j) = a(i-1,j-1);
            }
        }
    }
    internalcholeskyrcond(a1, n, isupper, false, double(0), v);
    result = v;
    return result;
}


double rcondspd(ap::real_2d_array a, int n, bool isupper)
{
    double result;
    int i;
    int j;
    int im;
    int jm;
    double v;
    double nrm;
    ap::integer_1d_array pivots;

    nrm = 0;
    for(j = 1; j <= n; j++)
    {
        v = 0;
        for(i = 1; i <= n; i++)
        {
            im = i;
            jm = j;
            if( isupper&&j<i )
            {
                im = j;
                jm = i;
            }
            if( !isupper&&j>i )
            {
                im = j;
                jm = i;
            }
            v = v+fabs(a(im,jm));
        }
        nrm = ap::maxreal(nrm, v);
    }
    if( choleskydecomposition(a, n, isupper) )
    {
        internalcholeskyrcond(a, n, isupper, true, nrm, v);
        result = v;
    }
    else
    {
        result = -1;
    }
    return result;
}


double rcondcholesky(const ap::real_2d_array& cd, int n, bool isupper)
{
    double result;
    double v;

    internalcholeskyrcond(cd, n, isupper, false, double(0), v);
    result = v;
    return result;
}


void internalcholeskyrcond(const ap::real_2d_array& chfrm,
     int n,
     bool isupper,
     bool isnormprovided,
     double anorm,
     double& rcond)
{
    bool normin;
    int i;
    int ix;
    int kase;
    double ainvnm;
    double scl;
    double scalel;
    double scaleu;
    double smlnum;
    ap::real_1d_array work0;
    ap::real_1d_array work1;
    ap::real_1d_array work2;
    ap::integer_1d_array iwork;
    double v;

    ap::ap_error::make_assertion(n>=0, "");
    
    //
    // Estimate the norm of A.
    //
    if( !isnormprovided )
    {
        kase = 0;
        anorm = 0;
        while(true)
        {
            iterativeestimate1norm(n, work1, work0, iwork, anorm, kase);
            if( kase==0 )
            {
                break;
            }
            if( isupper )
            {
                
                //
                // Multiply by U
                //
                for(i = 1; i <= n; i++)
                {
                    v = ap::vdotproduct(&chfrm(i, i), &work0(i), ap::vlen(i,n));
                    work0(i) = v;
                }
                
                //
                // Multiply by U'
                //
                for(i = n; i >= 1; i--)
                {
                    v = ap::vdotproduct(chfrm.getcolumn(i, 1, i), work0.getvector(1, i));
                    work0(i) = v;
                }
            }
            else
            {
                
                //
                // Multiply by L'
                //
                for(i = 1; i <= n; i++)
                {
                    v = ap::vdotproduct(chfrm.getcolumn(i, i, n), work0.getvector(i, n));
                    work0(i) = v;
                }
                
                //
                // Multiply by L
                //
                for(i = n; i >= 1; i--)
                {
                    v = ap::vdotproduct(&chfrm(i, 1), &work0(1), ap::vlen(1,i));
                    work0(i) = v;
                }
            }
        }
    }
    
    //
    // Quick return if possible
    //
    rcond = 0;
    if( n==0 )
    {
        rcond = 1;
        return;
    }
    if( ap::fp_eq(anorm,0) )
    {
        return;
    }
    smlnum = ap::minrealnumber;
    
    //
    // Estimate the 1-norm of inv(A).
    //
    kase = 0;
    normin = false;
    while(true)
    {
        iterativeestimate1norm(n, work1, work0, iwork, ainvnm, kase);
        if( kase==0 )
        {
            break;
        }
        if( isupper )
        {
            
            //
            // Multiply by inv(U').
            //
            safesolvetriangular(chfrm, n, work0, scalel, isupper, true, false, normin, work2);
            normin = true;
            
            //
            // Multiply by inv(U).
            //
            safesolvetriangular(chfrm, n, work0, scaleu, isupper, false, false, normin, work2);
        }
        else
        {
            
            //
            // Multiply by inv(L).
            //
            safesolvetriangular(chfrm, n, work0, scalel, isupper, false, false, normin, work2);
            normin = true;
            
            //
            // Multiply by inv(L').
            //
            safesolvetriangular(chfrm, n, work0, scaleu, isupper, true, false, normin, work2);
        }
        
        //
        // Multiply by 1/SCALE if doing so will not cause overflow.
        //
        scl = scalel*scaleu;
        if( ap::fp_neq(scl,1) )
        {
            ix = 1;
            for(i = 2; i <= n; i++)
            {
                if( ap::fp_greater(fabs(work0(i)),fabs(work0(ix))) )
                {
                    ix = i;
                }
            }
            if( ap::fp_less(scl,fabs(work0(ix))*smlnum)||ap::fp_eq(scl,0) )
            {
                return;
            }
            for(i = 1; i <= n; i++)
            {
                work0(i) = work0(i)/scl;
            }
        }
    }
    
    //
    // Compute the estimate of the reciprocal condition number.
    //
    if( ap::fp_neq(ainvnm,0) )
    {
        v = 1/ainvnm;
        rcond = v/anorm;
    }
}




