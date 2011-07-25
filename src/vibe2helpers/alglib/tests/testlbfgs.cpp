
#include <stdafx.h>
#include <stdio.h>
#include "testlbfgs.h"

bool testminlbfgs(bool silent)
{
    bool result;
    bool waserrors;
    bool referror;
    bool lin1error;
    bool lin2error;
    bool eqerror;
    bool converror;
    int n;
    int m;
    ap::real_1d_array x;
    ap::real_1d_array xe;
    ap::real_1d_array b;
    int i;
    int j;
    double v;
    ap::real_2d_array a;
    lbfgsstate state;
    lbfgsreport rep;

    waserrors = false;
    
    //
    // Reference problem
    //
    x.setbounds(0, 2);
    n = 3;
    m = 2;
    x(0) = 100*ap::randomreal()-50;
    x(1) = 100*ap::randomreal()-50;
    x(2) = 100*ap::randomreal()-50;
    minlbfgs(n, m, x, 0.0, 0.0, 0.0, 0, 0, state);
    while(minlbfgsiteration(state))
    {
        state.f = ap::sqr(state.x(0)-2)+ap::sqr(state.x(1))+ap::sqr(state.x(2)-state.x(0));
        state.g(0) = 2*(state.x(0)-2)+2*(state.x(0)-state.x(2));
        state.g(1) = 2*state.x(1);
        state.g(2) = 2*(state.x(2)-state.x(0));
    }
    minlbfgsresults(state, x, rep);
    referror = rep.terminationtype<=0||ap::fp_greater(fabs(x(0)-2),0.001)||ap::fp_greater(fabs(x(1)),0.001)||ap::fp_greater(fabs(x(2)-2),0.001);
    
    //
    // 1D problem #1
    //
    x.setbounds(0, 0);
    n = 1;
    m = 1;
    x(0) = 100*ap::randomreal()-50;
    minlbfgs(n, m, x, 0.0, 0.0, 0.0, 0, 0, state);
    while(minlbfgsiteration(state))
    {
        state.f = -cos(state.x(0));
        state.g(0) = sin(state.x(0));
    }
    minlbfgsresults(state, x, rep);
    lin1error = rep.terminationtype<=0||ap::fp_greater(fabs(x(0)/ap::pi()-ap::round(x(0)/ap::pi())),0.001);
    
    //
    // 1D problem #2
    //
    x.setbounds(0, 0);
    n = 1;
    m = 1;
    x(0) = 100*ap::randomreal()-50;
    minlbfgs(n, m, x, 0.0, 0.0, 0.0, 0, 0, state);
    while(minlbfgsiteration(state))
    {
        state.f = ap::sqr(state.x(0))/(1+ap::sqr(state.x(0)));
        state.g(0) = (2*state.x(0)*(1+ap::sqr(state.x(0)))-ap::sqr(state.x(0))*2*state.x(0))/ap::sqr(1+ap::sqr(state.x(0)));
    }
    minlbfgsresults(state, x, rep);
    lin2error = rep.terminationtype<=0||ap::fp_greater(fabs(x(0)),0.001);
    
    //
    // Linear equations
    //
    eqerror = false;
    for(n = 1; n <= 10; n++)
    {
        
        //
        // Prepare task
        //
        a.setbounds(0, n-1, 0, n-1);
        x.setbounds(0, n-1);
        xe.setbounds(0, n-1);
        b.setbounds(0, n-1);
        for(i = 0; i <= n-1; i++)
        {
            xe(i) = 2*ap::randomreal()-1;
        }
        for(i = 0; i <= n-1; i++)
        {
            for(j = 0; j <= n-1; j++)
            {
                a(i,j) = 2*ap::randomreal()-1;
            }
            a(i,i) = a(i,i)+3*ap::sign(a(i,i));
        }
        for(i = 0; i <= n-1; i++)
        {
            v = ap::vdotproduct(&a(i, 0), &xe(0), ap::vlen(0,n-1));
            b(i) = v;
        }
        
        //
        // Test different M
        //
        for(m = 1; m <= n; m++)
        {
            
            //
            // Solve task
            //
            for(i = 0; i <= n-1; i++)
            {
                x(i) = 2*ap::randomreal()-1;
            }
            minlbfgs(n, m, x, 0.0, 0.0, 0.0, 0, 0, state);
            while(minlbfgsiteration(state))
            {
                state.f = 0;
                for(i = 0; i <= n-1; i++)
                {
                    state.g(i) = 0;
                }
                for(i = 0; i <= n-1; i++)
                {
                    v = ap::vdotproduct(&a(i, 0), &state.x(0), ap::vlen(0,n-1));
                    state.f = state.f+ap::sqr(v-b(i));
                    for(j = 0; j <= n-1; j++)
                    {
                        state.g(j) = state.g(j)+2*(v-b(i))*a(i,j);
                    }
                }
            }
            minlbfgsresults(state, x, rep);
            eqerror = eqerror||rep.terminationtype<=0;
            for(i = 0; i <= n-1; i++)
            {
                eqerror = eqerror||ap::fp_greater(fabs(x(i)-xe(i)),0.001);
            }
        }
    }
    
    //
    // Testing convergence properties
    //
    converror = false;
    x.setbounds(0, 2);
    n = 3;
    m = 2;
    for(i = 0; i <= 2; i++)
    {
        x(i) = 6*ap::randomreal()-3;
    }
    minlbfgs(n, m, x, 0.0001, 0.0, 0.0, 0, 0, state);
    while(minlbfgsiteration(state))
    {
        state.f = ap::sqr(exp(state.x(0))-2)+ap::sqr(state.x(1))+ap::sqr(state.x(2)-state.x(0));
        state.g(0) = 2*(exp(state.x(0))-2)*exp(state.x(0))+2*(state.x(0)-state.x(2));
        state.g(1) = 2*state.x(1);
        state.g(2) = 2*(state.x(2)-state.x(0));
    }
    minlbfgsresults(state, x, rep);
    converror = converror||ap::fp_greater(fabs(x(0)-log(double(2))),0.05);
    converror = converror||ap::fp_greater(fabs(x(1)),0.05);
    converror = converror||ap::fp_greater(fabs(x(2)-log(double(2))),0.05);
    converror = converror||rep.terminationtype!=4;
    for(i = 0; i <= 2; i++)
    {
        x(i) = 6*ap::randomreal()-3;
    }
    minlbfgs(n, m, x, 0.0, 0.0001, 0.0, 0, 0, state);
    while(minlbfgsiteration(state))
    {
        state.f = ap::sqr(exp(state.x(0))-2)+ap::sqr(state.x(1))+ap::sqr(state.x(2)-state.x(0));
        state.g(0) = 2*(exp(state.x(0))-2)*exp(state.x(0))+2*(state.x(0)-state.x(2));
        state.g(1) = 2*state.x(1);
        state.g(2) = 2*(state.x(2)-state.x(0));
    }
    minlbfgsresults(state, x, rep);
    converror = converror||ap::fp_greater(fabs(x(0)-log(double(2))),0.05);
    converror = converror||ap::fp_greater(fabs(x(1)),0.05);
    converror = converror||ap::fp_greater(fabs(x(2)-log(double(2))),0.05);
    converror = converror||rep.terminationtype!=1;
    for(i = 0; i <= 2; i++)
    {
        x(i) = 6*ap::randomreal()-3;
    }
    minlbfgs(n, m, x, 0.0, 0.0, 0.0001, 0, 0, state);
    while(minlbfgsiteration(state))
    {
        state.f = ap::sqr(exp(state.x(0))-2)+ap::sqr(state.x(1))+ap::sqr(state.x(2)-state.x(0));
        state.g(0) = 2*(exp(state.x(0))-2)*exp(state.x(0))+2*(state.x(0)-state.x(2));
        state.g(1) = 2*state.x(1);
        state.g(2) = 2*(state.x(2)-state.x(0));
    }
    minlbfgsresults(state, x, rep);
    converror = converror||ap::fp_greater(fabs(x(0)-log(double(2))),0.05);
    converror = converror||ap::fp_greater(fabs(x(1)),0.05);
    converror = converror||ap::fp_greater(fabs(x(2)-log(double(2))),0.05);
    converror = converror||rep.terminationtype!=2;
    for(i = 0; i <= 2; i++)
    {
        x(i) = 2*ap::randomreal()-1;
    }
    minlbfgs(n, m, x, 0.0, 0.0, 0.0, 10, 0, state);
    while(minlbfgsiteration(state))
    {
        state.f = ap::sqr(exp(state.x(0))-2)+ap::sqr(state.x(1))+ap::sqr(state.x(2)-state.x(0));
        state.g(0) = 2*(exp(state.x(0))-2)*exp(state.x(0))+2*(state.x(0)-state.x(2));
        state.g(1) = 2*state.x(1);
        state.g(2) = 2*(state.x(2)-state.x(0));
    }
    minlbfgsresults(state, x, rep);
    converror = converror||rep.terminationtype!=5||rep.iterationscount!=10;
    
    //
    // end
    //
    waserrors = referror||lin1error||lin2error||eqerror||converror;
    if( !silent )
    {
        printf("TESTING L-BFGS OPTIMIZATION\n");
        printf("REFERENCE PROBLEM:                        ");
        if( referror )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("1-D PROBLEM #1:                           ");
        if( lin1error )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("1-D PROBLEM #2:                           ");
        if( lin2error )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("LINEAR EQUATIONS:                         ");
        if( eqerror )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("CONVERGENCE PROPERTIES:                   ");
        if( converror )
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
        printf("\n\n");
    }
    result = !waserrors;
    return result;
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool testlbfgs_test_silent()
{
    bool result;

    result = testminlbfgs(true);
    return result;
}


/*************************************************************************
Unit test
*************************************************************************/
bool testlbfgs_test()
{
    bool result;

    result = testminlbfgs(false);
    return result;
}




