/*************************************************************************
Copyright (c) 2007-2008, Sergey Bochkanov (ALGLIB project).

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
#include "densesolver.h"

static int densesolverrfsmax(int n, double r1, double rinf);
static int densesolverrfsmaxv2(int n, double r2);

/*************************************************************************
Dense solver.

This  subroutine  solves  a  system  A*X=B,  where A is NxN non-denegerate
real matrix, X and B are NxM real matrices.

Additional features include:
* automatic detection of degenerate cases
* iterative improvement

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   size of right part
    
OUTPUT PARAMETERS
    Info    -   return code:
                * -3    if A is singular, or VERY close to singular.
                        X is filled by zeros in such cases.
                * -1    if N<=0 or M<=0 was passed
                *  1    if task is solved (matrix A may be near  singular,
                        check R1/RInf parameters for condition numbers).
    Rep     -   solver report, see below for more info
    X       -   array[0..N-1,0..M-1], it contains:
                * solution of A*X=B if A is non-singular (well-conditioned
                  or ill-conditioned, but not very close to singular)
                * zeros,  if  A  is  singular  or  VERY  close to singular
                  (in this case Info=-3).

SOLVER REPORT

Subroutine sets following fields of the Rep structure:
* R1        reciprocal of condition number: 1/cond(A), 1-norm.
* RInf      reciprocal of condition number: 1/cond(A), inf-norm.

SEE ALSO:
    DenseSolverR() - solves A*x = b, where x and b are Nx1 matrices.

  -- ALGLIB --
     Copyright 24.08.2009 by Bochkanov Sergey
*************************************************************************/
void rmatrixsolvem(const ap::real_2d_array& a,
     int n,
     const ap::real_2d_array& b,
     int m,
     int& info,
     densesolverreport& rep,
     ap::real_2d_array& x)
{
    int i;
    int j;
    int k;
    int rfs;
    int nrfs;
    ap::integer_1d_array p;
    ap::real_1d_array xc;
    ap::real_1d_array y;
    ap::real_1d_array bc;
    ap::real_1d_array xa;
    ap::real_1d_array xb;
    ap::real_1d_array tx;
    ap::real_2d_array da;
    double v;
    double verr;
    bool smallerr;
    bool terminatenexttime;

    
    //
    // prepare: check inputs, allocate space...
    //
    if( n<=0||m<=0 )
    {
        info = -1;
        return;
    }
    da.setlength(n, n);
    x.setlength(n, m);
    y.setlength(n);
    xc.setlength(n);
    bc.setlength(n);
    tx.setlength(n+1);
    xa.setlength(n+1);
    xb.setlength(n+1);
    
    //
    // factorize matrix, test for exact/near singularity
    //
    for(i = 0; i <= n-1; i++)
    {
        ap::vmove(&da(i, 0), &a(i, 0), ap::vlen(0,n-1));
    }
    rmatrixlu(da, n, n, p);
    rep.r1 = rmatrixlurcond1(da, n);
    rep.rinf = rmatrixlurcondinf(da, n);
    if( ap::fp_less(rep.r1,10*ap::machineepsilon)||ap::fp_less(rep.rinf,10*ap::machineepsilon) )
    {
        for(i = 0; i <= n-1; i++)
        {
            for(j = 0; j <= m-1; j++)
            {
                x(i,j) = 0;
            }
        }
        rep.r1 = 0;
        rep.rinf = 0;
        info = -3;
        return;
    }
    info = 1;
    
    //
    // solve
    //
    for(k = 0; k <= m-1; k++)
    {
        
        //
        // First, non-iterative part of solution process:
        // * pivots
        // * L*y = b
        // * U*x = y
        //
        ap::vmove(bc.getvector(0, n-1), b.getcolumn(k, 0, n-1));
        for(i = 0; i <= n-1; i++)
        {
            if( p(i)!=i )
            {
                v = bc(i);
                bc(i) = bc(p(i));
                bc(p(i)) = v;
            }
        }
        y(0) = bc(0);
        for(i = 1; i <= n-1; i++)
        {
            v = ap::vdotproduct(&da(i, 0), &y(0), ap::vlen(0,i-1));
            y(i) = bc(i)-v;
        }
        xc(n-1) = y(n-1)/da(n-1,n-1);
        for(i = n-2; i >= 0; i--)
        {
            v = ap::vdotproduct(&da(i, i+1), &xc(i+1), ap::vlen(i+1,n-1));
            xc(i) = (y(i)-v)/da(i,i);
        }
        
        //
        // Iterative improvement of xc:
        // * calculate r = bc-A*xc using extra-precise dot product
        // * solve A*y = r
        // * update x:=x+r
        //
        // This cycle is executed until one of two things happens:
        // 1. maximum number of iterations reached
        // 2. last iteration decreased error to the lower limit
        //
        nrfs = densesolverrfsmax(n, rep.r1, rep.rinf);
        terminatenexttime = false;
        for(rfs = 0; rfs <= nrfs-1; rfs++)
        {
            if( terminatenexttime )
            {
                break;
            }
            
            //
            // generate right part
            //
            smallerr = true;
            for(i = 0; i <= n-1; i++)
            {
                ap::vmove(&xa(0), &a(i, 0), ap::vlen(0,n-1));
                xa(n) = -1;
                ap::vmove(&xb(0), &xc(0), ap::vlen(0,n-1));
                xb(n) = b(i,k);
                xdot(xa, xb, n+1, tx, v, verr);
                bc(i) = -v;
                smallerr = smallerr&&ap::fp_less(fabs(v),4*verr);
            }
            if( smallerr )
            {
                terminatenexttime = true;
            }
            
            //
            // solve
            //
            for(i = 0; i <= n-1; i++)
            {
                if( p(i)!=i )
                {
                    v = bc(i);
                    bc(i) = bc(p(i));
                    bc(p(i)) = v;
                }
            }
            y(0) = bc(0);
            for(i = 1; i <= n-1; i++)
            {
                v = ap::vdotproduct(&da(i, 0), &y(0), ap::vlen(0,i-1));
                y(i) = bc(i)-v;
            }
            tx(n-1) = y(n-1)/da(n-1,n-1);
            for(i = n-2; i >= 0; i--)
            {
                v = ap::vdotproduct(&da(i, i+1), &tx(i+1), ap::vlen(i+1,n-1));
                tx(i) = (y(i)-v)/da(i,i);
            }
            
            //
            // update
            //
            ap::vadd(&xc(0), &tx(0), ap::vlen(0,n-1));
        }
        
        //
        // Store xc
        //
        ap::vmove(x.getcolumn(k, 0, n-1), xc.getvector(0, n-1));
    }
}


