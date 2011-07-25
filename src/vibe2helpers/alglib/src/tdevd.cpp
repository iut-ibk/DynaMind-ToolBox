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
#include "tdevd.h"

static void tdevde2(const double& a,
     const double& b,
     const double& c,
     double& rt1,
     double& rt2);
static void tdevdev2(const double& a,
     const double& b,
     const double& c,
     double& rt1,
     double& rt2,
     double& cs1,
     double& sn1);
static double tdevdpythag(double a, double b);
static double tdevdextsign(double a, double b);

/*************************************************************************
Finding the eigenvalues and eigenvectors of a tridiagonal symmetric matrix

The algorithm finds the eigen pairs of a tridiagonal symmetric matrix by
using an QL/QR algorithm with implicit shifts.

Input parameters:
    D       -   the main diagonal of a tridiagonal matrix.
                Array whose index ranges within [0..N-1].
    E       -   the secondary diagonal of a tridiagonal matrix.
                Array whose index ranges within [0..N-2].
    N       -   size of matrix A.
    ZNeeded -   flag controlling whether the eigenvectors are needed or not.
                If ZNeeded is equal to:
                 * 0, the eigenvectors are not needed;
                 * 1, the eigenvectors of a tridiagonal matrix
                   are multiplied by the square matrix Z. It is used if the
                   tridiagonal matrix is obtained by the similarity
                   transformation of a symmetric matrix;
                 * 2, the eigenvectors of a tridiagonal matrix replace the
                   square matrix Z;
                 * 3, matrix Z contains the first row of the eigenvectors
                   matrix.
    Z       -   if ZNeeded=1, Z contains the square matrix by which the
                eigenvectors are multiplied.
                Array whose indexes range within [0..N-1, 0..N-1].

Output parameters:
    D       -   eigenvalues in ascending order.
                Array whose index ranges within [0..N-1].
    Z       -   if ZNeeded is equal to:
                 * 0, Z hasn’t changed;
                 * 1, Z contains the product of a given matrix (from the left)
                   and the eigenvectors matrix (from the right);
                 * 2, Z contains the eigenvectors.
                 * 3, Z contains the first row of the eigenvectors matrix.
                If ZNeeded<3, Z is the array whose indexes range within [0..N-1, 0..N-1].
                In that case, the eigenvectors are stored in the matrix columns.
                If ZNeeded=3, Z is the array whose indexes range within [0..0, 0..N-1].

Result:
    True, if the algorithm has converged.
    False, if the algorithm hasn't converged.

  -- LAPACK routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     September 30, 1994
*************************************************************************/
bool smatrixtdevd(ap::real_1d_array& d,
     ap::real_1d_array e,
     int n,
     int zneeded,
     ap::real_2d_array& z)
{
    bool result;
    ap::real_1d_array d1;
    ap::real_1d_array e1;
    ap::real_2d_array z1;
    int i;

    
    //
    // Prepare 1-based task
    //
    d1.setbounds(1, n);
    e1.setbounds(1, n);
    ap::vmove(&d1(1), &d(0), ap::vlen(1,n));
    if( n>1 )
    {
        ap::vmove(&e1(1), &e(0), ap::vlen(1,n-1));
    }
    if( zneeded==1 )
    {
        z1.setbounds(1, n, 1, n);
        for(i = 1; i <= n; i++)
        {
            ap::vmove(&z1(i, 1), &z(i-1, 0), ap::vlen(1,n));
        }
    }
    
    //
    // Solve 1-based task
    //
    result = tridiagonalevd(d1, e1, n, zneeded, z1);
    if( !result )
    {
        return result;
    }
    
    //
    // Convert back to 0-based result
    //
    ap::vmove(&d(0), &d1(1), ap::vlen(0,n-1));
    if( zneeded!=0 )
    {
        if( zneeded==1 )
        {
            for(i = 1; i <= n; i++)
            {
                ap::vmove(&z(i-1, 0), &z1(i, 1), ap::vlen(0,n-1));
            }
            return result;
        }
        if( zneeded==2 )
        {
            z.setbounds(0, n-1, 0, n-1);
            for(i = 1; i <= n; i++)
            {
                ap::vmove(&z(i-1, 0), &z1(i, 1), ap::vlen(0,n-1));
            }
            return result;
        }
        if( zneeded==3 )
        {
            z.setbounds(0, 0, 0, n-1);
            ap::vmove(&z(0, 0), &z1(1, 1), ap::vlen(0,n-1));
            return result;
        }
        ap::ap_error::make_assertion(false, "SMatrixTDEVD: Incorrect ZNeeded!");
    }
    return result;
}


