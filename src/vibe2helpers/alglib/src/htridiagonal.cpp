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
#include "htridiagonal.h"

/*************************************************************************
Reduction of a Hermitian matrix which is given  by  its  higher  or  lower
triangular part to a real  tridiagonal  matrix  using  unitary  similarity
transformation: Q'*A*Q = T.

Input parameters:
    A       -   matrix to be transformed
                array with elements [0..N-1, 0..N-1].
    N       -   size of matrix A.
    IsUpper -   storage format. If IsUpper = True, then matrix A is  given
                by its upper triangle, and the lower triangle is not  used
                and not modified by the algorithm, and vice versa
                if IsUpper = False.

Output parameters:
    A       -   matrices T and Q in  compact form (see lower)
    Tau     -   array of factors which are forming matrices H(i)
                array with elements [0..N-2].
    D       -   main diagonal of real symmetric matrix T.
                array with elements [0..N-1].
    E       -   secondary diagonal of real symmetric matrix T.
                array with elements [0..N-2].


  If IsUpper=True, the matrix Q is represented as a product of elementary
  reflectors

     Q = H(n-2) . . . H(2) H(0).

  Each H(i) has the form

     H(i) = I - tau * v * v'

  where tau is a complex scalar, and v is a complex vector with
  v(i+1:n-1) = 0, v(i) = 1, v(0:i-1) is stored on exit in
  A(0:i-1,i+1), and tau in TAU(i).

  If IsUpper=False, the matrix Q is represented as a product of elementary
  reflectors

     Q = H(0) H(2) . . . H(n-2).

  Each H(i) has the form

     H(i) = I - tau * v * v'

  where tau is a complex scalar, and v is a complex vector with
  v(0:i) = 0, v(i+1) = 1, v(i+2:n-1) is stored on exit in A(i+2:n-1,i),
  and tau in TAU(i).

  The contents of A on exit are illustrated by the following examples
  with n = 5:

  if UPLO = 'U':                       if UPLO = 'L':

    (  d   e   v1  v2  v3 )              (  d                  )
    (      d   e   v2  v3 )              (  e   d              )
    (          d   e   v3 )              (  v0  e   d          )
    (              d   e  )              (  v0  v1  e   d      )
    (                  d  )              (  v0  v1  v2  e   d  )

where d and e denote diagonal and off-diagonal elements of T, and vi
denotes an element of the vector defining H(i).

  -- LAPACK routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     October 31, 1992
*************************************************************************/
void hmatrixtd(ap::complex_2d_array& a,
     int n,
     bool isupper,
     ap::complex_1d_array& tau,
     ap::real_1d_array& d,
     ap::real_1d_array& e)
{
    int i;
    ap::complex alpha;
    ap::complex taui;
    ap::complex v;
    ap::complex_1d_array t;
    ap::complex_1d_array t2;
    ap::complex_1d_array t3;
    int i_;
    int i1_;

    if( n<=0 )
    {
        return;
    }
    for(i = 0; i <= n-1; i++)
    {
        ap::ap_error::make_assertion(ap::fp_eq(a(i,i).y,0), "");
    }
    if( n>1 )
    {
        tau.setbounds(0, n-2);
        e.setbounds(0, n-2);
    }
    d.setbounds(0, n-1);
    t.setbounds(0, n-1);
    t2.setbounds(0, n-1);
    t3.setbounds(0, n-1);
    if( isupper )
    {
        
        //
        // Reduce the upper triangle of A
        //
        a(n-1,n-1) = a(n-1,n-1).x;
        for(i = n-2; i >= 0; i--)
        {
            
            //
            // Generate elementary reflector H = I+1 - tau * v * v'
            //
            alpha = a(i,i+1);
            t(1) = alpha;
            if( i>=1 )
            {
                i1_ = (0) - (2);
                for(i_=2; i_<=i+1;i_++)
                {
                    t(i_) = a(i_+i1_,i+1);
                }
            }
            complexgeneratereflection(t, i+1, taui);
            if( i>=1 )
            {
                i1_ = (2) - (0);
                for(i_=0; i_<=i-1;i_++)
                {
                    a(i_,i+1) = t(i_+i1_);
                }
            }
            alpha = t(1);
            e(i) = alpha.x;
            if( taui!=0 )
            {
                
                //
                // Apply H(I+1) from both sides to A
                //
                a(i,i+1) = 1;
                
                //
                // Compute  x := tau * A * v  storing x in TAU
                //
                i1_ = (0) - (1);
                for(i_=1; i_<=i+1;i_++)
                {
                    t(i_) = a(i_+i1_,i+1);
                }
                hermitianmatrixvectormultiply(a, isupper, 0, i, t, taui, t2);
                i1_ = (1) - (0);
                for(i_=0; i_<=i;i_++)
                {
                    tau(i_) = t2(i_+i1_);
                }
                
                //
                // Compute  w := x - 1/2 * tau * (x'*v) * v
                //
                v = 0.0;
                for(i_=0; i_<=i;i_++)
                {
                    v += ap::conj(tau(i_))*a(i_,i+1);
                }
                alpha = -0.5*taui*v;
                for(i_=0; i_<=i;i_++)
                {
                    tau(i_) = tau(i_) + alpha*a(i_,i+1);
                }
                
                //
                // Apply the transformation as a rank-2 update:
                //    A := A - v * w' - w * v'
                //
                i1_ = (0) - (1);
                for(i_=1; i_<=i+1;i_++)
                {
                    t(i_) = a(i_+i1_,i+1);
                }
                i1_ = (0) - (1);
                for(i_=1; i_<=i+1;i_++)
                {
                    t3(i_) = tau(i_+i1_);
                }
                hermitianrank2update(a, isupper, 0, i, t, t3, t2, -1);
            }
            else
            {
                a(i,i) = a(i,i).x;
            }
            a(i,i+1) = e(i);
            d(i+1) = a(i+1,i+1).x;
            tau(i) = taui;
        }
        d(0) = a(0,0).x;
    }
    else
    {
        
        //
        // Reduce the lower triangle of A
        //
        a(0,0) = a(0,0).x;
        for(i = 0; i <= n-2; i++)
        {
            
            //
            // Generate elementary reflector H = I - tau * v * v'
            //
            i1_ = (i+1) - (1);
            for(i_=1; i_<=n-i-1;i_++)
            {
                t(i_) = a(i_+i1_,i);
            }
            complexgeneratereflection(t, n-i-1, taui);
            i1_ = (1) - (i+1);
            for(i_=i+1; i_<=n-1;i_++)
            {
                a(i_,i) = t(i_+i1_);
            }
            e(i) = a(i+1,i).x;
            if( taui!=0 )
            {
                
                //
                // Apply H(i) from both sides to A(i+1:n,i+1:n)
                //
                a(i+1,i) = 1;
                
                //
                // Compute  x := tau * A * v  storing y in TAU
                //
                i1_ = (i+1) - (1);
                for(i_=1; i_<=n-i-1;i_++)
                {
                    t(i_) = a(i_+i1_,i);
                }
                hermitianmatrixvectormultiply(a, isupper, i+1, n-1, t, taui, t2);
                i1_ = (1) - (i);
                for(i_=i; i_<=n-2;i_++)
                {
                    tau(i_) = t2(i_+i1_);
                }
                
                //
                // Compute  w := x - 1/2 * tau * (x'*v) * v
                //
                i1_ = (i+1)-(i);
                v = 0.0;
                for(i_=i; i_<=n-2;i_++)
                {
                    v += ap::conj(tau(i_))*a(i_+i1_,i);
                }
                alpha = -0.5*taui*v;
                i1_ = (i+1) - (i);
                for(i_=i; i_<=n-2;i_++)
                {
                    tau(i_) = tau(i_) + alpha*a(i_+i1_,i);
                }
                
                //
                // Apply the transformation as a rank-2 update:
                // A := A - v * w' - w * v'
                //
                i1_ = (i+1) - (1);
                for(i_=1; i_<=n-i-1;i_++)
                {
                    t(i_) = a(i_+i1_,i);
                }
                i1_ = (i) - (1);
                for(i_=1; i_<=n-i-1;i_++)
                {
                    t2(i_) = tau(i_+i1_);
                }
                hermitianrank2update(a, isupper, i+1, n-1, t, t2, t3, -1);
            }
            else
            {
                a(i+1,i+1) = a(i+1,i+1).x;
            }
            a(i+1,i) = e(i);
            d(i) = a(i,i).x;
            tau(i) = taui;
        }
        d(n-1) = a(n-1,n-1).x;
    }
}