/*************************************************************************
Dense solver.

This subroutine finds solution of the linear system A*X=B with non-square,
possibly degenerate A.  System  is  solved in the least squares sense, and
general least squares solution  X = X0 + CX*y  which  minimizes |A*X-B| is
returned. If A is non-degenerate, solution in the  usual sense is returned

Additional features include:
* iterative improvement

INPUT PARAMETERS
    A       -   array[0..NRows-1,0..NCols-1], system matrix
    NRows   -   vertical size of A
    NCols   -   horizontal size of A
    B       -   array[0..NCols-1], right part
    Threshold-  a number in [0,1]. Singular values  beyond  Threshold  are
                considered  zero.  Set  it to 0.0, if you don't understand
                what it means, so the solver will choose good value on its
                own.
                
OUTPUT PARAMETERS
    Info    -   return code:
                * -4    SVD subroutine failed
                * -1    if NRows<=0 or NCols<=0 or Threshold<0 was passed
                *  1    if task is solved
    Rep     -   solver report, see below for more info
    X       -   array[0..N-1,0..M-1], it contains:
                * solution of A*X=B if A is non-singular (well-conditioned
                  or ill-conditioned, but not very close to singular)
                * zeros,  if  A  is  singular  or  VERY  close to singular
                  (in this case Info=-3).

SOLVER REPORT

Subroutine sets following fields of the Rep structure:
* R2        reciprocal of condition number: 1/cond(A), 2-norm.
* N         = NCols
* K         dim(Null(A))
* CX        array[0..N-1,0..K-1], kernel of A.
            Columns of CX store such vectors that A*CX[i]=0.

  -- ALGLIB --
     Copyright 24.08.2009 by Bochkanov Sergey
*************************************************************************/
void rmatrixsolvels(const ap::real_2d_array& a,
     int nrows,
     int ncols,
     const ap::real_1d_array& b,
     double threshold,
     int& info,
     densesolverlsreport& rep,
     ap::real_1d_array& x)
{
    ap::real_1d_array sv;
    ap::real_2d_array u;
    ap::real_2d_array vt;
    ap::real_1d_array rp;
    ap::real_1d_array utb;
    ap::real_1d_array sutb;
    ap::real_1d_array tmp;
    ap::real_1d_array ta;
    ap::real_1d_array tx;
    ap::real_1d_array buf;
    ap::real_1d_array w;
    int i;
    int j;
    int nsv;
    int kernelidx;
    double v;
    double verr;
    bool svdfailed;
    bool zeroa;
    int rfs;
    int nrfs;
    bool terminatenexttime;
    bool smallerr;

    if( nrows<=0||ncols<=0||ap::fp_less(threshold,0) )
    {
        info = -1;
        return;
    }
    if( ap::fp_eq(threshold,0) )
    {
        threshold = 1000*ap::machineepsilon;
    }
    
    //
    // Factorize A first
    //
    svdfailed = !rmatrixsvd(a, nrows, ncols, 1, 2, 2, sv, u, vt);
    zeroa = ap::fp_eq(sv(0),0);
    if( svdfailed||zeroa )
    {
        if( svdfailed )
        {
            info = -4;
        }
        else
        {
            info = 1;
        }
        x.setlength(ncols);
        for(i = 0; i <= ncols-1; i++)
        {
            x(i) = 0;
        }
        rep.n = ncols;
        rep.k = ncols;
        rep.cx.setlength(ncols, ncols);
        for(i = 0; i <= ncols-1; i++)
        {
            for(j = 0; j <= ncols-1; j++)
            {
                if( i==j )
                {
                    rep.cx(i,j) = 1;
                }
                else
                {
                    rep.cx(i,j) = 0;
                }
            }
        }
        rep.r2 = 0;
        return;
    }
    nsv = ap::minint(ncols, nrows);
    if( nsv==ncols )
    {
        rep.r2 = sv(nsv-1)/sv(0);
    }
    else
    {
        rep.r2 = 0;
    }
    rep.n = ncols;
    info = 1;
    
    //
    // Iterative improvement of xc combined with solution:
    // 1. xc = 0
    // 2. calculate r = bc-A*xc using extra-precise dot product
    // 3. solve A*y = r
    // 4. update x:=x+r
    // 5. goto 2
    //
    // This cycle is executed until one of two things happens:
    // 1. maximum number of iterations reached
    // 2. last iteration decreased error to the lower limit
    //
    utb.setlength(nsv);
    sutb.setlength(nsv);
    x.setlength(ncols);
    tmp.setlength(ncols);
    ta.setlength(ncols+1);
    tx.setlength(ncols+1);
    buf.setlength(ncols+1);
    for(i = 0; i <= ncols-1; i++)
    {
        x(i) = 0;
    }
    kernelidx = nsv;
    for(i = 0; i <= nsv-1; i++)
    {
        if( ap::fp_less_eq(sv(i),threshold*sv(0)) )
        {
            kernelidx = i;
            break;
        }
    }
    rep.k = ncols-kernelidx;
    nrfs = densesolverrfsmaxv2(ncols, rep.r2);
    terminatenexttime = false;
    rp.setlength(nrows);
    for(rfs = 0; rfs <= nrfs; rfs++)
    {
        if( terminatenexttime )
        {
            break;
        }
        
        //
        // calculate right part
        //
        if( rfs==0 )
        {
            ap::vmove(&rp(0), &b(0), ap::vlen(0,nrows-1));
        }
        else
        {
            smallerr = true;
            for(i = 0; i <= nrows-1; i++)
            {
                ap::vmove(&ta(0), &a(i, 0), ap::vlen(0,ncols-1));
                ta(ncols) = -1;
                ap::vmove(&tx(0), &x(0), ap::vlen(0,ncols-1));
                tx(ncols) = b(i);
                xdot(ta, tx, ncols+1, buf, v, verr);
                rp(i) = -v;
                smallerr = smallerr&&ap::fp_less(fabs(v),4*verr);
            }
            if( smallerr )
            {
                terminatenexttime = true;
            }
        }
        
        //
        // solve A*dx = rp
        //
        for(i = 0; i <= ncols-1; i++)
        {
            tmp(i) = 0;
        }
        for(i = 0; i <= nsv-1; i++)
        {
            utb(i) = 0;
        }
        for(i = 0; i <= nrows-1; i++)
        {
            v = rp(i);
            ap::vadd(&utb(0), &u(i, 0), ap::vlen(0,nsv-1), v);
        }
        for(i = 0; i <= nsv-1; i++)
        {
            if( i<kernelidx )
            {
                sutb(i) = utb(i)/sv(i);
            }
            else
            {
                sutb(i) = 0;
            }
        }
        for(i = 0; i <= nsv-1; i++)
        {
            v = sutb(i);
            ap::vadd(&tmp(0), &vt(i, 0), ap::vlen(0,ncols-1), v);
        }
        
        //
        // update x:  x:=x+dx
        //
        ap::vadd(&x(0), &tmp(0), ap::vlen(0,ncols-1));
    }
    
    //
    // fill CX
    //
    if( rep.k>0 )
    {
        rep.cx.setlength(ncols, rep.k);
        for(i = 0; i <= rep.k-1; i++)
        {
            ap::vmove(rep.cx.getcolumn(i, 0, ncols-1), vt.getrow(kernelidx+i, 0, ncols-1));
        }
    }
}


