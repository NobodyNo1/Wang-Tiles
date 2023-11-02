#include <time.h>

#ifndef BENCHMARK_HELPER_H
#define BENCHMARK_HELPER_H

#define clockStart()                        \
        double time_spent;                  \
        clock_t begin, end;                 \
        begin = clock();

#define clockRestart()                      \
        begin = clock();

#define clockEnd(message)                                                       \
        end = clock();                                                  \
        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;             \
        printf("    [Benchmark] %s : %f sec\n", message, time_spent);           \


#endif 