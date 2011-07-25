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
#include "crcond.h"

static void internalcomplexrcondestimatenorm(const int& n,
     ap::complex_1d_array& v,
     ap::complex_1d_array& x,
     double& est,
     int& kase,
     ap::integer_1d_array& isave,
     ap::real_1d_array& rsave);
static double internalcomplexrcondscsum1(const ap::complex_1d_array& x, int n);
static int internalcomplexrcondicmax1(const ap::complex_1d_array& x, int n);
static void internalcomplexrcondsaveall(ap::integer_1d_array& isave,
     ap::real_1d_array& rsave,
     int& i,
     int& iter,
     int& j,
     int& jlast,
     int& jump,
     double& absxi,
     double& altsgn,
     double& estold,
     double& temp);
static void internalcomplexrcondloadall(ap::integer_1d_array& isave,
     ap::real_1d_array& rsave,
     int& i,
     int& iter,
     int& j,
     int& jlast,
     int& jump,
     double& absxi,
     double& altsgn,
     double& estold,
     double& temp);

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
double cmatrixrcond1(const ap::complex_2d_array& a, int n)
{
    double result;
    int i;
    ap::complex_2d_array a1;
    int i_;
    int i1_;

    ap::ap_error::make_assertion(n>=1, "CMatrixRCond1: N<1!");
    a1.setbounds(1, n, 1, n);
    for(i = 1; i <= n; i++)
    {
        i1_ = (0) - (1);
        for(i_=1; i_<=n;i_++)
        {
            a1(i,i_) = a(i-1,i_+i1_);
        }
    }
    result = complexrcond1(a1, n);
    return result;
}


/*************************************************************************
Estimate of the condition number of a matrix given by its LU decomposition (1-norm)

The algorithm calculates a lower bound of the condition number. In this case,
the algorithm does not return a lower bound of the condition number, but an
inverse number (to avoid an overflow in case of a singular matrix).

Input parameters:
    LUDcmp      -   LU decomposition of a matrix in compact form. Output of
                    the CMatrixLU subroutine.
    N           -   size of matrix A.

Result: 1/LowerBound(cond(A))
*************************************************************************/
double cmatrixlurcond1(const ap::complex_2d_array& ludcmp, int n)
{
    double result;
    int i;
    ap::complex_2d_array a1;
    int i_;
    int i1_;

    ap::ap_error::make_assertion(n>=1, "CMatrixLURCond1: N<1!");
    a1.setbounds(1, n, 1, n);
    for(i = 1; i <= n; i++)
    {
        i1_ = (0) - (1);
        for(i_=1; i_<=n;i_++)
        {
            a1(i,i_) = ludcmp(i-1,i_+i1_);
        }
    }
    result = complexrcond1lu(a1, n);
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
double cmatrixrcondinf(const ap::complex_2d_array& a, int n)
{
    double result;
    int i;
    ap::complex_2d_array a1;
    int i_;
    int i1_;

    ap::ap_error::make_assertion(n>=1, "CMatrixRCondInf: N<1!");
    a1.setbounds(1, n, 1, n);
    for(i = 1; i <= n; i++)
    {
        i1_ = (0) - (1);
        for(i_=1; i_<=n;i_++)
        {
            a1(i,i_) = a(i-1,i_+i1_);
        }
    }
    result = complexrcondinf(a1, n);
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
                the CMatrixLU subroutine.
    N       -   size of matrix A.

Result: 1/LowerBound(cond(A))
*************************************************************************/
double cmatrixlurcondinf(const ap::complex_2d_array& ludcmp, int n)
{
    double result;
    int i;
    ap::complex_2d_array a1;
    int i_;
    int i1_;

    ap::ap_error::make_assertion(n>=1, "CMatrixLURCondInf: N<1!");
    a1.setbounds(1, n, 1, n);
    for(i = 1; i <= n; i++)
    {
        i1_ = (0) - (1);
        for(i_=1; i_<=n;i_++)
        {
            a1(i,i_) = ludcmp(i-1,i_+i1_);
        }
    }
    result = complexrcondinflu(a1, n);
    return result;
}


double complexrcond1(ap::complex_2d_array a, int n)
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
            v = v+ap::abscomplex(a(i,j));
        }
        nrm = ap::maxreal(nrm, v);
    }
    complexludecomposition(a, n, n, pivots);
    internalestimatecomplexrcondlu(a, n, true, true, nrm, v);
    result = v;
    return result;
}