/*************************************************************************
Dense solver.

Similar to RMatrixSolveM() but solves task with one right part  (where b/x
are vectors, not matrices).

See RMatrixSolveM()  description  for  more  information  about subroutine
parameters.

  -- ALGLIB --
     Copyright 24.08.2009 by Bochkanov Sergey
*************************************************************************/
void rmatrixsolve(const ap::real_2d_array& a,
     int n,
     const ap::real_1d_array& b,
     int& info,
     densesolverreport& rep,
     ap::real_1d_array& x)
{
    ap::real_2d_array bm;
    ap::real_2d_array xm;

    if( n<=0 )
    {
        info = -1;
        return;
    }
    bm.setlength(n, 1);
    ap::vmove(bm.getcolumn(0, 0, n-1), b.getvector(0, n-1));
    rmatrixsolvem(a, n, bm, 1, info, rep, xm);
    x.setlength(n);
    ap::vmove(x.getvector(0, n-1), xm.getcolumn(0, 0, n-1));
}


/*************************************************************************
Internal subroutine.
Returns maximum count of RFS iterations as function of:
1. machine epsilon
2. task size.
3. condition number
*************************************************************************/
static int densesolverrfsmax(int n, double r1, double rinf)
{
    int result;

    result = 2;
    return result;
}


/*************************************************************************
Internal subroutine.
Returns maximum count of RFS iterations as function of:
1. machine epsilon
2. task size.
3. norm-2 condition number
*************************************************************************/
static int densesolverrfsmaxv2(int n, double r2)
{
    int result;

    result = densesolverrfsmax(n, double(0), double(0));
    return result;
}