bool tridiagonalevd(ap::real_1d_array& d,
     ap::real_1d_array e,
     int n,
     int zneeded,
     ap::real_2d_array& z)
{
    bool result;
    int maxit;
    int i;
    int icompz;
    int ii;
    int iscale;
    int j;
    int jtot;
    int k;
    int t;
    int l;
    int l1;
    int lend;
    int lendm1;
    int lendp1;
    int lendsv;
    int lm1;
    int lsv;
    int m;
    int mm;
    int mm1;
    int nm1;
    int nmaxit;
    int tmpint;
    double anorm;
    double b;
    double c;
    double eps;
    double eps2;
    double f;
    double g;
    double p;
    double r;
    double rt1;
    double rt2;
    double s;
    double safmax;
    double safmin;
    double ssfmax;
    double ssfmin;
    double tst;
    double tmp;
    ap::real_1d_array work1;
    ap::real_1d_array work2;
    ap::real_1d_array workc;
    ap::real_1d_array works;
    ap::real_1d_array wtemp;
    bool gotoflag;
    int zrows;
    bool wastranspose;

    ap::ap_error::make_assertion(zneeded>=0&&zneeded<=3, "TridiagonalEVD: Incorrent ZNeeded");
    
    //
    // Quick return if possible
    //
    if( zneeded<0||zneeded>3 )
    {
        result = false;
        return result;
    }
    result = true;
    if( n==0 )
    {
        return result;
    }
    if( n==1 )
    {
        if( zneeded==2||zneeded==3 )
        {
            z.setbounds(1, 1, 1, 1);
            z(1,1) = 1;
        }
        return result;
    }
    maxit = 30;
    
    //
    // Initialize arrays
    //
    wtemp.setbounds(1, n);
    work1.setbounds(1, n-1);
    work2.setbounds(1, n-1);
    workc.setbounds(1, n);
    works.setbounds(1, n);
    
    //
    // Determine the unit roundoff and over/underflow thresholds.
    //
    eps = ap::machineepsilon;
    eps2 = ap::sqr(eps);
    safmin = ap::minrealnumber;
    safmax = ap::maxrealnumber;
    ssfmax = sqrt(safmax)/3;
    ssfmin = sqrt(safmin)/eps2;
    
    //
    // Prepare Z
    //
    // Here we are using transposition to get rid of column operations
    //
    //
    wastranspose = false;
    if( zneeded==0 )
    {
        zrows = 0;
    }
    if( zneeded==1 )
    {
        zrows = n;
    }
    if( zneeded==2 )
    {
        zrows = n;
    }
    if( zneeded==3 )
    {
        zrows = 1;
    }
    if( zneeded==1 )
    {
        wastranspose = true;
        inplacetranspose(z, 1, n, 1, n, wtemp);
    }
    if( zneeded==2 )
    {
        wastranspose = true;
        z.setbounds(1, n, 1, n);
        for(i = 1; i <= n; i++)
        {
            for(j = 1; j <= n; j++)
            {
                if( i==j )
                {
                    z(i,j) = 1;
                }
                else
                {
                    z(i,j) = 0;
                }
            }
        }
    }
    if( zneeded==3 )
    {
        wastranspose = false;
        z.setbounds(1, 1, 1, n);
        for(j = 1; j <= n; j++)
        {
            if( j==1 )
            {
                z(1,j) = 1;
            }
            else
            {
                z(1,j) = 0;
            }
        }
    }
    nmaxit = n*maxit;
    jtot = 0;
    
    //
    // Determine where the matrix splits and choose QL or QR iteration
    // for each block, according to whether top or bottom diagonal
    // element is smaller.
    //
    l1 = 1;
    nm1 = n-1;
    while(true)
    {
        if( l1>n )
        {
            break;
        }
        if( l1>1 )
        {
            e(l1-1) = 0;
        }
        gotoflag = false;
        if( l1<=nm1 )
        {
            for(m = l1; m <= nm1; m++)
            {
                tst = fabs(e(m));
                if( ap::fp_eq(tst,0) )
                {
                    gotoflag = true;
                    break;
                }
                if( ap::fp_less_eq(tst,sqrt(fabs(d(m)))*sqrt(fabs(d(m+1)))*eps) )
                {
                    e(m) = 0;
                    gotoflag = true;
                    break;
                }
            }
        }
        if( !gotoflag )
        {
            m = n;
        }
        
        //
        // label 30:
        //
        l = l1;
        lsv = l;
        lend = m;
        lendsv = lend;
        l1 = m+1;
        if( lend==l )
        {
            continue;
        }
        
        //
        // Scale submatrix in rows and columns L to LEND
        //
        if( l==lend )
        {
            anorm = fabs(d(l));
        }
        else
        {
            anorm = ap::maxreal(fabs(d(l))+fabs(e(l)), fabs(e(lend-1))+fabs(d(lend)));
            for(i = l+1; i <= lend-1; i++)
            {
                anorm = ap::maxreal(anorm, fabs(d(i))+fabs(e(i))+fabs(e(i-1)));
            }
        }
        iscale = 0;
        if( ap::fp_eq(anorm,0) )
        {
            continue;
        }
        if( ap::fp_greater(anorm,ssfmax) )
        {
            iscale = 1;
            tmp = ssfmax/anorm;
            tmpint = lend-1;
            ap::vmul(&d(l), ap::vlen(l,lend), tmp);
            ap::vmul(&e(l), ap::vlen(l,tmpint), tmp);
        }
        if( ap::fp_less(anorm,ssfmin) )
        {
            iscale = 2;
            tmp = ssfmin/anorm;
            tmpint = lend-1;
            ap::vmul(&d(l), ap::vlen(l,lend), tmp);
            ap::vmul(&e(l), ap::vlen(l,tmpint), tmp);
        }
        
        //
        // Choose between QL and QR iteration
        //
        if( ap::fp_less(fabs(d(lend)),fabs(d(l))) )
        {
            lend = lsv;
            l = lendsv;
        }
        if( lend>l )
        {
            
            //
            // QL Iteration
            //
            // Look for small subdiagonal element.
            //
            while(true)
            {
                gotoflag = false;
                if( l!=lend )
                {
                    lendm1 = lend-1;
                    for(m = l; m <= lendm1; m++)
                    {
                        tst = ap::sqr(fabs(e(m)));
                        if( ap::fp_less_eq(tst,eps2*fabs(d(m))*fabs(d(m+1))+safmin) )
                        {
                            gotoflag = true;
                            break;
                        }
                    }
                }
                if( !gotoflag )
                {
                    m = lend;
                }
                if( m<lend )
                {
                    e(m) = 0;
                }
                p = d(l);
                if( m!=l )
                {
                    
                    //
                    // If remaining matrix is 2-by-2, use DLAE2 or SLAEV2
                    // to compute its eigensystem.
                    //
                    if( m==l+1 )
                    {
                        if( zneeded>0 )
                        {
                            tdevdev2(d(l), e(l), d(l+1), rt1, rt2, c, s);
                            work1(l) = c;
                            work2(l) = s;
                            workc(1) = work1(l);
                            works(1) = work2(l);
                            if( !wastranspose )
                            {
                                applyrotationsfromtheright(false, 1, zrows, l, l+1, workc, works, z, wtemp);
                            }
                            else
                            {
                                applyrotationsfromtheleft(false, l, l+1, 1, zrows, workc, works, z, wtemp);
                            }
                        }
                        else
                        {
                            tdevde2(d(l), e(l), d(l+1), rt1, rt2);
                        }
                        d(l) = rt1;
                        d(l+1) = rt2;
                        e(l) = 0;
                        l = l+2;
                        if( l<=lend )
                        {
                            continue;
                        }
                        
                        //
                        // GOTO 140
                        //
                        break;
                    }
                    if( jtot==nmaxit )
                    {
                        
                        //
                        // GOTO 140
                        //
                        break;
                    }
                    jtot = jtot+1;
                    
                    //
                    // Form shift.
                    //
                    g = (d(l+1)-p)/(2*e(l));
                    r = tdevdpythag(g, double(1));
                    g = d(m)-p+e(l)/(g+tdevdextsign(r, g));
                    s = 1;
                    c = 1;
                    p = 0;
                    
                    //
                    // Inner loop
                    //
                    mm1 = m-1;
                    for(i = mm1; i >= l; i--)
                    {
                        f = s*e(i);
                        b = c*e(i);
                        generaterotation(g, f, c, s, r);
                        if( i!=m-1 )
                        {
                            e(i+1) = r;
                        }
                        g = d(i+1)-p;
                        r = (d(i)-g)*s+2*c*b;
                        p = s*r;
                        d(i+1) = g+p;
                        g = c*r-b;
                        
                        //
                        // If eigenvectors are desired, then save rotations.
                        //
                        if( zneeded>0 )
                        {
                            work1(i) = c;
                            work2(i) = -s;
                        }
                    }
                    
                    //
                    // If eigenvectors are desired, then apply saved rotations.
                    //
                    if( zneeded>0 )
                    {
                        for(i = l; i <= m-1; i++)
                        {
                            workc(i-l+1) = work1(i);
                            works(i-l+1) = work2(i);
                        }
                        if( !wastranspose )
                        {
                            applyrotationsfromtheright(false, 1, zrows, l, m, workc, works, z, wtemp);
                        }
                        else
                        {
                            applyrotationsfromtheleft(false, l, m, 1, zrows, workc, works, z, wtemp);
                        }
                    }
                    d(l) = d(l)-p;
                    e(l) = g;
                    continue;
                }
                
                //
                // Eigenvalue found.
                //
                d(l) = p;
                l = l+1;
                if( l<=lend )
                {
                    continue;
                }
                break;
            }
        }
        else
        {
            
            //
            // QR Iteration
            //
            // Look for small superdiagonal element.
            //
            while(true)
            {
                gotoflag = false;
                if( l!=lend )
                {
                    lendp1 = lend+1;
                    for(m = l; m >= lendp1; m--)
                    {
                        tst = ap::sqr(fabs(e(m-1)));
                        if( ap::fp_less_eq(tst,eps2*fabs(d(m))*fabs(d(m-1))+safmin) )
                        {
                            gotoflag = true;
                            break;
                        }
                    }
                }
                if( !gotoflag )
                {
                    m = lend;
                }
                if( m>lend )
                {
                    e(m-1) = 0;
                }
                p = d(l);
                if( m!=l )
                {
                    
                    //
                    // If remaining matrix is 2-by-2, use DLAE2 or SLAEV2
                    // to compute its eigensystem.
                    //
                    if( m==l-1 )
                    {
                        if( zneeded>0 )
                        {
                            tdevdev2(d(l-1), e(l-1), d(l), rt1, rt2, c, s);
                            work1(m) = c;
                            work2(m) = s;
                            workc(1) = c;
                            works(1) = s;
                            if( !wastranspose )
                            {
                                applyrotationsfromtheright(true, 1, zrows, l-1, l, workc, works, z, wtemp);
                            }
                            else
                            {
                                applyrotationsfromtheleft(true, l-1, l, 1, zrows, workc, works, z, wtemp);
                            }
                        }
                        else
                        {
                            tdevde2(d(l-1), e(l-1), d(l), rt1, rt2);
                        }
                        d(l-1) = rt1;
                        d(l) = rt2;
                        e(l-1) = 0;
                        l = l-2;
                        if( l>=lend )
                        {
                            continue;
                        }
                        break;
                    }
                    if( jtot==nmaxit )
                    {
                        break;
                    }
                    jtot = jtot+1;
                    
                    //
                    // Form shift.
                    //
                    g = (d(l-1)-p)/(2*e(l-1));
                    r = tdevdpythag(g, double(1));
                    g = d(m)-p+e(l-1)/(g+tdevdextsign(r, g));
                    s = 1;
                    c = 1;
                    p = 0;
                    
                    //
                    // Inner loop
                    //
                    lm1 = l-1;
                    for(i = m; i <= lm1; i++)
                    {
                        f = s*e(i);
                        b = c*e(i);
                        generaterotation(g, f, c, s, r);
                        if( i!=m )
                        {
                            e(i-1) = r;
                        }
                        g = d(i)-p;
                        r = (d(i+1)-g)*s+2*c*b;
                        p = s*r;
                        d(i) = g+p;
                        g = c*r-b;
                        
                        //
                        // If eigenvectors are desired, then save rotations.
                        //
                        if( zneeded>0 )
                        {
                            work1(i) = c;
                            work2(i) = s;
                        }
                    }
                    
                    //
                    // If eigenvectors are desired, then apply saved rotations.
                    //
                    if( zneeded>0 )
                    {
                        mm = l-m+1;
                        for(i = m; i <= l-1; i++)
                        {
                            workc(i-m+1) = work1(i);
                            works(i-m+1) = work2(i);
                        }
                        if( !wastranspose )
                        {
                            applyrotationsfromtheright(true, 1, zrows, m, l, workc, works, z, wtemp);
                        }
                        else
                        {
                            applyrotationsfromtheleft(true, m, l, 1, zrows, workc, works, z, wtemp);
                        }
                    }
                    d(l) = d(l)-p;
                    e(lm1) = g;
                    continue;
                }
                
                //
                // Eigenvalue found.
                //
                d(l) = p;
                l = l-1;
                if( l>=lend )
                {
                    continue;
                }
                break;
            }
        }
        
        //
        // Undo scaling if necessary
        //
        if( iscale==1 )
        {
            tmp = anorm/ssfmax;
            tmpint = lendsv-1;
            ap::vmul(&d(lsv), ap::vlen(lsv,lendsv), tmp);
            ap::vmul(&e(lsv), ap::vlen(lsv,tmpint), tmp);
        }
        if( iscale==2 )
        {
            tmp = anorm/ssfmin;
            tmpint = lendsv-1;
            ap::vmul(&d(lsv), ap::vlen(lsv,lendsv), tmp);
            ap::vmul(&e(lsv), ap::vlen(lsv,tmpint), tmp);
        }
        
        //
        // Check for no convergence to an eigenvalue after a total
        // of N*MAXIT iterations.
        //
        if( jtot>=nmaxit )
        {
            result = false;
            if( wastranspose )
            {
                inplacetranspose(z, 1, n, 1, n, wtemp);
            }
            return result;
        }
    }
    
    //
    // Order eigenvalues and eigenvectors.
    //
    if( zneeded==0 )
    {
        
        //
        // Sort
        //
        if( n==1 )
        {
            return result;
        }
        if( n==2 )
        {
            if( ap::fp_greater(d(1),d(2)) )
            {
                tmp = d(1);
                d(1) = d(2);
                d(2) = tmp;
            }
            return result;
        }
        i = 2;
        do
        {
            t = i;
            while(t!=1)
            {
                k = t/2;
                if( ap::fp_greater_eq(d(k),d(t)) )
                {
                    t = 1;
                }
                else
                {
                    tmp = d(k);
                    d(k) = d(t);
                    d(t) = tmp;
                    t = k;
                }
            }
            i = i+1;
        }
        while(i<=n);
        i = n-1;
        do
        {
            tmp = d(i+1);
            d(i+1) = d(1);
            d(+1) = tmp;
            t = 1;
            while(t!=0)
            {
                k = 2*t;
                if( k>i )
                {
                    t = 0;
                }
                else
                {
                    if( k<i )
                    {
                        if( ap::fp_greater(d(k+1),d(k)) )
                        {
                            k = k+1;
                        }
                    }
                    if( ap::fp_greater_eq(d(t),d(k)) )
                    {
                        t = 0;
                    }
                    else
                    {
                        tmp = d(k);
                        d(k) = d(t);
                        d(t) = tmp;
                        t = k;
                    }
                }
            }
            i = i-1;
        }
        while(i>=1);
    }
    else
    {
        
        //
        // Use Selection Sort to minimize swaps of eigenvectors
        //
        for(ii = 2; ii <= n; ii++)
        {
            i = ii-1;
            k = i;
            p = d(i);
            for(j = ii; j <= n; j++)
            {
                if( ap::fp_less(d(j),p) )
                {
                    k = j;
                    p = d(j);
                }
            }
            if( k!=i )
            {
                d(k) = d(i);
                d(i) = p;
                if( wastranspose )
                {
                    ap::vmove(&wtemp(1), &z(i, 1), ap::vlen(1,n));
                    ap::vmove(&z(i, 1), &z(k, 1), ap::vlen(1,n));
                    ap::vmove(&z(k, 1), &wtemp(1), ap::vlen(1,n));
                }
                else
                {
                    ap::vmove(wtemp.getvector(1, zrows), z.getcolumn(i, 1, zrows));
                    ap::vmove(z.getcolumn(i, 1, zrows), z.getcolumn(k, 1, zrows));
                    ap::vmove(z.getcolumn(k, 1, zrows), wtemp.getvector(1, zrows));
                }
            }
        }
        if( wastranspose )
        {
            inplacetranspose(z, 1, n, 1, n, wtemp);
        }
    }
    return result;
}