double complexrcond1lu(const ap::complex_2d_array& lu, int n)
{
    double result;
    double v;

    internalestimatecomplexrcondlu(lu, n, true, false, double(0), v);
    result = v;
    return result;
}


double complexrcondinf(ap::complex_2d_array a, int n)
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
            v = v+ap::abscomplex(a(i,j));
        }
        nrm = ap::maxreal(nrm, v);
    }
    complexludecomposition(a, n, n, pivots);
    internalestimatecomplexrcondlu(a, n, false, true, nrm, v);
    result = v;
    return result;
}


double complexrcondinflu(const ap::complex_2d_array& lu, int n)
{
    double result;
    double v;

    internalestimatecomplexrcondlu(lu, n, false, false, double(0), v);
    result = v;
    return result;
}


void internalestimatecomplexrcondlu(const ap::complex_2d_array& lu,
     const int& n,
     bool onenorm,
     bool isanormprovided,
     double anorm,
     double& rcond)
{
    ap::complex_1d_array cwork1;
    ap::complex_1d_array cwork2;
    ap::complex_1d_array cwork3;
    ap::complex_1d_array cwork4;
    ap::integer_1d_array isave;
    ap::real_1d_array rsave;
    int kase;
    int kase1;
    double ainvnm;
    double smlnum;
    bool cw;
    ap::complex v;
    int i;
    int i_;

    if( n<=0 )
    {
        return;
    }
    cwork1.setbounds(1, n);
    cwork2.setbounds(1, n);
    cwork3.setbounds(1, n);
    cwork4.setbounds(1, n);
    isave.setbounds(0, 4);
    rsave.setbounds(0, 3);
    rcond = 0;
    if( n==0 )
    {
        rcond = 1;
        return;
    }
    smlnum = ap::minrealnumber;
    
    //
    // Estimate the norm of inv(A).
    //
    if( !isanormprovided )
    {
        anorm = 0;
        if( onenorm )
        {
            kase1 = 1;
        }
        else
        {
            kase1 = 2;
        }
        kase = 0;
        do
        {
            internalcomplexrcondestimatenorm(n, cwork4, cwork1, anorm, kase, isave, rsave);
            if( kase!=0 )
            {
                if( kase==kase1 )
                {
                    
                    //
                    // Multiply by U
                    //
                    for(i = 1; i <= n; i++)
                    {
                        v = 0.0;
                        for(i_=i; i_<=n;i_++)
                        {
                            v += lu(i,i_)*cwork1(i_);
                        }
                        cwork1(i) = v;
                    }
                    
                    //
                    // Multiply by L
                    //
                    for(i = n; i >= 1; i--)
                    {
                        v = 0;
                        if( i>1 )
                        {
                            v = 0.0;
                            for(i_=1; i_<=i-1;i_++)
                            {
                                v += lu(i,i_)*cwork1(i_);
                            }
                        }
                        cwork1(i) = v+cwork1(i);
                    }
                }
                else
                {
                    
                    //
                    // Multiply by L'
                    //
                    for(i = 1; i <= n; i++)
                    {
                        cwork2(i) = 0;
                    }
                    for(i = 1; i <= n; i++)
                    {
                        v = cwork1(i);
                        if( i>1 )
                        {
                            for(i_=1; i_<=i-1;i_++)
                            {
                                cwork2(i_) = cwork2(i_) + v*ap::conj(lu(i,i_));
                            }
                        }
                        cwork2(i) = cwork2(i)+v;
                    }
                    
                    //
                    // Multiply by U'
                    //
                    for(i = 1; i <= n; i++)
                    {
                        cwork1(i) = 0;
                    }
                    for(i = 1; i <= n; i++)
                    {
                        v = cwork2(i);
                        for(i_=i; i_<=n;i_++)
                        {
                            cwork1(i_) = cwork1(i_) + v*ap::conj(lu(i,i_));
                        }
                    }
                }
            }
        }
        while(kase!=0);
    }
    
    //
    // Quick return if possible
    //
    if( ap::fp_eq(anorm,0) )
    {
        return;
    }
    
    //
    // Estimate the norm of inv(A).
    //
    ainvnm = 0;
    if( onenorm )
    {
        kase1 = 1;
    }
    else
    {
        kase1 = 2;
    }
    kase = 0;
    do
    {
        internalcomplexrcondestimatenorm(n, cwork4, cwork1, ainvnm, kase, isave, rsave);
        if( kase!=0 )
        {
            if( kase==kase1 )
            {
                
                //
                // Multiply by inv(L).
                //
                cw = complexsafesolvetriangular(lu, n, cwork1, false, 0, true, cwork2, cwork3);
                if( !cw )
                {
                    rcond = 0;
                    return;
                }
                
                //
                // Multiply by inv(U).
                //
                cw = complexsafesolvetriangular(lu, n, cwork1, true, 0, false, cwork2, cwork3);
                if( !cw )
                {
                    rcond = 0;
                    return;
                }
            }
            else
            {
                
                //
                // Multiply by inv(U').
                //
                cw = complexsafesolvetriangular(lu, n, cwork1, true, 2, false, cwork2, cwork3);
                if( !cw )
                {
                    rcond = 0;
                    return;
                }
                
                //
                // Multiply by inv(L').
                //
                cw = complexsafesolvetriangular(lu, n, cwork1, false, 2, true, cwork2, cwork3);
                if( !cw )
                {
                    rcond = 0;
                    return;
                }
            }
        }
    }
    while(kase!=0);
    
    //
    // Compute the estimate of the reciprocal condition number.
    //
    if( ap::fp_neq(ainvnm,0) )
    {
        rcond = 1/ainvnm;
        rcond = rcond/anorm;
    }
}