/*************************************************************************
Unpacking matrix Q which reduces a Hermitian matrix to a real  tridiagonal
form.

Input parameters:
    A       -   the result of a HMatrixTD subroutine
    N       -   size of matrix A.
    IsUpper -   storage format (a parameter of HMatrixTD subroutine)
    Tau     -   the result of a HMatrixTD subroutine

Output parameters:
    Q       -   transformation matrix.
                array with elements [0..N-1, 0..N-1].

  -- ALGLIB --
     Copyright 2005, 2007, 2008 by Bochkanov Sergey
*************************************************************************/
void hmatrixtdunpackq(const ap::complex_2d_array& a,
     const int& n,
     const bool& isupper,
     const ap::complex_1d_array& tau,
     ap::complex_2d_array& q)
{
    int i;
    int j;
    ap::complex_1d_array v;
    ap::complex_1d_array work;
    int i_;
    int i1_;

    if( n==0 )
    {
        return;
    }
    
    //
    // init
    //
    q.setbounds(0, n-1, 0, n-1);
    v.setbounds(1, n);
    work.setbounds(0, n-1);
    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            if( i==j )
            {
                q(i,j) = 1;
            }
            else
            {
                q(i,j) = 0;
            }
        }
    }
    
    //
    // unpack Q
    //
    if( isupper )
    {
        for(i = 0; i <= n-2; i++)
        {
            
            //
            // Apply H(i)
            //
            i1_ = (0) - (1);
            for(i_=1; i_<=i+1;i_++)
            {
                v(i_) = a(i_+i1_,i+1);
            }
            v(i+1) = 1;
            complexapplyreflectionfromtheleft(q, tau(i), v, 0, i, 0, n-1, work);
        }
    }
    else
    {
        for(i = n-2; i >= 0; i--)
        {
            
            //
            // Apply H(i)
            //
            i1_ = (i+1) - (1);
            for(i_=1; i_<=n-i-1;i_++)
            {
                v(i_) = a(i_+i1_,i);
            }
            v(1) = 1;
            complexapplyreflectionfromtheleft(q, tau(i), v, i+1, n-1, 0, n-1, work);
        }
    }
}


