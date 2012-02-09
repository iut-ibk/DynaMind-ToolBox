#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "lbfgs.h"

int main(int argc, char **argv)
{
    int n;
    int m;
    lbfgsstate state;
    lbfgsreport rep;
    ap::real_1d_array s;
    double x;
    double y;

    
    //
    // Function minimized:
    //     F = exp(x-1) + exp(1-x) + (y-x)^2
    // N = 2 - task dimension
    // M = 1 - build tank-1 model
    //
    n = 2;
    m = 1;
    s.setlength(2);
    s(0) = ap::randomreal()-0.5;
    s(1) = ap::randomreal()-0.5;
    minlbfgs(n, m, s, 0.0, 0.0, 0.0001, 0, 0, state);
    while(minlbfgsiteration(state))
    {
        x = state.x(0);
        y = state.x(1);
        state.f = exp(x-1)+exp(1-x)+ap::sqr(y-x);
        state.g(0) = exp(x-1)-exp(1-x)+2*(x-y);
        state.g(1) = 2*(y-x);
    }
    minlbfgsresults(state, s, rep);
    
    //
    // output results
    //
    printf("F = exp(x-1) + exp(1-x) + (y-x)^2\n");
    printf("X = %4.2lf (should be 1.00)\n",
        double(s(0)));
    printf("Y = %4.2lf (should be 1.00)\n",
        double(s(1)));
    return 0;
}

