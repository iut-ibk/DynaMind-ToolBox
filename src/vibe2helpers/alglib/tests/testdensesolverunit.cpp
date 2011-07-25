
#include <stdafx.h>
#include <stdio.h>
#include "testdensesolverunit.h"

static void unset2d(ap::real_2d_array& x);
static void unset1d(ap::real_1d_array& x);
static void unsetrep(densesolverreport& r);
static void unsetlsrep(densesolverlsreport& r);

/*************************************************************************
Test
*************************************************************************/
bool testdensesolver(bool silent)
{
    bool result;
    ap::real_2d_array a;
    ap::real_2d_array atmp;
    ap::real_2d_array xe;
    ap::real_2d_array b;
    ap::real_1d_array bv;
    int i;
    int j;
    int k;
    int n;
    int m;
    int pass;
    int taskkind;
    double mx;
    double v;
    double verr;
    int info;
    densesolverreport rep;
    densesolverlsreport repls;
    ap::real_2d_array x;
    ap::real_1d_array xv;
    ap::real_1d_array y;
    ap::real_1d_array tx;
    int maxn;
    int maxm;
    int passcount;
    double threshold;
    double weakthreshold;
    bool genrealerrors;
    bool lsrealerrors;
    bool rfserrors;
    bool waserrors;

    maxn = 10;
    maxm = 5;
    passcount = 5;
    threshold = 10000*ap::machineepsilon;
    weakthreshold = 0.001;
    rfserrors = false;
    genrealerrors = false;
    lsrealerrors = false;
    waserrors = false;
    
    //
    // General square matrices:
    // * test general properties
    // * test iterative improvement
    //
    for(pass = 1; pass <= passcount; pass++)
    {
        for(n = 1; n <= maxn; n++)
        {
            for(m = 1; m <= maxm; m++)
            {
                
                //
                // ********************************************************
                // WELL CONDITIONED TASKS
                // ability to find correct solution is tested
                // ********************************************************
                //
                // 1. generate random well conditioned matrix A:
                //        A = R + W,
                //    where R has small random elements, and W is
                //    permutation of diagonal matrix with large
                //    random entries. So A should be well conditioned.
                // 2. generate random solution vector xe
                // 3. generate right part b=A*xe
                // 4. test different methods on original A
                //
                a.setlength(n, n);
                for(i = 0; i <= n-1; i++)
                {
                    for(j = 0; j <= n-1; j++)
                    {
                        a(i,j) = 2*ap::randomreal()-1;
                    }
                }
                k = ap::randominteger(n);
                for(i = 0; i <= n-1; i++)
                {
                    a(i,(i+k)%n) = (ap::randominteger(2)-0.5)*(5+5*ap::randomreal());
                }
                xe.setlength(n, m);
                for(i = 0; i <= n-1; i++)
                {
                    for(j = 0; j <= m-1; j++)
                    {
                        xe(i,j) = 2*ap::randomreal()-1;
                    }
                }
                b.setlength(n, m);
                for(i = 0; i <= n-1; i++)
                {
                    for(j = 0; j <= m-1; j++)
                    {
                        v = ap::vdotproduct(a.getrow(i, 0, n-1), xe.getcolumn(j, 0, n-1));
                        b(i,j) = v;
                    }
                }
                
                //
                // Test RMatrixSolveM()
                //
                info = 0;
                unsetrep(rep);
                unset2d(x);
                rmatrixsolvem(a, n, b, m, info, rep, x);
                if( info<=0 )
                {
                    genrealerrors = true;
                }
                else
                {
                    genrealerrors = genrealerrors||ap::fp_less(rep.r1,100*ap::machineepsilon)||ap::fp_greater(rep.r1,1+1000*ap::machineepsilon);
                    genrealerrors = genrealerrors||ap::fp_less(rep.rinf,100*ap::machineepsilon)||ap::fp_greater(rep.rinf,1+1000*ap::machineepsilon);
                    for(i = 0; i <= n-1; i++)
                    {
                        for(j = 0; j <= m-1; j++)
                        {
                            genrealerrors = genrealerrors||ap::fp_greater(fabs(xe(i,j)-x(i,j)),threshold);
                        }
                    }
                }
                
                //
                // Test RMatrixSolve()
                //
                info = 0;
                unsetrep(rep);
                unset2d(x);
                bv.setlength(n);
                ap::vmove(bv.getvector(0, n-1), b.getcolumn(0, 0, n-1));
                rmatrixsolve(a, n, bv, info, rep, xv);
                if( info<=0 )
                {
                    genrealerrors = true;
                }
                else
                {
                    genrealerrors = genrealerrors||ap::fp_less(rep.r1,100*ap::machineepsilon)||ap::fp_greater(rep.r1,1+1000*ap::machineepsilon);
                    genrealerrors = genrealerrors||ap::fp_less(rep.rinf,100*ap::machineepsilon)||ap::fp_greater(rep.rinf,1+1000*ap::machineepsilon);
                    for(i = 0; i <= n-1; i++)
                    {
                        genrealerrors = genrealerrors||ap::fp_greater(fabs(xe(i,0)-xv(i)),threshold);
                    }
                }
                
                //
                // Test DenseSolverRLS():
                // * test on original system A*x = b
                // * test on overdetermined system with the same solution: (A' A')'*x = b
                // * test on underdetermined system with the same solution: (A 0 0 0 ) * z = b
                //
                info = 0;
                unsetlsrep(repls);
                unset1d(xv);
                bv.setlength(n);
                ap::vmove(bv.getvector(0, n-1), b.getcolumn(0, 0, n-1));
                rmatrixsolvels(a, n, n, bv, 0.0, info, repls, xv);
                if( info<=0 )
                {
                    lsrealerrors = true;
                }
                else
                {
                    lsrealerrors = lsrealerrors||ap::fp_less(repls.r2,100*ap::machineepsilon)||ap::fp_greater(repls.r2,1+1000*ap::machineepsilon);
                    lsrealerrors = lsrealerrors||repls.n!=n||repls.k!=0;
                    for(i = 0; i <= n-1; i++)
                    {
                        lsrealerrors = lsrealerrors||ap::fp_greater(fabs(xe(i,0)-xv(i)),threshold);
                    }
                }
                info = 0;
                unsetlsrep(repls);
                unset1d(xv);
                bv.setlength(2*n);
                ap::vmove(bv.getvector(0, n-1), b.getcolumn(0, 0, n-1));
                ap::vmove(bv.getvector(n, 2*n-1), b.getcolumn(0, 0, n-1));
                atmp.setlength(2*n, n);
                copymatrix(a, 0, n-1, 0, n-1, atmp, 0, n-1, 0, n-1);
                copymatrix(a, 0, n-1, 0, n-1, atmp, n, 2*n-1, 0, n-1);
                rmatrixsolvels(atmp, 2*n, n, bv, 0.0, info, repls, xv);
                if( info<=0 )
                {
                    lsrealerrors = true;
                }
                else
                {
                    lsrealerrors = lsrealerrors||ap::fp_less(repls.r2,100*ap::machineepsilon)||ap::fp_greater(repls.r2,1+1000*ap::machineepsilon);
                    lsrealerrors = lsrealerrors||repls.n!=n||repls.k!=0;
                    for(i = 0; i <= n-1; i++)
                    {
                        lsrealerrors = lsrealerrors||ap::fp_greater(fabs(xe(i,0)-xv(i)),threshold);
                    }
                }
                info = 0;
                unsetlsrep(repls);
                unset1d(xv);
                bv.setlength(n);
                ap::vmove(bv.getvector(0, n-1), b.getcolumn(0, 0, n-1));
                atmp.setlength(n, 2*n);
                copymatrix(a, 0, n-1, 0, n-1, atmp, 0, n-1, 0, n-1);
                for(i = 0; i <= n-1; i++)
                {
                    for(j = n; j <= 2*n-1; j++)
                    {
                        atmp(i,j) = 0;
                    }
                }
                rmatrixsolvels(atmp, n, 2*n, bv, 0.0, info, repls, xv);
                if( info<=0 )
                {
                    lsrealerrors = true;
                }
                else
                {
                    lsrealerrors = lsrealerrors||ap::fp_neq(repls.r2,0);
                    lsrealerrors = lsrealerrors||repls.n!=2*n||repls.k!=n;
                    for(i = 0; i <= n-1; i++)
                    {
                        lsrealerrors = lsrealerrors||ap::fp_greater(fabs(xe(i,0)-xv(i)),threshold);
                    }
                    for(i = n; i <= 2*n-1; i++)
                    {
                        lsrealerrors = lsrealerrors||ap::fp_greater(fabs(xv(i)),threshold);
                    }
                }
                
                //
                // ********************************************************
                // EXACTLY SINGULAR MATRICES
                // ability to detect singularity is tested
                // ********************************************************
                //
                // 1. generate different types of singular matrices:
                //    * zero
                //    * with zero columns
                //    * with zero rows
                //    * with equal rows/columns
                // 2. generate random solution vector xe
                // 3. generate right part b=A*xe
                // 4. test different methods
                //
                for(taskkind = 0; taskkind <= 4; taskkind++)
                {
                    unset2d(a);
                    if( taskkind==0 )
                    {
                        
                        //
                        // all zeros
                        //
                        a.setlength(n, n);
                        for(i = 0; i <= n-1; i++)
                        {
                            for(j = 0; j <= n-1; j++)
                            {
                                a(i,j) = 0;
                            }
                        }
                    }
                    if( taskkind==1 )
                    {
                        
                        //
                        // there is zero column
                        //
                        a.setlength(n, n);
                        for(i = 0; i <= n-1; i++)
                        {
                            for(j = 0; j <= n-1; j++)
                            {
                                a(i,j) = 2*ap::randomreal()-1;
                            }
                        }
                        k = ap::randominteger(n);
                        ap::vmul(a.getcolumn(k, 0, n-1), 0);
                    }
                    if( taskkind==2 )
                    {
                        
                        //
                        // there is zero row
                        //
                        a.setlength(n, n);
                        for(i = 0; i <= n-1; i++)
                        {
                            for(j = 0; j <= n-1; j++)
                            {
                                a(i,j) = 2*ap::randomreal()-1;
                            }
                        }
                        k = ap::randominteger(n);
                        ap::vmul(&a(k, 0), ap::vlen(0,n-1), 0);
                    }
                    if( taskkind==3 )
                    {
                        
                        //
                        // equal columns
                        //
                        if( n<2 )
                        {
                            continue;
                        }
                        a.setlength(n, n);
                        for(i = 0; i <= n-1; i++)
                        {
                            for(j = 0; j <= n-1; j++)
                            {
                                a(i,j) = 2*ap::randomreal()-1;
                            }
                        }
                        k = 1+ap::randominteger(n-1);
                        ap::vmove(a.getcolumn(0, 0, n-1), a.getcolumn(k, 0, n-1));
                    }
                    if( taskkind==4 )
                    {
                        
                        //
                        // equal rows
                        //
                        if( n<2 )
                        {
                            continue;
                        }
                        a.setlength(n, n);
                        for(i = 0; i <= n-1; i++)
                        {
                            for(j = 0; j <= n-1; j++)
                            {
                                a(i,j) = 2*ap::randomreal()-1;
                            }
                        }
                        k = 1+ap::randominteger(n-1);
                        ap::vmove(&a(0, 0), &a(k, 0), ap::vlen(0,n-1));
                    }
                    xe.setlength(n, m);
                    for(i = 0; i <= n-1; i++)
                    {
                        for(j = 0; j <= m-1; j++)
                        {
                            xe(i,j) = 2*ap::randomreal()-1;
                        }
                    }
                    b.setlength(n, m);
                    for(i = 0; i <= n-1; i++)
                    {
                        for(j = 0; j <= m-1; j++)
                        {
                            v = ap::vdotproduct(a.getrow(i, 0, n-1), xe.getcolumn(j, 0, n-1));
                            b(i,j) = v;
                        }
                    }
                    
                    //
                    // Test RMatrixSolveM()
                    //
                    info = 0;
                    unsetrep(rep);
                    unset2d(x);
                    rmatrixsolvem(a, n, b, m, info, rep, x);
                    genrealerrors = genrealerrors||info!=-3;
                    genrealerrors = genrealerrors||ap::fp_less(rep.r1,0)||ap::fp_greater(rep.r1,1000*ap::machineepsilon);
                    genrealerrors = genrealerrors||ap::fp_less(rep.rinf,0)||ap::fp_greater(rep.rinf,1000*ap::machineepsilon);
                    for(i = 0; i <= n-1; i++)
                    {
                        for(j = 0; j <= m-1; j++)
                        {
                            genrealerrors = genrealerrors||ap::fp_neq(x(i,j),0);
                        }
                    }
                    
                    //
                    // Test RMatrixSolve()
                    //
                    info = 0;
                    unsetrep(rep);
                    unset2d(x);
                    bv.setlength(n);
                    ap::vmove(bv.getvector(0, n-1), b.getcolumn(0, 0, n-1));
                    rmatrixsolve(a, n, bv, info, rep, xv);
                    genrealerrors = genrealerrors||info!=-3;
                    genrealerrors = genrealerrors||ap::fp_less(rep.r1,0)||ap::fp_greater(rep.r1,1000*ap::machineepsilon);
                    genrealerrors = genrealerrors||ap::fp_less(rep.rinf,0)||ap::fp_greater(rep.rinf,1000*ap::machineepsilon);
                    for(i = 0; i <= n-1; i++)
                    {
                        genrealerrors = genrealerrors||ap::fp_neq(xv(i),0);
                    }
                }
            }
        }
    }
    for(pass = 1; pass <= passcount; pass++)
    {
        
        //
        // Test iterative improvement for real matrices
        //
        // A matrix/right part are constructed such that both matrix
        // and solution components are within (0,1). Such matrix/right part
        // have nice properties - system can be solved using iterative
        // improvement with |A*x-b| about several ulps of max(1,|b|).
        //
        n = 100;
        a.setlength(n, n);
        b.setlength(n, 1);
        bv.setlength(n);
        tx.setlength(n);
        xv.setlength(n);
        y.setlength(n);
        for(i = 0; i <= n-1; i++)
        {
            xv(i) = ap::randomreal();
        }
        for(i = 0; i <= n-1; i++)
        {
            for(j = 0; j <= n-1; j++)
            {
                a(i,j) = ap::randomreal();
            }
            ap::vmove(&y(0), &a(i, 0), ap::vlen(0,n-1));
            xdot(y, xv, n, tx, v, verr);
            bv(i) = v;
        }
        ap::vmove(b.getcolumn(0, 0, n-1), bv.getvector(0, n-1));
        
        //
        // Test RMatrixSolveM()
        //
        unset2d(x);
        rmatrixsolvem(a, n, b, 1, info, rep, x);
        if( info<=0 )
        {
            rfserrors = true;
        }
        else
        {
            xv.setlength(n);
            ap::vmove(xv.getvector(0, n-1), x.getcolumn(0, 0, n-1));
            for(i = 0; i <= n-1; i++)
            {
                ap::vmove(&y(0), &a(i, 0), ap::vlen(0,n-1));
                xdot(y, xv, n, tx, v, verr);
                rfserrors = rfserrors||ap::fp_greater(fabs(v-b(i,0)),4*ap::machineepsilon*ap::maxreal(double(1), fabs(b(i,0))));
            }
        }
        
        //
        // Test RMatrixSolve()
        //
        unset1d(xv);
        rmatrixsolve(a, n, bv, info, rep, xv);
        if( info<=0 )
        {
            rfserrors = true;
        }
        else
        {
            for(i = 0; i <= n-1; i++)
            {
                ap::vmove(&y(0), &a(i, 0), ap::vlen(0,n-1));
                xdot(y, xv, n, tx, v, verr);
                rfserrors = rfserrors||ap::fp_greater(fabs(v-bv(i)),4*ap::machineepsilon*ap::maxreal(double(1), fabs(bv(i))));
            }
        }
        
        //
        // Test LS-solver on the same matrix
        //
        rmatrixsolvels(a, n, n, bv, 0.0, info, repls, xv);
        if( info<=0 )
        {
            rfserrors = true;
        }
        else
        {
            for(i = 0; i <= n-1; i++)
            {
                ap::vmove(&y(0), &a(i, 0), ap::vlen(0,n-1));
                xdot(y, xv, n, tx, v, verr);
                rfserrors = rfserrors||ap::fp_greater(fabs(v-bv(i)),4*ap::machineepsilon*ap::maxreal(double(1), fabs(bv(i))));
            }
        }
    }
    
    //
    // end
    //
    waserrors = genrealerrors||lsrealerrors||rfserrors;
    if( !silent )
    {
        printf("TESTING DENSE SOLVER\n");
        printf("* GENERAL REAL:                           ");
        if( genrealerrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("* LEAST SQUARES REAL:                     ");
        if( lsrealerrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("* ITERATIVE IMPROVEMENT:                  ");
        if( rfserrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        if( waserrors )
        {
            printf("TEST FAILED\n");
        }
        else
        {
            printf("TEST PASSED\n");
        }
    }
    result = !waserrors;
    return result;
}


/*************************************************************************
Unsets real matrix
*************************************************************************/
static void unset2d(ap::real_2d_array& x)
{

    x.setlength(1, 1);
    x(0,0) = 2*ap::randomreal()-1;
}


/*************************************************************************
Unsets real vector
*************************************************************************/
static void unset1d(ap::real_1d_array& x)
{

    x.setlength(1);
    x(0) = 2*ap::randomreal()-1;
}


/*************************************************************************
Unsets report
*************************************************************************/
static void unsetrep(densesolverreport& r)
{

    r.r1 = -1;
    r.rinf = -1;
}


/*************************************************************************
Unsets report
*************************************************************************/
static void unsetlsrep(densesolverlsreport& r)
{

    r.r2 = -1;
    r.n = -1;
    r.k = -1;
    unset2d(r.cx);
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool testdensesolverunit_test_silent()
{
    bool result;

    result = testdensesolver(true);
    return result;
}


/*************************************************************************
Unit test
*************************************************************************/
bool testdensesolverunit_test()
{
    bool result;

    result = testdensesolver(false);
    return result;
}




