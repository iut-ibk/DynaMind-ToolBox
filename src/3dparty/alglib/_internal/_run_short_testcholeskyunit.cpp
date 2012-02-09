#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "testcholeskyunit.h"

int main(int argc, char **argv)
{
    unsigned seed;
    if( argc==2 )
        seed = (unsigned)atoi(argv[1]);
    else
    {
        time_t t;
        seed = (unsigned)time(&t);
    }
    srand(seed);
    try
    {
        if(!testcholeskyunit_test_silent())
            throw 0;
    }
    catch(...)
    {
        printf("%-32s FAILED(seed=%ld)\n", "cholesky", (long)seed);
        return 1;
    }
    printf("%-32s OK\n", "cholesky");
    return 0;
}