static void internalcomplexrcondestimatenorm(const int& n,
     ap::complex_1d_array& v,
     ap::complex_1d_array& x,
     double& est,
     int& kase,
     ap::integer_1d_array& isave,
     ap::real_1d_array& rsave)
{
    int itmax;
    int i;
    int iter;
    int j;
    int jlast;
    int jump;
    double absxi;
    double altsgn;
    double estold;
    double safmin;
    double temp;
    int i_;

    
    //
    //Executable Statements ..
    //
    itmax = 5;
    safmin = ap::minrealnumber;
    if( kase==0 )
    {
        for(i = 1; i <= n; i++)
        {
            x(i) = double(1)/double(n);
        }
        kase = 1;
        jump = 1;
        internalcomplexrcondsaveall(isave, rsave, i, iter, j, jlast, jump, absxi, altsgn, estold, temp);
        return;
    }
    internalcomplexrcondloadall(isave, rsave, i, iter, j, jlast, jump, absxi, altsgn, estold, temp);
    
    //
    // ENTRY   (JUMP = 1)
    // FIRST ITERATION.  X HAS BEEN OVERWRITTEN BY A*X.
    //
    if( jump==1 )
    {
        if( n==1 )
        {
            v(1) = x(1);
            est = ap::abscomplex(v(1));
            kase = 0;
            internalcomplexrcondsaveall(isave, rsave, i, iter, j, jlast, jump, absxi, altsgn, estold, temp);
            return;
        }
        est = internalcomplexrcondscsum1(x, n);
        for(i = 1; i <= n; i++)
        {
            absxi = ap::abscomplex(x(i));
            if( ap::fp_greater(absxi,safmin) )
            {
                x(i) = x(i)/absxi;
            }
            else
            {
                x(i) = 1;
            }
        }
        kase = 2;
        jump = 2;
        internalcomplexrcondsaveall(isave, rsave, i, iter, j, jlast, jump, absxi, altsgn, estold, temp);
        return;
    }
    
    //
    // ENTRY   (JUMP = 2)
    // FIRST ITERATION.  X HAS BEEN OVERWRITTEN BY CTRANS(A)*X.
    //
    if( jump==2 )
    {
        j = internalcomplexrcondicmax1(x, n);
        iter = 2;
        
        //
        // MAIN LOOP - ITERATIONS 2,3,...,ITMAX.
        //
        for(i = 1; i <= n; i++)
        {
            x(i) = 0;
        }
        x(j) = 1;
        kase = 1;
        jump = 3;
        internalcomplexrcondsaveall(isave, rsave, i, iter, j, jlast, jump, absxi, altsgn, estold, temp);
        return;
    }
    
    //
    // ENTRY   (JUMP = 3)
    // X HAS BEEN OVERWRITTEN BY A*X.
    //
    if( jump==3 )
    {
        for(i_=1; i_<=n;i_++)
        {
            v(i_) = x(i_);
        }
        estold = est;
        est = internalcomplexrcondscsum1(v, n);
        
        //
        // TEST FOR CYCLING.
        //
        if( ap::fp_less_eq(est,estold) )
        {
            
            //
            // ITERATION COMPLETE.  FINAL STAGE.
            //
            altsgn = 1;
            for(i = 1; i <= n; i++)
            {
                x(i) = altsgn*(1+double(i-1)/double(n-1));
                altsgn = -altsgn;
            }
            kase = 1;
            jump = 5;
            internalcomplexrcondsaveall(isave, rsave, i, iter, j, jlast, jump, absxi, altsgn, estold, temp);
            return;
        }
        for(i = 1; i <= n; i++)
        {
            absxi = ap::abscomplex(x(i));
            if( ap::fp_greater(absxi,safmin) )
            {
                x(i) = x(i)/absxi;
            }
            else
            {
                x(i) = 1;
            }
        }
        kase = 2;
        jump = 4;
        internalcomplexrcondsaveall(isave, rsave, i, iter, j, jlast, jump, absxi, altsgn, estold, temp);
        return;
    }
    
    //
    // ENTRY   (JUMP = 4)
    // X HAS BEEN OVERWRITTEN BY CTRANS(A)*X.
    //
    if( jump==4 )
    {
        jlast = j;
        j = internalcomplexrcondicmax1(x, n);
        if( ap::fp_neq(ap::abscomplex(x(jlast)),ap::abscomplex(x(j)))&&iter<itmax )
        {
            iter = iter+1;
            
            //
            // MAIN LOOP - ITERATIONS 2,3,...,ITMAX.
            //
            for(i = 1; i <= n; i++)
            {
                x(i) = 0;
            }
            x(j) = 1;
            kase = 1;
            jump = 3;
            internalcomplexrcondsaveall(isave, rsave, i, iter, j, jlast, jump, absxi, altsgn, estold, temp);
            return;
        }
        
        //
        // ITERATION COMPLETE.  FINAL STAGE.
        //
        altsgn = 1;
        for(i = 1; i <= n; i++)
        {
            x(i) = altsgn*(1+double(i-1)/double(n-1));
            altsgn = -altsgn;
        }
        kase = 1;
        jump = 5;
        internalcomplexrcondsaveall(isave, rsave, i, iter, j, jlast, jump, absxi, altsgn, estold, temp);
        return;
    }
    
    //
    // ENTRY   (JUMP = 5)
    // X HAS BEEN OVERWRITTEN BY A*X.
    //
    if( jump==5 )
    {
        temp = 2*(internalcomplexrcondscsum1(x, n)/(3*n));
        if( ap::fp_greater(temp,est) )
        {
            for(i_=1; i_<=n;i_++)
            {
                v(i_) = x(i_);
            }
            est = temp;
        }
        kase = 0;
        internalcomplexrcondsaveall(isave, rsave, i, iter, j, jlast, jump, absxi, altsgn, estold, temp);
        return;
    }
}


