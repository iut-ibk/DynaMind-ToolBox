
#include <stdafx.h>
#include <stdio.h>
#include "testlm.h"

static bool rkindvsstatecheck(int rkind, const lmstate& state);

bool testminlm(bool silent)
{
    bool result;
    bool waserrors;
    bool referror;
    bool lin1error;
    bool lin2error;
    bool eqerror;
    bool converror;
    bool scerror;
    int rkind;
    int nset;
    int n;
    int m;
    ap::real_1d_array x;
    ap::real_1d_array xe;
    ap::real_1d_array b;
    int i;
    int j;
    double v;
    ap::real_2d_array a;
    lmstate state;
    lmreport rep;

    waserrors = false;
    referror = false;
    lin1error = false;
    lin2error = false;
    eqerror = false;
    converror = false;
    scerror = false;
    
    //
    // Reference problem.
    // RKind is a algorithm selector:
    // * 0 = FJ
    // * 1 = FGJ
    // * 2 = FGH
    //
    x.setbounds(0, 2);
    n = 3;
    m = 3;
    for(rkind = 0; rkind <= 2; rkind++)
    {
        x(0) = 100*ap::randomreal()-50;
        x(1) = 100*ap::randomreal()-50;
        x(2) = 100*ap::randomreal()-50;
        if( rkind==0 )
        {
            minlmfj(n, m, x, 0.0, 0.0, 0, state);
        }
        if( rkind==1 )
        {
            minlmfgj(n, m, x, 0.0, 0.0, 0, state);
        }
        if( rkind==2 )
        {
            minlmfgh(n, x, 0.0, 0.0, 0, state);
        }
        while(minlmiteration(state))
        {
            
            //
            // (x-2)^2 + y^2 + (z-x)^2
            //
            state.f = ap::sqr(state.x(0)-2)+ap::sqr(state.x(1))+ap::sqr(state.x(2)-state.x(0));
            if( state.needfg||state.needfgh )
            {
                state.g(0) = 2*(state.x(0)-2)+2*(state.x(0)-state.x(2));
                state.g(1) = 2*state.x(1);
                state.g(2) = 2*(state.x(2)-state.x(0));
            }
            if( state.needfij )
            {
                state.fi(0) = state.x(0)-2;
                state.fi(1) = state.x(1);
                state.fi(2) = state.x(2)-state.x(0);
                state.j(0,0) = 1;
                state.j(0,1) = 0;
                state.j(0,2) = 0;
                state.j(1,0) = 0;
                state.j(1,1) = 1;
                state.j(1,2) = 0;
                state.j(2,0) = -1;
                state.j(2,1) = 0;
                state.j(2,2) = 1;
            }
            if( state.needfgh )
            {
                state.h(0,0) = 4;
                state.h(0,1) = 0;
                state.h(0,2) = -2;
                state.h(1,0) = 0;
                state.h(1,1) = 2;
                state.h(1,2) = 0;
                state.h(2,0) = -2;
                state.h(2,1) = 0;
                state.h(2,2) = 2;
            }
            scerror = scerror||!rkindvsstatecheck(rkind, state);
        }
        minlmresults(state, x, rep);
        referror = referror||rep.terminationtype<=0||ap::fp_greater(fabs(x(0)-2),0.001)||ap::fp_greater(fabs(x(1)),0.001)||ap::fp_greater(fabs(x(2)-2),0.001);
    }
    
    //
    // end
    //
    waserrors = referror||lin1error||lin2error||eqerror||converror||scerror;
    if( !silent )
    {
        printf("TESTING LEVENBERG-MARQUARDT OPTIMIZATION\n");
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
        printf("STATE FIELDS CONSISTENCY:                 ");
        if( scerror )
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
Asserts that State fields are consistent with RKind.
Returns False otherwise.
*************************************************************************/
static bool rkindvsstatecheck(int rkind, const lmstate& state)
{
    bool result;
    int nset;

    nset = 0;
    if( state.needf )
    {
        nset = nset+1;
    }
    if( state.needfg )
    {
        nset = nset+1;
    }
    if( state.needfij )
    {
        nset = nset+1;
    }
    if( state.needfgh )
    {
        nset = nset+1;
    }
    if( nset!=1 )
    {
        result = false;
        return result;
    }
    if( rkind==0&&(state.needfg||state.needfgh) )
    {
        result = false;
        return result;
    }
    if( rkind==1&&state.needfgh )
    {
        result = false;
        return result;
    }
    if( rkind==2&&state.needfij )
    {
        result = false;
        return result;
    }
    result = true;
    return result;
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool testlm_test_silent()
{
    bool result;

    result = testminlm(true);
    return result;
}


/*************************************************************************
Unit test
*************************************************************************/
bool testlm_test()
{
    bool result;

    result = testminlm(false);
    return result;
}