/*************************************************************************
DLAE2  computes the eigenvalues of a 2-by-2 symmetric matrix
   [  A   B  ]
   [  B   C  ].
On return, RT1 is the eigenvalue of larger absolute value, and RT2
is the eigenvalue of smaller absolute value.

  -- LAPACK auxiliary routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     October 31, 1992
*************************************************************************/
static void tdevde2(const double& a,
     const double& b,
     const double& c,
     double& rt1,
     double& rt2)
{
    double ab;
    double acmn;
    double acmx;
    double adf;
    double df;
    double rt;
    double sm;
    double tb;

    sm = a+c;
    df = a-c;
    adf = fabs(df);
    tb = b+b;
    ab = fabs(tb);
    if( ap::fp_greater(fabs(a),fabs(c)) )
    {
        acmx = a;
        acmn = c;
    }
    else
    {
        acmx = c;
        acmn = a;
    }
    if( ap::fp_greater(adf,ab) )
    {
        rt = adf*sqrt(1+ap::sqr(ab/adf));
    }
    else
    {
        if( ap::fp_less(adf,ab) )
        {
            rt = ab*sqrt(1+ap::sqr(adf/ab));
        }
        else
        {
            
            //
            // Includes case AB=ADF=0
            //
            rt = ab*sqrt(double(2));
        }
    }
    if( ap::fp_less(sm,0) )
    {
        rt1 = 0.5*(sm-rt);
        
        //
        // Order of execution important.
        // To get fully accurate smaller eigenvalue,
        // next line needs to be executed in higher precision.
        //
        rt2 = acmx/rt1*acmn-b/rt1*b;
    }
    else
    {
        if( ap::fp_greater(sm,0) )
        {
            rt1 = 0.5*(sm+rt);
            
            //
            // Order of execution important.
            // To get fully accurate smaller eigenvalue,
            // next line needs to be executed in higher precision.
            //
            rt2 = acmx/rt1*acmn-b/rt1*b;
        }
        else
        {
            
            //
            // Includes case RT1 = RT2 = 0
            //
            rt1 = 0.5*rt;
            rt2 = -0.5*rt;
        }
    }
}