/*************************************************************************

  -- LAPACK routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     October 31, 1992
*************************************************************************/
void hermitiantotridiagonal(ap::complex_2d_array& a,
     int n,
     bool isupper,
     ap::complex_1d_array& tau,
     ap::real_1d_array& d,
     ap::real_1d_array& e)
{
    int i;
    ap::complex alpha;
    ap::complex taui;
    ap::complex v;
    ap::complex_1d_array t;
    ap::complex_1d_array t2;
    ap::complex_1d_array t3;
    int i_;
    int i1_;

    if( n<=0 )
    {
        return;
    }
    for(i = 1; i <= n; i++)
    {
        ap::ap_error::make_assertion(ap::fp_eq(a(i,i).y,0), "");
    }
    tau.setbounds(1, ap::maxint(1, n-1));
    d.setbounds(1, n);
    e.setbounds(1, ap::maxint(1, n-1));
    t.setbounds(1, n);
    t2.setbounds(1, n);
    t3.setbounds(1, n);
    if( isupper )
    {
        
        //
        // Reduce the upper triangle of A
        //
        a(n,n) = a(n,n).x;
        for(i = n-1; i >= 1; i--)
        {
            
            //
            // Generate elementary reflector H(i) = I - tau * v * v'
            // to annihilate A(1:i-1,i+1)
            //
            alpha = a(i,i+1);
            t(1) = alpha;
            if( i>=2 )
            {
                i1_ = (1) - (2);
                for(i_=2; i_<=i;i_++)
                {
                    t(i_) = a(i_+i1_,i+1);
                }
            }
            complexgeneratereflection(t, i, taui);
            if( i>=2 )
            {
                i1_ = (2) - (1);
                for(i_=1; i_<=i-1;i_++)
                {
                    a(i_,i+1) = t(i_+i1_);
                }
            }
            alpha = t(1);
            e(i) = alpha.x;
            if( taui!=0 )
            {
                
                //
                // Apply H(i) from both sides to A(1:i,1:i)
                //
                a(i,i+1) = 1;
                
                //
                // Compute  x := tau * A * v  storing x in TAU(1:i)
                //
                for(i_=1; i_<=i;i_++)
                {
                    t(i_) = a(i_,i+1);
                }
                hermitianmatrixvectormultiply(a, isupper, 1, i, t, taui, tau);
                
                //
                // Compute  w := x - 1/2 * tau * (x'*v) * v
                //
                v = 0.0;
                for(i_=1; i_<=i;i_++)
                {
                    v += ap::conj(tau(i_))*a(i_,i+1);
                }
                alpha = -0.5*taui*v;
                for(i_=1; i_<=i;i_++)
                {
                    tau(i_) = tau(i_) + alpha*a(i_,i+1);
                }
                
                //
                // Apply the transformation as a rank-2 update:
                //    A := A - v * w' - w * v'
                //
                for(i_=1; i_<=i;i_++)
                {
                    t(i_) = a(i_,i+1);
                }
                hermitianrank2update(a, isupper, 1, i, t, tau, t2, -1);
            }
            else
            {
                a(i,i) = a(i,i).x;
            }
            a(i,i+1) = e(i);
            d(i+1) = a(i+1,i+1).x;
            tau(i) = taui;
        }
        d(1) = a(1,1).x;
    }
    else
    {
        
        //
        // Reduce the lower triangle of A
        //
        a(1,1) = a(1,1).x;
        for(i = 1; i <= n-1; i++)
        {
            
            //
            // Generate elementary reflector H(i) = I - tau * v * v'
            // to annihilate A(i+2:n,i)
            //
            i1_ = (i+1) - (1);
            for(i_=1; i_<=n-i;i_++)
            {
                t(i_) = a(i_+i1_,i);
            }
            complexgeneratereflection(t, n-i, taui);
            i1_ = (1) - (i+1);
            for(i_=i+1; i_<=n;i_++)
            {
                a(i_,i) = t(i_+i1_);
            }
            e(i) = a(i+1,i).x;
            if( taui!=0 )
            {
                
                //
                // Apply H(i) from both sides to A(i+1:n,i+1:n)
                //
                a(i+1,i) = 1;
                
                //
                // Compute  x := tau * A * v  storing y in TAU(i:n-1)
                //
                i1_ = (i+1) - (1);
                for(i_=1; i_<=n-i;i_++)
                {
                    t(i_) = a(i_+i1_,i);
                }
                hermitianmatrixvectormultiply(a, isupper, i+1, n, t, taui, t2);
                i1_ = (1) - (i);
                for(i_=i; i_<=n-1;i_++)
                {
                    tau(i_) = t2(i_+i1_);
                }
                
                //
                // Compute  w := x - 1/2 * tau * (x'*v) * v
                //
                i1_ = (i+1)-(i);
                v = 0.0;
                for(i_=i; i_<=n-1;i_++)
                {
                    v += ap::conj(tau(i_))*a(i_+i1_,i);
                }
                alpha = -0.5*taui*v;
                i1_ = (i+1) - (i);
                for(i_=i; i_<=n-1;i_++)
                {
                    tau(i_) = tau(i_) + alpha*a(i_+i1_,i);
                }
                
                //
                // Apply the transformation as a rank-2 update:
                // A := A - v * w' - w * v'
                //
                i1_ = (i+1) - (1);
                for(i_=1; i_<=n-i;i_++)
                {
                    t(i_) = a(i_+i1_,i);
                }
                i1_ = (i) - (1);
                for(i_=1; i_<=n-i;i_++)
                {
                    t2(i_) = tau(i_+i1_);
                }
                hermitianrank2update(a, isupper, i+1, n, t, t2, t3, -1);
            }
            else
            {
                a(i+1,i+1) = a(i+1,i+1).x;
            }
            a(i+1,i) = e(i);
            d(i) = a(i,i).x;
            tau(i) = taui;
        }
        d(n) = a(n,n).x;
    }
}


