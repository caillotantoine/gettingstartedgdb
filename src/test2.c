#include <stdio.h>
#include <stdlib.h>

#include "fibonacci.h"

int main(int argc, char **argv)
{
        int i = 0;
        int fibRes =  0;
        for(int i = 0; i<=9; i++)
        {
                fibRes = fibonacci(i);
                printf("Fibonacci %d %d\n", i, fibRes);
        }
        return 0;
}

