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
#include "rcond.h"

static void internalestimatercondlu(const ap::real_2d_array& ludcmp,
     int n,
     bool onenorm,
     bool isanormprovided,
     double anorm,
     double& rc);
static void internalestimatenorm(int n,
     ap::real_1d_array& v,
     ap::real_1d_array& x,
     ap::integer_1d_array& isgn,
     double& est,
     int& kase);

/*************************************************************************
Estimate of a matrix condition number (1-norm)

The algorithm calculates a lower bound of the condition number. In this case,
the algorithm does not return a lower bound of the condition number, but an
inverse number (to avoid an overflow in case of a singular matrix).

Input parameters:
    A   -   matrix. Array whose indexes range within [0..N-1, 0..N-1].
    N   -   size of matrix A.

Result: 1/LowerBound(cond(A))
*************************************************************************/
double rmatrixrcond1(const ap::real_2d_array& a, int n)
{
    double result;
    int i;
    ap::real_2d_array a1;

    ap::ap_error::make_assertion(n>=1, "RMatrixRCond1: N<1!");
    a1.setbounds(1, n, 1, n);
    for(i = 1; i <= n; i++)
    {
        ap::vmove(&a1(i, 1), &a(i-1, 0), ap::vlen(1,n));
    }
    result = rcond1(a1, n);
    return result;
}


/*************************************************************************
Estimate of the condition number of a matrix given by its LU decomposition (1-norm)

The algorithm calculates a lower bound of the condition number. In this case,
the algorithm does not return a lower bound of the condition number, but an
inverse number (to avoid an overflow in case of a singular matrix).

Input parameters:
    LUDcmp      -   LU decomposition of a matrix in compact form. Output of
                    the RMatrixLU subroutine.
    N           -   size of matrix A.

Result: 1/LowerBound(cond(A))
*************************************************************************/
double rmatrixlurcond1(const ap::real_2d_array& ludcmp, int n)
{
    double result;
    int i;
    ap::real_2d_array a1;

    ap::ap_error::make_assertion(n>=1, "RMatrixLURCond1: N<1!");
    a1.setbounds(1, n, 1, n);
    for(i = 1; i <= n; i++)
    {
        ap::vmove(&a1(i, 1), &ludcmp(i-1, 0), ap::vlen(1,n));
    }
    result = rcond1lu(a1, n);
    return result;
}


/*************************************************************************
Estimate of a matrix condition number (infinity-norm).

The algorithm calculates a lower bound of the condition number. In this case,
the algorithm does not return a lower bound of the condition number, but an
inverse number (to avoid an overflow in case of a singular matrix).

Input parameters:
    A   -   matrix. Array whose indexes range within [0..N-1, 0..N-1].
    N   -   size of matrix A.

Result: 1/LowerBound(cond(A))
*************************************************************************/
double rmatrixrcondinf(const ap::real_2d_array& a, int n)
{
    double result;
    int i;
    ap::real_2d_array a1;

    ap::ap_error::make_assertion(n>=1, "RMatrixRCondInf: N<1!");
    a1.setbounds(1, n, 1, n);
    for(i = 1; i <= n; i++)
    {
        ap::vmove(&a1(i, 1), &a(i-1, 0), ap::vlen(1,n));
    }
    result = rcondinf(a1, n);
    return result;
}


/*************************************************************************
Estimate of the condition number of a matrix given by its LU decomposition
(infinity norm).

The algorithm calculates a lower bound of the condition number. In this case,
the algorithm does not return a lower bound of the condition number, but an
inverse number (to avoid an overflow in case of a singular matrix).

Input parameters:
    LUDcmp  -   LU decomposition of a matrix in compact form. Output of
                the RMatrixLU subroutine.
    N       -   size of matrix A.

Result: 1/LowerBound(cond(A))
*************************************************************************/
double rmatrixlurcondinf(const ap::real_2d_array& ludcmp, int n)
{
    double result;
    int i;
    ap::real_2d_array a1;

    ap::ap_error::make_assertion(n>=1, "RMatrixLURCondInf: N<1!");
    a1.setbounds(1, n, 1, n);
    for(i = 1; i <= n; i++)
    {
        ap::vmove(&a1(i, 1), &ludcmp(i-1, 0), ap::vlen(1,n));
    }
    result = rcondinflu(a1, n);
    return result;
}


