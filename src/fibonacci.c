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

