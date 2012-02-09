/*************************************************************************
Copyright (c) 2009, Sergey Bochkanov (ALGLIB project).

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
#include "minlm.h"

static const int lmmodefj = 0;
static const int lmmodefgj = 1;
static const int lmmodefgh = 2;
static const int lmflagnoprelbfgs = 1;
static const int lmflagnointlbfgs = 2;
static const int lmprelbfgsm = 5;
static const int lmintlbfgsits = 5;
static const int lbfgsnorealloc = 1;

static void lmprepare(int n, int m, bool havegrad, lmstate& state);
static void lmclearrequestfields(lmstate& state);

/*************************************************************************
    LEVENBERG-MARQUARDT-LIKE METHOD FOR NON-LINEAR OPTIMIZATION

Optimization using function gradient and Hessian.  Algorithm -  Levenberg-
Marquardt   modification   with   L-BFGS   pre-optimization  and  internal
pre-conditioned L-BFGS optimization after each Levenberg-Marquardt step.

Function F has general form (not "sum-of-squares"):

    F = F(x[0], ..., x[n-1])

EXAMPLE

See HTML-documentation.

INPUT PARAMETERS:
    N       -   dimension, N>1
    X       -   initial solution, array[0..N-1]
    EpsF    -   stopping criterion. Algorithm stops if
                |F(k+1)-F(k)| <= EpsF*max{|F(k)|, |F(k+1)|, 1}
    EpsX    -   stopping criterion. Algorithm stops if
                |X(k+1)-X(k)| <= EpsX*(1+|X(k)|)
    MaxIts  -   stopping criterion. Algorithm stops after MaxIts iterations.
                MaxIts=0 means no stopping criterion.

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state between subsequent
                calls of MinLMIteration. Used for reverse communication.
                This structure should be passed to MinLMIteration subroutine.

See also MinLMIteration, MinLMResults.

NOTE

Passing EpsF=0, EpsX=0 and MaxIts=0 (simultaneously) will lead to automatic
stopping criterion selection (small EpsX).

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmfgh(const int& n,
     const ap::real_1d_array& x,
     const double& epsf,
     const double& epsx,
     const int& maxits,
     lmstate& state)
{

    
    //
    // Prepare RComm
    //
    state.rstate.ia.setbounds(0, 3);
    state.rstate.ba.setbounds(0, 0);
    state.rstate.ra.setbounds(0, 8);
    state.rstate.stage = -1;
    
    //
    // prepare internal structures
    //
    lmprepare(n, 0, true, state);
    
    //
    // initialize, check parameters
    //
    state.xupdated = false;
    state.n = n;
    state.m = 0;
    state.epsf = epsf;
    state.epsx = epsx;
    state.maxits = maxits;
    state.flags = 0;
    if( ap::fp_eq(state.epsf,0)&&ap::fp_eq(state.epsx,0)&&state.maxits==0 )
    {
        state.epsx = 1.0E-6;
    }
    state.usermode = lmmodefgh;
    state.wrongparams = false;
    if( n<1||ap::fp_less(epsf,0)||ap::fp_less(epsx,0)||maxits<0 )
    {
        state.wrongparams = true;
        return;
    }
    ap::vmove(&state.x(0), &x(0), ap::vlen(0,n-1));
}


/*************************************************************************
    LEVENBERG-MARQUARDT-LIKE METHOD FOR NON-LINEAR OPTIMIZATION

Optimization using function gradient and Jacobian.  Algorithm -  Levenberg-
Marquardt   modification   with   L-BFGS   pre-optimization  and  internal
pre-conditioned L-BFGS optimization after each Levenberg-Marquardt step.

Function F is represented as sum of squares:

    F = f[0]^2(x[0],...,x[n-1]) + ... + f[m-1]^2(x[0],...,x[n-1])

EXAMPLE

See HTML-documentation.

INPUT PARAMETERS:
    N       -   dimension, N>1
    M       -   number of functions f[i]
    X       -   initial solution, array[0..N-1]
    EpsF    -   stopping criterion. Algorithm stops if
                |F(k+1)-F(k)| <= EpsF*max{|F(k)|, |F(k+1)|, 1}
    EpsX    -   stopping criterion. Algorithm stops if
                |X(k+1)-X(k)| <= EpsX*(1+|X(k)|)
    MaxIts  -   stopping criterion. Algorithm stops after MaxIts iterations.
                MaxIts=0 means no stopping criterion.

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state between subsequent
                calls of MinLMIteration. Used for reverse communication.
                This structure should be passed to MinLMIteration subroutine.

See also MinLMIteration, MinLMResults.

NOTE

Passing EpsF=0, EpsX=0 and MaxIts=0 (simultaneously) will lead to automatic
stopping criterion selection (small EpsX).

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmfgj(const int& n,
     const int& m,
     const ap::real_1d_array& x,
     const double& epsf,
     const double& epsx,
     const int& maxits,
     lmstate& state)
{

    
    //
    // Prepare RComm
    //
    state.rstate.ia.setbounds(0, 3);
    state.rstate.ba.setbounds(0, 0);
    state.rstate.ra.setbounds(0, 8);
    state.rstate.stage = -1;
    
    //
    // prepare internal structures
    //
    lmprepare(n, m, true, state);
    
    //
    // initialize, check parameters
    //
    state.xupdated = false;
    state.n = n;
    state.m = m;
    state.epsf = epsf;
    state.epsx = epsx;
    state.maxits = maxits;
    state.flags = 0;
    if( ap::fp_eq(state.epsf,0)&&ap::fp_eq(state.epsx,0)&&state.maxits==0 )
    {
        state.epsx = 1.0E-6;
    }
    state.usermode = lmmodefgj;
    state.wrongparams = false;
    if( n<1||m<1||ap::fp_less(epsf,0)||ap::fp_less(epsx,0)||maxits<0 )
    {
        state.wrongparams = true;
        return;
    }
    ap::vmove(&state.x(0), &x(0), ap::vlen(0,n-1));
}


/*************************************************************************
    CLASSIC LEVENBERG-MARQUARDT METHOD FOR NON-LINEAR OPTIMIZATION

Optimization using Jacobi matrix. Algorithm  -  classic Levenberg-Marquardt
method.

Function F is represented as sum of squares:

    F = f[0]^2(x[0],...,x[n-1]) + ... + f[m-1]^2(x[0],...,x[n-1])

EXAMPLE

See HTML-documentation.

INPUT PARAMETERS:
    N       -   dimension, N>1
    M       -   number of functions f[i]
    X       -   initial solution, array[0..N-1]
    EpsF    -   stopping criterion. Algorithm stops if
                |F(k+1)-F(k)| <= EpsF*max{|F(k)|, |F(k+1)|, 1}
    EpsX    -   stopping criterion. Algorithm stops if
                |X(k+1)-X(k)| <= EpsX*(1+|X(k)|)
    MaxIts  -   stopping criterion. Algorithm stops after MaxIts iterations.
                MaxIts=0 means no stopping criterion.

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state between subsequent
                calls of MinLMIteration. Used for reverse communication.
                This structure should be passed to MinLMIteration subroutine.

See also MinLMIteration, MinLMResults.

NOTE

Passing EpsF=0, EpsX=0 and MaxIts=0 (simultaneously) will lead to automatic
stopping criterion selection (small EpsX).

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmfj(const int& n,
     const int& m,
     const ap::real_1d_array& x,
     const double& epsf,
     const double& epsx,
     const int& maxits,
     lmstate& state)
{

    
    //
    // Prepare RComm
    //
    state.rstate.ia.setbounds(0, 3);
    state.rstate.ba.setbounds(0, 0);
    state.rstate.ra.setbounds(0, 8);
    state.rstate.stage = -1;
    
    //
    // prepare internal structures
    //
    lmprepare(n, m, true, state);
    
    //
    // initialize, check parameters
    //
    state.xupdated = false;
    state.n = n;
    state.m = m;
    state.epsf = epsf;
    state.epsx = epsx;
    state.maxits = maxits;
    state.flags = 0;
    if( ap::fp_eq(state.epsf,0)&&ap::fp_eq(state.epsx,0)&&state.maxits==0 )
    {
        state.epsx = 1.0E-6;
    }
    state.usermode = lmmodefj;
    state.wrongparams = false;
    if( n<1||m<1||ap::fp_less(epsf,0)||ap::fp_less(epsx,0)||maxits<0 )
    {
        state.wrongparams = true;
        return;
    }
    ap::vmove(&state.x(0), &x(0), ap::vlen(0,n-1));
}


/*************************************************************************
One Levenberg-Marquardt iteration.

Called after inialization of State structure with MinLMXXX subroutine.
See HTML docs for examples.

Input parameters:
    State   -   structure which stores algorithm state between subsequent
                calls and which is used for reverse communication. Must be
                initialized with MinLMXXX call first.

If subroutine returned False, iterative algorithm has converged.

If subroutine returned True, then:
* if State.NeedF=True,      -   function value F at State.X[0..N-1]
                                is required
* if State.NeedFG=True      -   function value F and gradient G
                                are required
* if State.NeedFiJ=True     -   function vector f[i] and Jacobi matrix J
                                are required
* if State.NeedFGH=True     -   function value F, gradient G and Hesian H
                                are required

One and only one of this fields can be set at time.

Results are stored:
* function value            -   in LMState.F
* gradient                  -   in LMState.G[0..N-1]
* Jacobi matrix             -   in LMState.J[0..M-1,0..N-1]
* Hessian                   -   in LMState.H[0..N-1,0..N-1]

  -- ALGLIB --
     Copyright 10.03.2009 by Bochkanov Sergey
*************************************************************************/
bool minlmiteration(lmstate& state)
{
    bool result;
    int n;
    int m;
    int i;
    double xnorm;
    double stepnorm;
    bool spd;
    double fbase;
    double fnew;
    double lambda;
    double nu;
    double lambdaup;
    double lambdadown;
    int lbfgsflags;
    double v;

    
    //
    // Reverse communication preparations
    // I know it looks ugly, but it works the same way
    // anywhere from C++ to Python.
    //
    // This code initializes locals by:
    // * random values determined during code
    //   generation - on first subroutine call
    // * values from previous call - on subsequent calls
    //
    if( state.rstate.stage>=0 )
    {
        n = state.rstate.ia(0);
        m = state.rstate.ia(1);
        i = state.rstate.ia(2);
        lbfgsflags = state.rstate.ia(3);
        spd = state.rstate.ba(0);
        xnorm = state.rstate.ra(0);
        stepnorm = state.rstate.ra(1);
        fbase = state.rstate.ra(2);
        fnew = state.rstate.ra(3);
        lambda = state.rstate.ra(4);
        nu = state.rstate.ra(5);
        lambdaup = state.rstate.ra(6);
        lambdadown = state.rstate.ra(7);
        v = state.rstate.ra(8);
    }
    else
    {
        n = -983;
        m = -989;
        i = -834;
        lbfgsflags = 900;
        spd = true;
        xnorm = 364;
        stepnorm = 214;
        fbase = -338;
        fnew = -686;
        lambda = 912;
        nu = 585;
        lambdaup = 497;
        lambdadown = -271;
        v = -581;
    }
    if( state.rstate.stage==0 )
    {
        goto lbl_0;
    }
    if( state.rstate.stage==1 )
    {
        goto lbl_1;
    }
    if( state.rstate.stage==2 )
    {
        goto lbl_2;
    }
    if( state.rstate.stage==3 )
    {
        goto lbl_3;
    }
    if( state.rstate.stage==4 )
    {
        goto lbl_4;
    }
    if( state.rstate.stage==5 )
    {
        goto lbl_5;
    }
    if( state.rstate.stage==6 )
    {
        goto lbl_6;
    }
    
    //
    // Routine body
    //
    ap::ap_error::make_assertion(state.usermode==lmmodefj||state.usermode==lmmodefgj||state.usermode==lmmodefgh, "LM: internal error");
    if( state.wrongparams )
    {
        state.repterminationtype = -1;
        result = false;
        return result;
    }
    
    //
    // prepare params
    //
    n = state.n;
    m = state.m;
    lambdaup = 10;
    lambdadown = 0.3;
    nu = 2;
    lbfgsflags = 0;
    
    //
    // if we have F and G
    //
    if( !((state.usermode==lmmodefgj||state.usermode==lmmodefgh)&&state.flags/lmflagnoprelbfgs%2==0) )
    {
        goto lbl_7;
    }
    
    //
    // First stage of the hybrid algorithm: LBFGS
    //
    minlbfgs(n, ap::minint(n, lmprelbfgsm), state.x, 0.0, 0.0, 0.0, ap::maxint(25, n), 0, state.internalstate);
lbl_9:
    if( !minlbfgsiteration(state.internalstate) )
    {
        goto lbl_10;
    }
    
    //
    // RComm
    //
    ap::vmove(&state.x(0), &state.internalstate.x(0), ap::vlen(0,n-1));
    lmclearrequestfields(state);
    state.needfg = true;
    state.rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    state.repnfunc = state.repnfunc+1;
    state.repngrad = state.repngrad+1;
    
    //
    // Call LBFGS
    //
    state.internalstate.f = state.f;
    ap::vmove(&state.internalstate.g(0), &state.g(0), ap::vlen(0,n-1));
    goto lbl_9;
lbl_10:
    minlbfgsresults(state.internalstate, state.x, state.internalrep);
lbl_7:
    
    //
    // Second stage of the hybrid algorithm: LM
    // Initialize quadratic model.
    //
    if( state.usermode!=lmmodefgh )
    {
        goto lbl_11;
    }
    
    //
    // RComm
    //
    lmclearrequestfields(state);
    state.needfgh = true;
    state.rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state.repnfunc = state.repnfunc+1;
    state.repngrad = state.repngrad+1;
    state.repnhess = state.repnhess+1;
    
    //
    // generate raw quadratic model
    //
    for(i = 0; i <= n-1; i++)
    {
        ap::vmove(&state.rawmodel(i, 0), &state.h(i, 0), ap::vlen(0,n-1));
    }
    ap::vmove(&state.gbase(0), &state.g(0), ap::vlen(0,n-1));
    fbase = state.f;
lbl_11:
    if( !(state.usermode==lmmodefgj||state.usermode==lmmodefj) )
    {
        goto lbl_13;
    }
    
    //
    // RComm
    //
    lmclearrequestfields(state);
    state.needfij = true;
    state.rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    state.repnfunc = state.repnfunc+1;
    state.repnjac = state.repnjac+1;
    
    //
    // generate raw quadratic model
    //
    matrixmatrixmultiply(state.j, 0, m-1, 0, n-1, true, state.j, 0, m-1, 0, n-1, false, 1.0, state.rawmodel, 0, n-1, 0, n-1, 0.0, state.work);
    matrixvectormultiply(state.j, 0, m-1, 0, n-1, true, state.fi, 0, m-1, 1.0, state.gbase, 0, n-1, 0.0);
    fbase = ap::vdotproduct(&state.fi(0), &state.fi(0), ap::vlen(0,m-1));
lbl_13:
    lambda = 0.001;
lbl_15:
    if( false )
    {
        goto lbl_16;
    }
    
    //
    // 1. Model = RawModel+lambda*I
    // 2. Try to solve (RawModel+Lambda*I)*dx = -g.
    //    Increase lambda if left part is not positive definite.
    //
    for(i = 0; i <= n-1; i++)
    {
        ap::vmove(&state.model(i, 0), &state.rawmodel(i, 0), ap::vlen(0,n-1));
        state.model(i,i) = state.model(i,i)+lambda;
    }
    spd = spdmatrixcholesky(state.model, n, true);
    state.repncholesky = state.repncholesky+1;
    if( !spd )
    {
        lambda = lambda*lambdaup*nu;
        nu = nu*2;
        goto lbl_15;
    }
    if( !spdmatrixcholeskysolve(state.model, state.gbase, n, true, state.xdir) )
    {
        lambda = lambda*lambdaup*nu;
        nu = nu*2;
        goto lbl_15;
    }
    ap::vmul(&state.xdir(0), ap::vlen(0,n-1), -1);
    
    //
    // Candidate lambda found.
    // 1. Save old w in WBase
    // 1. Test some stopping criterions
    // 2. If error(w+wdir)>error(w), increase lambda
    //
    ap::vmove(&state.xbase(0), &state.x(0), ap::vlen(0,n-1));
    ap::vadd(&state.x(0), &state.xdir(0), ap::vlen(0,n-1));
    xnorm = ap::vdotproduct(&state.xbase(0), &state.xbase(0), ap::vlen(0,n-1));
    stepnorm = ap::vdotproduct(&state.xdir(0), &state.xdir(0), ap::vlen(0,n-1));
    xnorm = sqrt(xnorm);
    stepnorm = sqrt(stepnorm);
    if( ap::fp_less_eq(stepnorm,state.epsx*(1+xnorm)) )
    {
        
        //
        // step size if small enough
        //
        state.repterminationtype = 2;
        goto lbl_16;
    }
    lmclearrequestfields(state);
    state.needf = true;
    state.rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    state.repnfunc = state.repnfunc+1;
    fnew = state.f;
    if( ap::fp_less_eq(fabs(fnew-fbase),state.epsf*ap::maxreal(double(1), ap::maxreal(fabs(fbase), fabs(fnew)))) )
    {
        
        //
        // function change is small enough
        //
        state.repterminationtype = 1;
        goto lbl_16;
    }
    if( ap::fp_greater(fnew,fbase) )
    {
        
        //
        // restore state and continue out search for lambda
        //
        ap::vmove(&state.x(0), &state.xbase(0), ap::vlen(0,n-1));
        lambda = lambda*lambdaup*nu;
        nu = nu*2;
        goto lbl_15;
    }
    if( !((state.usermode==lmmodefgj||state.usermode==lmmodefgh)&&state.flags/lmflagnointlbfgs%2==0) )
    {
        goto lbl_17;
    }
    ap::ap_error::make_assertion(state.usermode==lmmodefgh, "");
    
    //
    // Optimize using inv(cholesky(H)) as preconditioner
    //
    if( !rmatrixtrinverse(state.model, n, true, false) )
    {
        goto lbl_19;
    }
    
    //
    // if matrix can be inverted use it.
    // just silently move to next iteration otherwise.
    // (will be very, very rare, mostly for specially
    // designed near-degenerate tasks)
    //
    ap::vmove(&state.xbase(0), &state.x(0), ap::vlen(0,n-1));
    for(i = 0; i <= n-1; i++)
    {
        state.xprec(i) = 0;
    }
    minlbfgs(n, ap::minint(n, lmintlbfgsits), state.xprec, 0.0, 0.0, 0.0, lmintlbfgsits, lbfgsflags, state.internalstate);
lbl_21:
    if( !minlbfgsiteration(state.internalstate) )
    {
        goto lbl_22;
    }
    
    //
    // convert XPrec to unpreconditioned form, then call RComm.
    //
    for(i = 0; i <= n-1; i++)
    {
        v = ap::vdotproduct(&state.internalstate.x(i), &state.model(i, i), ap::vlen(i,n-1));
        state.x(i) = state.xbase(i)+v;
    }
    lmclearrequestfields(state);
    state.needfg = true;
    state.rstate.stage = 4;
    goto lbl_rcomm;
lbl_4:
    state.repnfunc = state.repnfunc+1;
    state.repngrad = state.repngrad+1;
    
    //
    // 1. pass State.F to State.InternalState.F
    // 2. convert gradient back to preconditioned form
    //
    state.internalstate.f = state.f;
    for(i = 0; i <= n-1; i++)
    {
        state.internalstate.g(i) = 0;
    }
    for(i = 0; i <= n-1; i++)
    {
        v = state.g(i);
        ap::vadd(&state.internalstate.g(i), &state.model(i, i), ap::vlen(i,n-1), v);
    }
    
    //
    // next iteration
    //
    goto lbl_21;
lbl_22:
    
    //
    // change LBFGS flags to NoRealloc.
    // L-BFGS subroutine will use memory allocated from previous run.
    // it is possible since all subsequent calls will be with same N/M.
    //
    lbfgsflags = lbfgsnorealloc;
    
    //
    // back to unpreconditioned X
    //
    minlbfgsresults(state.internalstate, state.xprec, state.internalrep);
    for(i = 0; i <= n-1; i++)
    {
        v = ap::vdotproduct(&state.xprec(i), &state.model(i, i), ap::vlen(i,n-1));
        state.x(i) = state.xbase(i)+v;
    }
lbl_19:
lbl_17:
    
    //
    // Accept new position.
    // Calculate Hessian
    //
    if( state.usermode!=lmmodefgh )
    {
        goto lbl_23;
    }
    
    //
    // RComm
    //
    lmclearrequestfields(state);
    state.needfgh = true;
    state.rstate.stage = 5;
    goto lbl_rcomm;
lbl_5:
    state.repnfunc = state.repnfunc+1;
    state.repngrad = state.repngrad+1;
    state.repnhess = state.repnhess+1;
    
    //
    // Update raw quadratic model
    //
    for(i = 0; i <= n-1; i++)
    {
        ap::vmove(&state.rawmodel(i, 0), &state.h(i, 0), ap::vlen(0,n-1));
    }
    ap::vmove(&state.gbase(0), &state.g(0), ap::vlen(0,n-1));
    fbase = state.f;
lbl_23:
    if( !(state.usermode==lmmodefgj||state.usermode==lmmodefj) )
    {
        goto lbl_25;
    }
    
    //
    // RComm
    //
    lmclearrequestfields(state);
    state.needfij = true;
    state.rstate.stage = 6;
    goto lbl_rcomm;
lbl_6:
    state.repnfunc = state.repnfunc+1;
    state.repnjac = state.repnjac+1;
    
    //
    // generate raw quadratic model
    //
    matrixmatrixmultiply(state.j, 0, m-1, 0, n-1, true, state.j, 0, m-1, 0, n-1, false, 1.0, state.rawmodel, 0, n-1, 0, n-1, 0.0, state.work);
    matrixvectormultiply(state.j, 0, m-1, 0, n-1, true, state.fi, 0, m-1, 1.0, state.gbase, 0, n-1, 0.0);
    fbase = ap::vdotproduct(&state.fi(0), &state.fi(0), ap::vlen(0,m-1));
lbl_25:
    state.repiterationscount = state.repiterationscount+1;
    if( state.repiterationscount>=state.maxits&&state.maxits>0 )
    {
        state.repterminationtype = 5;
        goto lbl_16;
    }
    
    //
    // Update lambda
    //
    lambda = lambda*lambdadown;
    nu = 2;
    goto lbl_15;
lbl_16:
    result = false;
    return result;
    
    //
    // Saving state
    //
lbl_rcomm:
    result = true;
    state.rstate.ia(0) = n;
    state.rstate.ia(1) = m;
    state.rstate.ia(2) = i;
    state.rstate.ia(3) = lbfgsflags;
    state.rstate.ba(0) = spd;
    state.rstate.ra(0) = xnorm;
    state.rstate.ra(1) = stepnorm;
    state.rstate.ra(2) = fbase;
    state.rstate.ra(3) = fnew;
    state.rstate.ra(4) = lambda;
    state.rstate.ra(5) = nu;
    state.rstate.ra(6) = lambdaup;
    state.rstate.ra(7) = lambdadown;
    state.rstate.ra(8) = v;
    return result;
}