double rcond1(ap::real_2d_array a, int n)
{
    double result;
    int i;
    int j;
    double v;
    double nrm;
    ap::integer_1d_array pivots;

    nrm = 0;
    for(j = 1; j <= n; j++)
    {
        v = 0;
        for(i = 1; i <= n; i++)
        {
            v = v+fabs(a(i,j));
        }
        nrm = ap::maxreal(nrm, v);
    }
    ludecomposition(a, n, n, pivots);
    internalestimatercondlu(a, n, true, true, nrm, v);
    result = v;
    return result;
}


double rcond1lu(const ap::real_2d_array& ludcmp, int n)
{
    double result;
    double v;

    internalestimatercondlu(ludcmp, n, true, false, double(0), v);
    result = v;
    return result;
}


double rcondinf(ap::real_2d_array a, int n)
{
    double result;
    int i;
    int j;
    double v;
    double nrm;
    ap::integer_1d_array pivots;

    nrm = 0;
    for(i = 1; i <= n; i++)
    {
        v = 0;
        for(j = 1; j <= n; j++)
        {
            v = v+fabs(a(i,j));
        }
        nrm = ap::maxreal(nrm, v);
    }
    ludecomposition(a, n, n, pivots);
    internalestimatercondlu(a, n, false, true, nrm, v);
    result = v;
    return result;
}


double rcondinflu(const ap::real_2d_array& ludcmp, int n)
{
    double result;
    double v;

    internalestimatercondlu(ludcmp, n, false, false, double(0), v);
    result = v;
    return result;
}


static void internalestimatercondlu(const ap::real_2d_array& ludcmp,
     int n,
     bool onenorm,
     bool isanormprovided,
     double anorm,
     double& rc)
{
    ap::real_1d_array work0;
    ap::real_1d_array work1;
    ap::real_1d_array work2;
    ap::real_1d_array work3;
    ap::integer_1d_array iwork;
    double v;
    bool normin;
    int i;
    int im1;
    int ip1;
    int ix;
    int kase;
    int kase1;
    double ainvnm;
    double ascale;
    double sl;
    double smlnum;
    double su;
    bool mupper;
    bool mtrans;
    bool munit;

    
    //
    // Quick return if possible
    //
    if( n==0 )
    {
        rc = 1;
        return;
    }
    
    //
    // init
    //
    if( onenorm )
    {
        kase1 = 1;
    }
    else
    {
        kase1 = 2;
    }
    mupper = true;
    mtrans = true;
    munit = true;
    work0.setbounds(1, n);
    work1.setbounds(1, n);
    work2.setbounds(1, n);
    work3.setbounds(1, n);
    iwork.setbounds(1, n);
    
    //
    // Estimate the norm of A.
    //
    if( !isanormprovided )
    {
        kase = 0;
        anorm = 0;
        while(true)
        {
            internalestimatenorm(n, work1, work0, iwork, anorm, kase);
            if( kase==0 )
            {
                break;
            }
            if( kase==kase1 )
            {
                
                //
                // Multiply by U
                //
                for(i = 1; i <= n; i++)
                {
                    v = ap::vdotproduct(&ludcmp(i, i), &work0(i), ap::vlen(i,n));
                    work0(i) = v;
                }
                
                //
                // Multiply by L
                //
                for(i = n; i >= 1; i--)
                {
                    im1 = i-1;
                    if( i>1 )
                    {
                        v = ap::vdotproduct(&ludcmp(i, 1), &work0(1), ap::vlen(1,im1));
                    }
                    else
                    {
                        v = 0;
                    }
                    work0(i) = work0(i)+v;
                }
            }
            else
            {
                
                //
                // Multiply by L'
                //
                for(i = 1; i <= n; i++)
                {
                    ip1 = i+1;
                    v = ap::vdotproduct(ludcmp.getcolumn(i, ip1, n), work0.getvector(ip1, n));
                    work0(i) = work0(i)+v;
                }
                
                //
                // Multiply by U'
                //
                for(i = n; i >= 1; i--)
                {
                    v = ap::vdotproduct(ludcmp.getcolumn(i, 1, i), work0.getvector(1, i));
                    work0(i) = v;
                }
            }
        }
    }
    
    //
    // Quick return if possible
    //
    rc = 0;
    if( ap::fp_eq(anorm,0) )
    {
        return;
    }
    
    //
    // Estimate the norm of inv(A).
    //
    smlnum = ap::minrealnumber;
    ainvnm = 0;
    normin = false;
    kase = 0;
    while(true)
    {
        internalestimatenorm(n, work1, work0, iwork, ainvnm, kase);
        if( kase==0 )
        {
            break;
        }
        if( kase==kase1 )
        {
            
            //
            // Multiply by inv(L).
            //
            safesolvetriangular(ludcmp, n, work0, sl, !mupper, !mtrans, munit, normin, work2);
            
            //
            // Multiply by inv(U).
            //
            safesolvetriangular(ludcmp, n, work0, su, mupper, !mtrans, !munit, normin, work3);
        }
        else
        {
            
            //
            // Multiply by inv(U').
            //
            safesolvetriangular(ludcmp, n, work0, su, mupper, mtrans, !munit, normin, work3);
            
            //
            // Multiply by inv(L').
            //
            safesolvetriangular(ludcmp, n, work0, sl, !mupper, mtrans, munit, normin, work2);
        }
        
        //
        // Divide X by 1/(SL*SU) if doing so will not cause overflow.
        //
        ascale = sl*su;
        normin = true;
        if( ap::fp_neq(ascale,1) )
        {
            ix = 1;
            for(i = 2; i <= n; i++)
            {
                if( ap::fp_greater(fabs(work0(i)),fabs(work0(ix))) )
                {
                    ix = i;
                }
            }
            if( ap::fp_less(ascale,fabs(work0(ix))*smlnum)||ap::fp_eq(ascale,0) )
            {
                return;
            }
            for(i = 1; i <= n; i++)
            {
                work0(i) = work0(i)/ascale;
            }
        }
    }
    
    //
    // Compute the estimate of the reciprocal condition number.
    //
    if( ap::fp_neq(ainvnm,0) )
    {
        rc = 1/ainvnm;
        rc = rc/anorm;
    }
}


