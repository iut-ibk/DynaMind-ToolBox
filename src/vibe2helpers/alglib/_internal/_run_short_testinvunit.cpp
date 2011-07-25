#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "testinvunit.h"

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
        if(!testinvunit_test_silent())
            throw 0;
    }
    catch(...)
    {
        printf("%-32s FAILED(seed=%ld)\n", "inv", (long)seed);
        return 1;
    }
    printf("%-32s OK\n", "inv");
    return 0;
}

