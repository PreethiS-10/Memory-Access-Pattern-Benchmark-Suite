# Memory Access Pattern Benchmark Suite

A comprehensive benchmarking tool that demonstrates how hardware prefetchers optimize different memory access patterns in C and C++. This project reproduces and extends Daniel Lemire's research on predictable memory access performance.

[![Language](https://img.shields.io/badge/Language-C%2FC%2B%2FPython-blue)](#)
[![Platform](https://img.shields.io/badge/Platform-Windows%2FLinux%2FmacOS-green)](#)
[![License](https://img.shields.io/badge/License-MIT-yellow)](#)

## 📋 Table of Contents

- [Overview](#overview)
- [Key Findings](#key-findings)
- [Installation](#installation)
- [Usage](#usage)
- [File Structure](#file-structure)
- [Understanding Results](#understanding-results)
- [Technical Background](#technical-background)
- [Requirements](#requirements)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)
- [References](#references)

## Overview

This benchmark suite measures and compares the performance of different memory access patterns to demonstrate the effectiveness of **hardware prefetchers** in modern processors. The project includes:

- ✅ **C and C++ implementations** with Windows/Linux compatibility
- ✅ **Automated compilation and execution** via Python script
- ✅ **Comprehensive visualization** with 4-panel comparison charts
- ✅ **Statistical analysis** with median timing and relative performance metrics
- ✅ **Cross-platform support** with MinGW detection for Windows

## Key Findings

The benchmark demonstrates that **predictable memory access patterns** significantly outperform random access:

| Pattern | Expected Speedup vs Random | Hardware Prefetching Effectiveness |
|---------|----------------------------|-------------------------------------|
| **Sequential** | 2-3x faster | Excellent - optimal prefetching |
| **Backward** | 2-3x faster | Excellent - reverse sequential prefetching |
| **Interleaved** | 1.3-1.5x faster | Moderate - limited pattern recognition |
| **Bouncing** | 1.1-1.3x faster | Poor - unpredictable jumps |
| **Random** | Baseline (1.0x) | None - no prefetching benefit |

## Installation

### Prerequisites

**Windows:**
- MinGW-w64 or Visual Studio Build Tools
- Python 3.7+ with pandas and matplotlib

**Linux/macOS:**
- GCC/G++ compiler
- Python 3.7+ with pandas and matplotlib

### Quick Setup

1. **Clone or download** the project files
2. **Install Python dependencies:**
   ```
   pip install pandas matplotlib
   ```
3. **Verify compiler installation:**
   ```
   gcc --version
   g++ --version
   ```

### Windows MinGW Setup

If gcc is not found on Windows:
1. Install MSYS2 from https://www.msys2.org/
2. Install MinGW-w64:
   ```
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-g++
   ```
3. Add `C:\msys64\mingw64\bin` to your PATH environment variable

## Usage

### Quick Start

Run the complete benchmark suite:
```
python run_both_benchmarks.py
```

### Manual Compilation

Compile individually:
```
# C version
gcc -O3 -std=c99 -Wall memory_benchmark_fixed.c -o memory_benchmark_c

# C++ version  
g++ -O3 -std=c++17 -Wall memory_benchmark_fixed.cpp -o memory_benchmark_cpp
```

### Expected Output

```
=== COMPLETE Memory Access Benchmark Suite ===

Compiling C version...
[SUCCESS] C compilation successful
[SUCCESS] C benchmark completed

Compiling C++ version...
[SUCCESS] C++ compilation successful  
[SUCCESS] C++ benchmark completed

Comparative Results:
                C   C++
Sequential   2.57  2.61
Backward     2.59  2.63
Interleaved  3.99  4.12
Bouncing     4.71  4.89
Random       5.37  5.44

Summary Insights:
   -  Sequential (C): 2.1x faster than Random
   -  Backward (C): 2.1x faster than Random
   -  Interleaved (C): 1.3x faster than Random
   -  Bouncing (C): 1.1x faster than Random
```

## File Structure

```
memory-benchmark-suite/
├── README.md                           # This file
├── run_both_benchmarks.py             # Main automation script
├── memory_benchmark_fixed.c           # Windows-compatible C implementation
├── memory_benchmark_fixed.cpp         # Windows-compatible C++ implementation
├── complete_benchmark_results.csv     # Generated results data
├── relative_performance_results.csv   # Generated speedup data
└── complete_memory_benchmark_comparison.png  # Generated 4-panel chart
```

### Generated Files

- **`complete_benchmark_results.csv`**: Raw timing data for all patterns and languages
- **`relative_performance_results.csv`**: Speedup factors relative to random access
- **`complete_memory_benchmark_comparison.png`**: 4-panel visualization chart

## Understanding Results

### Chart Interpretation Guide

| Chart | Metric | Better Performance |
|-------|--------|--------------------|
| **Top-Left**: Memory Access Pattern Performance | Time (ms) | **Lower is better** ⬇️ |
| **Top-Right**: Relative Performance | Speedup Factor | **Higher is better** ⬆️ |
| **Bottom-Left**: C vs C++ Performance Ratio | Time Ratio | < 1.0 = C faster, > 1.0 = C++ faster |
| **Bottom-Right**: Hardware Prefetching Effectiveness | Average Speedup | **Higher is better** ⬆️ |

### Performance Patterns

1. **Sequential & Backward Access**: Highest speedup (~2-3x)
   - Hardware prefetchers excel at linear memory patterns
   - Cache lines loaded ahead of actual memory requests

2. **Interleaved Access**: Moderate speedup (~1.3x)
   - Mixed pattern provides some prefetching benefit
   - Alternating between two linear sequences

3. **Bouncing Access**: Limited speedup (~1.1x)  
   - Jumping between memory locations
   - Minimal prefetching effectiveness

4. **Random Access**: Baseline performance (1.0x)
   - No predictable pattern for prefetchers
   - Pure memory latency without optimization

## Technical Background

### Memory Access Patterns

This benchmark tests five distinct access patterns on a **128 MiB array** (4M elements × 32 bytes):

1. **Sequential**: `arr[0], arr[8], arr[16], arr[24], ...`
2. **Backward**: `arr[max], arr[max-8], arr[max-16], ...`  
3. **Interleaved**: `arr[0], arr[middle], arr[8], arr[middle+8], ...`
4. **Bouncing**: `arr[0], arr[max], arr[8], arr[max-8], ...`
5. **Random**: Shuffled version of sequential indices

### Benchmark Methodology

- **Array Size**: 128 MiB (4,194,304 elements) for meaningful memory pressure
- **Access Stride**: Every 8th element to minimize cache line effects  
- **Iterations**: 10 runs with median calculation for statistical robustness
- **Warmup**: 3 initial runs excluded from timing to stabilize cache state
- **Timing**: High-resolution timers (QueryPerformanceCounter on Windows, chrono on Linux)

### Hardware Prefetching

Modern processors include **hardware prefetchers** that:
- Monitor memory access patterns in real-time
- Predict future memory requests based on observed patterns
- Preload data into cache hierarchy before CPU requests it
- Significantly reduce memory latency for predictable access patterns

## Requirements

### System Requirements
- **Memory**: Minimum 1 GB RAM (benchmark uses ~128 MiB)
- **CPU**: Any modern x64 processor (Intel/AMD)
- **Disk**: 50 MB for source files and results

### Software Dependencies
- **Compilers**: GCC/G++ 4.9+ or Visual Studio 2015+
- **Python**: 3.7+ with packages:
  ```
  pip install pandas>=1.3.0 matplotlib>=3.3.0
  ```

## Troubleshooting

### Common Issues

**1. "gcc not found" on Windows**
```
# Solution: Install MinGW-w64 and add to PATH
# Or use the script's auto-detection feature
```

**2. Compilation errors with clock_gettime**
```
# Use memory_benchmark_fixed.c which includes Windows-compatible timing
```

**3. Very small timing differences**
```
# Increase ARRAY_SIZE in source code for more memory pressure
# Ensure system isn't under heavy load during benchmarking
```

**4. Inconsistent results**
```
# Close other applications to reduce system noise
# Run multiple times and compare median results
```

### Validation

Expected behavior:
- Sequential/Backward should be 2-3x faster than Random
- C and C++ results should be similar (within 10%)
- Random access should show highest absolute time values
- All speedup factors should be ≥ 1.0x

## Contributing

Contributions welcome! Areas for enhancement:

- **Additional access patterns** (stride variations, linked-list traversal)
- **Different array sizes** and cache pressure scenarios  
- **ARM/RISC-V architecture** support and comparison
- **Vectorization analysis** with SIMD instructions
- **Multi-threading** impact on prefetching effectiveness

### Development Setup
1. Fork the repository
2. Create feature branch: `git checkout -b feature-name`
3. Test on multiple platforms
4. Submit pull request with benchmark results

## References

- **Daniel Lemire**: ["Predictable memory accesses are much faster"](https://lemire.me/blog/2025/08/15/predictable-memory-accesses-are-much-faster/)
- **Intel Optimization Manual**: Hardware prefetching mechanisms
- **Algorithmica HPC**: [CPU cache and prefetching guide](https://en.algorithmica.org/hpc/cpu-cache/prefetching/)
- **What Every Programmer Should Know About Memory** - Ulrich Drepper

---

**License**: MIT License  
**Author**: Based on Daniel Lemire's research, extended for cross-platform benchmarking  
**Last Updated**: September 2025

---

> 💡 **Tip**: For best results, run benchmarks on a quiet system with minimal background processes. The performance differences demonstrate fundamental hardware optimization principles that affect all memory-intensive applications.