static void internalestimatenorm(int n,
     ap::real_1d_array& v,
     ap::real_1d_array& x,
     ap::integer_1d_array& isgn,
     double& est,
     int& kase)
{
    int itmax;
    int i;
    double t;
    bool flg;
    int positer;
    int posj;
    int posjlast;
    int posjump;
    int posaltsgn;
    int posestold;
    int postemp;

    itmax = 5;
    posaltsgn = n+1;
    posestold = n+2;
    postemp = n+3;
    positer = n+1;
    posj = n+2;
    posjlast = n+3;
    posjump = n+4;
    if( kase==0 )
    {
        v.setbounds(1, n+3);
        x.setbounds(1, n);
        isgn.setbounds(1, n+4);
        t = double(1)/double(n);
        for(i = 1; i <= n; i++)
        {
            x(i) = t;
        }
        kase = 1;
        isgn(posjump) = 1;
        return;
    }
    
    //
    //     ................ ENTRY   (JUMP = 1)
    //     FIRST ITERATION.  X HAS BEEN OVERWRITTEN BY A*X.
    //
    if( isgn(posjump)==1 )
    {
        if( n==1 )
        {
            v(1) = x(1);
            est = fabs(v(1));
            kase = 0;
            return;
        }
        est = 0;
        for(i = 1; i <= n; i++)
        {
            est = est+fabs(x(i));
        }
        for(i = 1; i <= n; i++)
        {
            if( ap::fp_greater_eq(x(i),0) )
            {
                x(i) = 1;
            }
            else
            {
                x(i) = -1;
            }
            isgn(i) = ap::sign(x(i));
        }
        kase = 2;
        isgn(posjump) = 2;
        return;
    }
    
    //
    //     ................ ENTRY   (JUMP = 2)
    //     FIRST ITERATION.  X HAS BEEN OVERWRITTEN BY TRANDPOSE(A)*X.
    //
    if( isgn(posjump)==2 )
    {
        isgn(posj) = 1;
        for(i = 2; i <= n; i++)
        {
            if( ap::fp_greater(fabs(x(i)),fabs(x(isgn(posj)))) )
            {
                isgn(posj) = i;
            }
        }
        isgn(positer) = 2;
        
        //
        // MAIN LOOP - ITERATIONS 2,3,...,ITMAX.
        //
        for(i = 1; i <= n; i++)
        {
            x(i) = 0;
        }
        x(isgn(posj)) = 1;
        kase = 1;
        isgn(posjump) = 3;
        return;
    }
    
    //
    //     ................ ENTRY   (JUMP = 3)
    //     X HAS BEEN OVERWRITTEN BY A*X.
    //
    if( isgn(posjump)==3 )
    {
        ap::vmove(&v(1), &x(1), ap::vlen(1,n));
        v(posestold) = est;
        est = 0;
        for(i = 1; i <= n; i++)
        {
            est = est+fabs(v(i));
        }
        flg = false;
        for(i = 1; i <= n; i++)
        {
            if( ap::fp_greater_eq(x(i),0)&&isgn(i)<0||ap::fp_less(x(i),0)&&isgn(i)>=0 )
            {
                flg = true;
            }
        }
        
        //
        // REPEATED SIGN VECTOR DETECTED, HENCE ALGORITHM HAS CONVERGED.
        // OR MAY BE CYCLING.
        //
        if( !flg||ap::fp_less_eq(est,v(posestold)) )
        {
            v(posaltsgn) = 1;
            for(i = 1; i <= n; i++)
            {
                x(i) = v(posaltsgn)*(1+double(i-1)/double(n-1));
                v(posaltsgn) = -v(posaltsgn);
            }
            kase = 1;
            isgn(posjump) = 5;
            return;
        }
        for(i = 1; i <= n; i++)
        {
            if( ap::fp_greater_eq(x(i),0) )
            {
                x(i) = 1;
                isgn(i) = 1;
            }
            else
            {
                x(i) = -1;
                isgn(i) = -1;
            }
        }
        kase = 2;
        isgn(posjump) = 4;
        return;
    }
    
    //
    //     ................ ENTRY   (JUMP = 4)
    //     X HAS BEEN OVERWRITTEN BY TRANDPOSE(A)*X.
    //
    if( isgn(posjump)==4 )
    {
        isgn(posjlast) = isgn(posj);
        isgn(posj) = 1;
        for(i = 2; i <= n; i++)
        {
            if( ap::fp_greater(fabs(x(i)),fabs(x(isgn(posj)))) )
            {
                isgn(posj) = i;
            }
        }
        if( ap::fp_neq(x(isgn(posjlast)),fabs(x(isgn(posj))))&&isgn(positer)<itmax )
        {
            isgn(positer) = isgn(positer)+1;
            for(i = 1; i <= n; i++)
            {
                x(i) = 0;
            }
            x(isgn(posj)) = 1;
            kase = 1;
            isgn(posjump) = 3;
            return;
        }
        
        //
        // ITERATION COMPLETE.  FINAL STAGE.
        //
        v(posaltsgn) = 1;
        for(i = 1; i <= n; i++)
        {
            x(i) = v(posaltsgn)*(1+double(i-1)/double(n-1));
            v(posaltsgn) = -v(posaltsgn);
        }
        kase = 1;
        isgn(posjump) = 5;
        return;
    }
    
    //
    //     ................ ENTRY   (JUMP = 5)
    //     X HAS BEEN OVERWRITTEN BY A*X.
    //
    if( isgn(posjump)==5 )
    {
        v(postemp) = 0;
        for(i = 1; i <= n; i++)
        {
            v(postemp) = v(postemp)+fabs(x(i));
        }
        v(postemp) = 2*v(postemp)/(3*n);
        if( ap::fp_greater(v(postemp),est) )
        {
            ap::vmove(&v(1), &x(1), ap::vlen(1,n));
            est = v(postemp);
        }
        kase = 0;
        return;
    }
}




