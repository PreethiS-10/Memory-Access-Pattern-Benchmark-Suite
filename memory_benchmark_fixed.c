// memory_benchmark_fixed.c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
// Windows high-resolution timer
double get_time() {
    LARGE_INTEGER frequency, counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / frequency.QuadPart;
}
#else
#include <sys/time.h>
double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}
#endif

typedef struct {
    uint32_t a, b, c, d, e, f, g, h;
} DataStruct;

// Larger array for better memory pressure - 128 MiB
#define ARRAY_SIZE (4 * 1024 * 1024)
#define NUM_ITERATIONS 10
#define WARMUP_ITERATIONS 3

DataStruct *arr;
size_t *indices;

void generate_sequential_indices() {
    for (size_t i = 0; i < ARRAY_SIZE / 8; i++) {
        indices[i] = i * 8;
    }
}

void generate_random_indices() {
    generate_sequential_indices();
    
    // Fisher-Yates shuffle
    for (size_t i = ARRAY_SIZE / 8 - 1; i > 0; i--) {
        size_t j = rand() % (i + 1);
        size_t temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }
}

void generate_backward_indices() {
    for (size_t i = 0; i < ARRAY_SIZE / 8; i++) {
        indices[i] = (ARRAY_SIZE / 8 - 1 - i) * 8;
    }
}

void generate_interleaved_indices() {
    size_t half = (ARRAY_SIZE / 8) / 2;
    for (size_t i = 0; i < half; i++) {
        indices[2 * i] = i * 8;
        indices[2 * i + 1] = (half + i) * 8;
    }
}

void generate_bouncing_indices() {
    size_t count = ARRAY_SIZE / 8;
    for (size_t i = 0; i < count; i++) {
        if (i % 2 == 0) {
            indices[i] = (i / 2) * 8;
        } else {
            indices[i] = (count - 1 - i / 2) * 8;
        }
    }
}

double benchmark_pattern(void (*generate_func)(), const char* pattern_name) {
    generate_func();
    
    size_t count = ARRAY_SIZE / 8;
    double times[NUM_ITERATIONS];
    
    // Warmup runs
    for (int w = 0; w < WARMUP_ITERATIONS; w++) {
        volatile uint64_t sum = 0;
        for (size_t j = 0; j < count; j++) {
            sum += arr[indices[j]].a;
        }
    }
    
    // Actual benchmark runs
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        double start = get_time();
        
        volatile uint64_t sum = 0;
        for (size_t j = 0; j < count; j++) {
            sum += arr[indices[j]].a;
        }
        
        double end = get_time();
        times[i] = (end - start) * 1000.0; // Convert to ms
    }
    
    // Calculate median time (more robust than mean)
    // Simple bubble sort for small array
    for (int i = 0; i < NUM_ITERATIONS - 1; i++) {
        for (int j = 0; j < NUM_ITERATIONS - i - 1; j++) {
            if (times[j] > times[j + 1]) {
                double temp = times[j];
                times[j] = times[j + 1];
                times[j + 1] = temp;
            }
        }
    }
    
    double median_time = times[NUM_ITERATIONS / 2];
    printf("%12s: %8.2f ms\n", pattern_name, median_time);
    return median_time;
}

int main() {
    // Allocate memory
    arr = (DataStruct*)malloc(ARRAY_SIZE * sizeof(DataStruct));
    indices = (size_t*)malloc((ARRAY_SIZE / 8) * sizeof(size_t));
    
    if (!arr || !indices) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    
    // Initialize array with random-ish data to prevent compiler optimizations
    srand(12345);
    for (size_t i = 0; i < ARRAY_SIZE; i++) {
        arr[i].a = rand();
        arr[i].b = rand();
        arr[i].c = rand();
        arr[i].d = rand();
        arr[i].e = rand();
        arr[i].f = rand();
        arr[i].g = rand();
        arr[i].h = rand();
    }
    
    printf("Memory Access Pattern Benchmark (C - Windows)\n");
    printf("Array size: %zu elements (%.1f MiB)\n", 
           ARRAY_SIZE, (ARRAY_SIZE * sizeof(DataStruct)) / (1024.0 * 1024.0));
    printf("Accessing every 8th element, %d iterations\n\n", NUM_ITERATIONS);
    
    // Set random seed for reproducible results
    srand(42);
    
    // Run benchmarks
    double sequential = benchmark_pattern(generate_sequential_indices, "Sequential");
    double backward = benchmark_pattern(generate_backward_indices, "Backward");
    double interleaved = benchmark_pattern(generate_interleaved_indices, "Interleaved");
    double bouncing = benchmark_pattern(generate_bouncing_indices, "Bouncing");
    double random = benchmark_pattern(generate_random_indices, "Random");
    
    // Output CSV format for automation
    printf("\nCSV_OUTPUT:\n");
    printf("Pattern,Time_ms\n");
    printf("Sequential,%.2f\n", sequential);
    printf("Backward,%.2f\n", backward);
    printf("Interleaved,%.2f\n", interleaved);
    printf("Bouncing,%.2f\n", bouncing);
    printf("Random,%.2f\n", random);
    
    free(arr);
    free(indices);
    return 0;
}