/*************************************************************************
Levenberg-Marquardt algorithm results

Called after MinLMIteration returned False.

Input parameters:
    State   -   algorithm state (used by MinLMIteration).

Output parameters:
    X       -   array[0..N-1], solution
    Rep     -   optimization report:
                * Rep.TerminationType completetion code:
                    * -1    incorrect parameters were specified
                    *  1    relative function improvement is no more than
                            EpsF.
                    *  2    relative step is no more than EpsX.
                    *  4    gradient norm is no more than EpsG
                    *  5    MaxIts steps was taken
                * Rep.IterationsCount contains iterations count
                * Rep.NFunc     - number of function calculations
                * Rep.NJac      - number of Jacobi matrix calculations
                * Rep.NGrad     - number of gradient calculations
                * Rep.NHess     - number of Hessian calculations
                * Rep.NCholesky - number of Cholesky decomposition calculations

  -- ALGLIB --
     Copyright 10.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmresults(const lmstate& state, ap::real_1d_array& x, lmreport& rep)
{

    x.setbounds(0, state.n-1);
    ap::vmove(&x(0), &state.x(0), ap::vlen(0,state.n-1));
    rep.iterationscount = state.repiterationscount;
    rep.terminationtype = state.repterminationtype;
    rep.nfunc = state.repnfunc;
    rep.njac = state.repnjac;
    rep.ngrad = state.repngrad;
    rep.nhess = state.repnhess;
    rep.ncholesky = state.repncholesky;
}


/*************************************************************************
Prepare internal structures (except for RComm).

Note: M must be zero for FGH mode, non-zero for FJ/FGJ mode.
*************************************************************************/
static void lmprepare(int n, int m, bool havegrad, lmstate& state)
{

    state.repiterationscount = 0;
    state.repterminationtype = 0;
    state.repnfunc = 0;
    state.repnjac = 0;
    state.repngrad = 0;
    state.repnhess = 0;
    state.repncholesky = 0;
    if( n<0||m<0 )
    {
        return;
    }
    if( havegrad )
    {
        state.g.setbounds(0, n-1);
    }
    if( m!=0 )
    {
        state.j.setbounds(0, m-1, 0, n-1);
        state.fi.setbounds(0, m-1);
        state.h.setbounds(0, 0, 0, 0);
    }
    else
    {
        state.j.setbounds(0, 0, 0, 0);
        state.fi.setbounds(0, 0);
        state.h.setbounds(0, n-1, 0, n-1);
    }
    state.x.setbounds(0, n-1);
    state.rawmodel.setbounds(0, n-1, 0, n-1);
    state.model.setbounds(0, n-1, 0, n-1);
    state.xbase.setbounds(0, n-1);
    state.xprec.setbounds(0, n-1);
    state.gbase.setbounds(0, n-1);
    state.xdir.setbounds(0, n-1);
    state.work.setbounds(1, ap::maxint(n, m));
}


/*************************************************************************
Clears request fileds (to be sure that we don't forgot to clear something)
*************************************************************************/
static void lmclearrequestfields(lmstate& state)
{

    state.needf = false;
    state.needfg = false;
    state.needfgh = false;
    state.needfij = false;
}