static double internalcomplexrcondscsum1(const ap::complex_1d_array& x,
     int n)
{
    double result;
    int i;

    result = 0;
    for(i = 1; i <= n; i++)
    {
        result = result+ap::abscomplex(x(i));
    }
    return result;
}


static int internalcomplexrcondicmax1(const ap::complex_1d_array& x, int n)
{
    int result;
    int i;
    double m;

    result = 1;
    m = ap::abscomplex(x(1));
    for(i = 2; i <= n; i++)
    {
        if( ap::fp_greater(ap::abscomplex(x(i)),m) )
        {
            result = i;
            m = ap::abscomplex(x(i));
        }
    }
    return result;
}


static void internalcomplexrcondsaveall(ap::integer_1d_array& isave,
     ap::real_1d_array& rsave,
     int& i,
     int& iter,
     int& j,
     int& jlast,
     int& jump,
     double& absxi,
     double& altsgn,
     double& estold,
     double& temp)
{

    isave(0) = i;
    isave(1) = iter;
    isave(2) = j;
    isave(3) = jlast;
    isave(4) = jump;
    rsave(0) = absxi;
    rsave(1) = altsgn;
    rsave(2) = estold;
    rsave(3) = temp;
}


static void internalcomplexrcondloadall(ap::integer_1d_array& isave,
     ap::real_1d_array& rsave,
     int& i,
     int& iter,
     int& j,
     int& jlast,
     int& jump,
     double& absxi,
     double& altsgn,
     double& estold,
     double& temp)
{

    i = isave(0);
    iter = isave(1);
    j = isave(2);
    jlast = isave(3);
    jump = isave(4);
    absxi = rsave(0);
    altsgn = rsave(1);
    estold = rsave(2);
    temp = rsave(3);
}




