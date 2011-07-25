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

#ifndef _densesolver_h
#define _densesolver_h

#include "ap.h"
#include "ialglib.h"

#include "reflections.h"
#include "bidiagonal.h"
#include "qr.h"
#include "lq.h"
#include "blas.h"
#include "rotations.h"
#include "bdsvd.h"
#include "svd.h"
#include "lu.h"
#include "trlinsolve.h"
#include "rcond.h"
#include "tsort.h"
#include "xblas.h"


struct densesolverreport
{
    double r1;
    double rinf;
};


struct densesolverlsreport
{
    double r2;
    ap::real_2d_array cx;
    int n;
    int k;
};




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
     ap::real_2d_array& x);


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
     ap::real_1d_array& x);


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
     ap::real_1d_array& x);


#endif