/*************************************************************************
DLAEV2 computes the eigendecomposition of a 2-by-2 symmetric matrix

   [  A   B  ]
   [  B   C  ].

On return, RT1 is the eigenvalue of larger absolute value, RT2 is the
eigenvalue of smaller absolute value, and (CS1,SN1) is the unit right
eigenvector for RT1, giving the decomposition

   [ CS1  SN1 ] [  A   B  ] [ CS1 -SN1 ]  =  [ RT1  0  ]
   [-SN1  CS1 ] [  B   C  ] [ SN1  CS1 ]     [  0  RT2 ].


  -- LAPACK auxiliary routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     October 31, 1992
*************************************************************************/
static void tdevdev2(const double& a,
     const double& b,
     const double& c,
     double& rt1,
     double& rt2,
     double& cs1,
     double& sn1)
{
    int sgn1;
    int sgn2;
    double ab;
    double acmn;
    double acmx;
    double acs;
    double adf;
    double cs;
    double ct;
    double df;
    double rt;
    double sm;
    double tb;
    double tn;

    
    //
    // Compute the eigenvalues
    //
    sm = a+c;
    df = a-c;
    adf = fabs(df);
    tb = b+b;
    ab = fabs(tb);
    if( ap::fp_greater(fabs(a),fabs(c)) )
    {
        acmx = a;
        acmn = c;
    }
    else
    {
        acmx = c;
        acmn = a;
    }
    if( ap::fp_greater(adf,ab) )
    {
        rt = adf*sqrt(1+ap::sqr(ab/adf));
    }
    else
    {
        if( ap::fp_less(adf,ab) )
        {
            rt = ab*sqrt(1+ap::sqr(adf/ab));
        }
        else
        {
            
            //
            // Includes case AB=ADF=0
            //
            rt = ab*sqrt(double(2));
        }
    }
    if( ap::fp_less(sm,0) )
    {
        rt1 = 0.5*(sm-rt);
        sgn1 = -1;
        
        //
        // Order of execution important.
        // To get fully accurate smaller eigenvalue,
        // next line needs to be executed in higher precision.
        //
        rt2 = acmx/rt1*acmn-b/rt1*b;
    }
    else
    {
        if( ap::fp_greater(sm,0) )
        {
            rt1 = 0.5*(sm+rt);
            sgn1 = 1;
            
            //
            // Order of execution important.
            // To get fully accurate smaller eigenvalue,
            // next line needs to be executed in higher precision.
            //
            rt2 = acmx/rt1*acmn-b/rt1*b;
        }
        else
        {
            
            //
            // Includes case RT1 = RT2 = 0
            //
            rt1 = 0.5*rt;
            rt2 = -0.5*rt;
            sgn1 = 1;
        }
    }
    
    //
    // Compute the eigenvector
    //
    if( ap::fp_greater_eq(df,0) )
    {
        cs = df+rt;
        sgn2 = 1;
    }
    else
    {
        cs = df-rt;
        sgn2 = -1;
    }
    acs = fabs(cs);
    if( ap::fp_greater(acs,ab) )
    {
        ct = -tb/cs;
        sn1 = 1/sqrt(1+ct*ct);
        cs1 = ct*sn1;
    }
    else
    {
        if( ap::fp_eq(ab,0) )
        {
            cs1 = 1;
            sn1 = 0;
        }
        else
        {
            tn = -cs/tb;
            cs1 = 1/sqrt(1+tn*tn);
            sn1 = tn*cs1;
        }
    }
    if( sgn1==sgn2 )
    {
        tn = cs1;
        cs1 = -sn1;
        sn1 = tn;
    }
}


/*************************************************************************
Internal routine
*************************************************************************/
static double tdevdpythag(double a, double b)
{
    double result;

    if( ap::fp_less(fabs(a),fabs(b)) )
    {
        result = fabs(b)*sqrt(1+ap::sqr(a/b));
    }
    else
    {
        result = fabs(a)*sqrt(1+ap::sqr(b/a));
    }
    return result;
}


/*************************************************************************
Internal routine
*************************************************************************/
static double tdevdextsign(double a, double b)
{
    double result;

    if( ap::fp_greater_eq(b,0) )
    {
        result = fabs(a);
    }
    else
    {
        result = -fabs(a);
    }
    return result;
}




