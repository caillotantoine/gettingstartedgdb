#include <stdio.h>
#include <stdlib.h>

int fibonacci(int N)
{
	int i = 0;
	int prev = 0;
	int curr = 1;
	int buffer = 0;
	for(i=0; i<N; i++)
	{
		buffer = prev + curr;
		prev = curr;
		curr = buffer;
	}
	return prev;
}

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
