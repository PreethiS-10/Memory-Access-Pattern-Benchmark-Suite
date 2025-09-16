// memory_benchmark_fixed.cpp
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <iomanip>

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
double get_time() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration<double>(duration).count();
}
#endif

struct DataStruct {
    uint32_t a, b, c, d, e, f, g, h;
};

class MemoryBenchmark {
private:
    static constexpr size_t ARRAY_SIZE = 4 * 1024 * 1024; // 128 MiB
    static constexpr int NUM_ITERATIONS = 10;
    static constexpr int WARMUP_ITERATIONS = 3;
    
    std::vector<DataStruct> arr;
    std::vector<size_t> indices;
    std::mt19937 rng{42}; // Fixed seed
    
public:
    MemoryBenchmark() : arr(ARRAY_SIZE), indices(ARRAY_SIZE / 8) {
        // Initialize with random data to prevent optimizations
        std::random_device rd;
        std::mt19937 gen(12345); // Fixed seed for reproducibility
        
        for (size_t i = 0; i < ARRAY_SIZE; i++) {
            arr[i] = {
                static_cast<uint32_t>(gen()),
                static_cast<uint32_t>(gen()),
                static_cast<uint32_t>(gen()),
                static_cast<uint32_t>(gen()),
                static_cast<uint32_t>(gen()),
                static_cast<uint32_t>(gen()),
                static_cast<uint32_t>(gen()),
                static_cast<uint32_t>(gen())
            };
        }
    }
    
    void generateSequentialIndices() {
        for (size_t i = 0; i < indices.size(); i++) {
            indices[i] = i * 8;
        }
    }
    
    void generateRandomIndices() {
        generateSequentialIndices();
        std::shuffle(indices.begin(), indices.end(), rng);
    }
    
    void generateBackwardIndices() {
        for (size_t i = 0; i < indices.size(); i++) {
            indices[i] = (indices.size() - 1 - i) * 8;
        }
    }
    
    void generateInterleavedIndices() {
        size_t half = indices.size() / 2;
        for (size_t i = 0; i < half; i++) {
            indices[2 * i] = i * 8;
            indices[2 * i + 1] = (half + i) * 8;
        }
    }
    
    void generateBouncingIndices() {
        for (size_t i = 0; i < indices.size(); i++) {
            if (i % 2 == 0) {
                indices[i] = (i / 2) * 8;
            } else {
                indices[i] = (indices.size() - 1 - i / 2) * 8;
            }
        }
    }
    
    template<typename GenerateFunc>
    double benchmarkPattern(GenerateFunc generate, const std::string& patternName) {
        generate();
        
        std::vector<double> times(NUM_ITERATIONS);
        
        // Warmup runs
        for (int w = 0; w < WARMUP_ITERATIONS; w++) {
            volatile uint64_t sum = 0;
            for (size_t j = 0; j < indices.size(); j++) {
                sum += arr[indices[j]].a;
            }
        }
        
        // Benchmark runs
        for (int i = 0; i < NUM_ITERATIONS; i++) {
            double start = get_time();
            
            volatile uint64_t sum = 0;
            for (size_t j = 0; j < indices.size(); j++) {
                sum += arr[indices[j]].a;
            }
            
            double end = get_time();
            times[i] = (end - start) * 1000.0; // Convert to ms
        }
        
        // Calculate median time
        std::sort(times.begin(), times.end());
        double median_time = times[NUM_ITERATIONS / 2];
        
        std::cout << std::setw(12) << patternName << ": " 
                  << std::setw(8) << std::fixed << std::setprecision(2) 
                  << median_time << " ms" << std::endl;
        
        return median_time;
    }
    
    void runBenchmarks() {
        std::cout << "Memory Access Pattern Benchmark (C++ - Windows)" << std::endl;
        std::cout << "Array size: " << ARRAY_SIZE << " elements (" 
                  << (ARRAY_SIZE * sizeof(DataStruct)) / (1024.0 * 1024.0) 
                  << " MiB)" << std::endl;
        std::cout << "Accessing every 8th element, " << NUM_ITERATIONS 
                  << " iterations\n" << std::endl;
        
        auto sequential = benchmarkPattern([this]() { generateSequentialIndices(); }, "Sequential");
        auto backward = benchmarkPattern([this]() { generateBackwardIndices(); }, "Backward");
        auto interleaved = benchmarkPattern([this]() { generateInterleavedIndices(); }, "Interleaved");
        auto bouncing = benchmarkPattern([this]() { generateBouncingIndices(); }, "Bouncing");
        auto random = benchmarkPattern([this]() { generateRandomIndices(); }, "Random");
        
        // Output CSV format for automation
        std::cout << "\nCSV_OUTPUT:" << std::endl;
        std::cout << "Pattern,Time_ms" << std::endl;
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Sequential," << sequential << std::endl;
        std::cout << "Backward," << backward << std::endl;
        std::cout << "Interleaved," << interleaved << std::endl;
        std::cout << "Bouncing," << bouncing << std::endl;
        std::cout << "Random," << random << std::endl;
    }
};

int main() {
    MemoryBenchmark benchmark;
    benchmark.runBenchmarks();
    return 0;
}
