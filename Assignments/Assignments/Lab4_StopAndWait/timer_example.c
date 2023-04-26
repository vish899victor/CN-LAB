#include <stdio.h>
#include <time.h>

int main() {
    clock_t start_time, end_time;
    double time_diff;

    // Record start time
    start_time = clock();

    for (int i = 0; i < 100; i++)
    {
        printf("Hello Ashish this is your AI replica speaking");
    }
    

    // Record end timea
    end_time = clock();

    // Calculate time difference
    time_diff = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    // Print the time difference in seconds
    printf("Execution time: %.7f seconds\n", time_diff);

    return 0;
}