/*************************************************************************

  -- ALGLIB --
     Copyright 2005, 2007 by Bochkanov Sergey
*************************************************************************/
void unpackqfromhermitiantridiagonal(const ap::complex_2d_array& a,
     const int& n,
     const bool& isupper,
     const ap::complex_1d_array& tau,
     ap::complex_2d_array& q)
{
    int i;
    int j;
    ap::complex_1d_array v;
    ap::complex_1d_array work;
    int i_;
    int i1_;

    if( n==0 )
    {
        return;
    }
    
    //
    // init
    //
    q.setbounds(1, n, 1, n);
    v.setbounds(1, n);
    work.setbounds(1, n);
    for(i = 1; i <= n; i++)
    {
        for(j = 1; j <= n; j++)
        {
            if( i==j )
            {
                q(i,j) = 1;
            }
            else
            {
                q(i,j) = 0;
            }
        }
    }
    
    //
    // unpack Q
    //
    if( isupper )
    {
        for(i = 1; i <= n-1; i++)
        {
            
            //
            // Apply H(i)
            //
            for(i_=1; i_<=i;i_++)
            {
                v(i_) = a(i_,i+1);
            }
            v(i) = 1;
            complexapplyreflectionfromtheleft(q, tau(i), v, 1, i, 1, n, work);
        }
    }
    else
    {
        for(i = n-1; i >= 1; i--)
        {
            
            //
            // Apply H(i)
            //
            i1_ = (i+1) - (1);
            for(i_=1; i_<=n-i;i_++)
            {
                v(i_) = a(i_+i1_,i);
            }
            v(1) = 1;
            complexapplyreflectionfromtheleft(q, tau(i), v, i+1, n, 1, n, work);
        }
    }
}




