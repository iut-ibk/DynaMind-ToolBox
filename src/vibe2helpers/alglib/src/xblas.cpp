
#include <stdafx.h>
#include "xblas.h"

static double xfastpow(double r, int n);
static double xfrac(double r);

/*************************************************************************
More precise dot-product. Absolute error of  subroutine  result  is  about
1 ulp of max(MX,V), where:
    MX = max( |a[i]*b[i]| )
    V  = |(a,b)|
    
INPUT PARAMETERS
    A       -   array[0..N-1], vector 1
    B       -   array[0..N-1], vector 2
    N       -   vectors length, N<2^29.
    Temp    -   array[0..N-1], pre-allocated temporary storage
    
OUTPUT PARAMETERS
    R       -   (A,B)
    RErr    -   estimate of error. This estimate accounts for both  errors
                during  calculation  of  (A,B)  and  errors  introduced by
                rounding of A/B to fit in double (about 1 ulp).

  -- ALGLIB --
     Copyright 24.08.2009 by Bochkanov Sergey
*************************************************************************/
void xdot(const ap::real_1d_array& a,
     const ap::real_1d_array& b,
     int n,
     ap::real_1d_array& temp,
     double& r,
     double& rerr)
{
    int i;
    int k;
    int ks;
    double mx;
    double v;
    double v1;
    double v2;
    double s;
    double ln2;
    double chunk;
    double invchunk;
    bool allzeros;

    
    //
    // special cases:
    // * N=0
    // * N is too large to use integer arithmetics
    //
    if( n==0 )
    {
        r = 0;
        rerr = 0;
        return;
    }
    ap::ap_error::make_assertion(n<536870912, "XDot: N is too large!");
    
    //
    // Prepare
    //
    ln2 = log(double(2));
    
    //
    // calculate pairwise products vector TEMP
    // (relative precision of TEMP - almost full)
    // find infinity-norm of products vector
    //
    mx = 0;
    for(i = 0; i <= n-1; i++)
    {
        v = a(i)*b(i);
        temp(i) = v;
        mx = ap::maxreal(mx, fabs(v));
    }
    if( ap::fp_eq(mx,0) )
    {
        r = 0;
        rerr = 0;
        return;
    }
    rerr = mx*ap::machineepsilon;
    
    //
    // 1. find S such that 0.5<=S*MX<1
    // 2. multiply TEMP by S, so task is normalized in some sense
    // 3. S:=1/S so we can obtain original vector multiplying by S
    //
    k = ap::round(log(mx)/ln2);
    s = xfastpow(double(2), -k);
    while(ap::fp_greater_eq(s*mx,1))
    {
        s = 0.5*s;
    }
    while(ap::fp_less(s*mx,0.5))
    {
        s = 2*s;
    }
    ap::vmul(&temp(0), ap::vlen(0,n-1), s);
    s = 1/s;
    
    //
    // find Chunk=2^M such that N*Chunk<2^29
    //
    // we have chosen upper limit (2^29) with enough space left
    // to tolerate possible problems with rounding and N's close
    // to the limit, so we don't want to be very strict here.
    //
    k = ap::trunc(log(double(536870912)/double(n))/ln2);
    chunk = xfastpow(double(2), k);
    if( ap::fp_less(chunk,2) )
    {
        chunk = 2;
    }
    invchunk = 1/chunk;
    
    //
    // calculate result
    //
    r = 0;
    ap::vmul(&temp(0), ap::vlen(0,n-1), chunk);
    while(true)
    {
        s = s*invchunk;
        allzeros = true;
        ks = 0;
        for(i = 0; i <= n-1; i++)
        {
            v = temp(i);
            k = ap::trunc(v);
            if( ap::fp_neq(v,k) )
            {
                allzeros = false;
            }
            temp(i) = chunk*(v-k);
            ks = ks+k;
        }
        r = r+s*ks;
        v = fabs(r);
        if( allzeros||ap::fp_eq(s*n+mx,mx) )
        {
            break;
        }
    }
    
    //
    // correct error
    //
    rerr = ap::maxreal(rerr, fabs(r)*ap::machineepsilon);
}


static double xfastpow(double r, int n)
{
    double result;

    if( n>0 )
    {
        if( n%2==0 )
        {
            result = ap::sqr(xfastpow(r, n/2));
        }
        else
        {
            result = r*xfastpow(r, n-1);
        }
        return result;
    }
    if( n==0 )
    {
        result = 1;
    }
    if( n<0 )
    {
        result = xfastpow(1/r, -n);
    }
    return result;
}


static double xfrac(double r)
{
    double result;
    int i;

    if( ap::fp_eq(r,0) )
    {
        result = 0;
        return result;
    }
    if( ap::fp_less(r,0) )
    {
        result = -1;
        r = -r;
    }
    else
    {
        result = 1;
    }
    result = result*(r-ap::ifloor(r));
    return result;
}




