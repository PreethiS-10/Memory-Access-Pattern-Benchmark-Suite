
import subprocess
import pandas as pd
import matplotlib.pyplot as plt
import os
import sys
import shutil

class CompleteBenchmarkRunner:
    def __init__(self):
        self.results = {}
        self.is_windows = sys.platform.startswith('win')
    
    def find_compilers(self):
        """Find and setup compilers"""
        # Add MinGW paths for Windows
        if self.is_windows:
            mingw_paths = [r"C:\MinGW\bin", r"C:\mingw64\bin", r"C:\msys64\mingw64\bin"]
            for path in mingw_paths:
                if os.path.exists(path) and path not in os.environ.get('PATH', ''):
                    os.environ['PATH'] = path + os.pathsep + os.environ.get('PATH', '')
                    print(f"[INFO] Added {path} to PATH")
        
        compilers = {}
        compilers['c'] = shutil.which('gcc')
        compilers['cpp'] = shutil.which('g++')
        
        return compilers
    
    def compile_and_run_c(self):
        """Compile and run C version"""
        compilers = self.find_compilers()
        
        if not compilers['c']:
            print("[ERROR] C compiler (gcc) not found")
            return False
        
        print("Compiling C version...")
        
        source_file = 'memory_benchmark_fixed.c'
        output_file = 'memory_benchmark_c.exe' if self.is_windows else 'memory_benchmark_c'
        
        if not os.path.exists(source_file):
            print(f"[ERROR] {source_file} not found")
            return False
        
        compile_cmd = ['gcc', '-O3', '-std=c99', '-Wall', source_file, '-o', output_file]
        
        try:
            result = subprocess.run(compile_cmd, capture_output=True, text=True)
            if result.returncode != 0:
                print(f"[ERROR] C compilation failed: {result.stderr}")
                return False
            
            print("[SUCCESS] C compilation successful")
            
            # Run benchmark
            result = subprocess.run([output_file], capture_output=True, text=True, cwd='.')
            if result.returncode == 0:
                print("[SUCCESS] C benchmark completed")
                self.parse_output(result.stdout, "C")
                return True
            else:
                print(f"[ERROR] C execution failed: {result.stderr}")
                return False
                
        except Exception as e:
            print(f"[ERROR] Error: {e}")
            return False
    
    def compile_and_run_cpp(self):
        """Compile and run C++ version"""
        compilers = self.find_compilers()
        
        if not compilers['cpp']:
            print("[ERROR] C++ compiler (g++) not found")
            return False
        
        print("Compiling C++ version...")
        
        source_file = 'memory_benchmark_fixed.cpp'
        output_file = 'memory_benchmark_cpp.exe' if self.is_windows else 'memory_benchmark_cpp'
        
        if not os.path.exists(source_file):
            print(f"[ERROR] {source_file} not found")
            return False
        
        compile_cmd = ['g++', '-O3', '-std=c++17', '-Wall', source_file, '-o', output_file]
        
        try:
            result = subprocess.run(compile_cmd, capture_output=True, text=True)
            if result.returncode != 0:
                print(f"[ERROR] C++ compilation failed: {result.stderr}")
                return False
            
            print("[SUCCESS] C++ compilation successful")
            
            # Run benchmark
            result = subprocess.run([output_file], capture_output=True, text=True, cwd='.')
            if result.returncode == 0:
                print("[SUCCESS] C++ benchmark completed")
                self.parse_output(result.stdout, "C++")
                return True
            else:
                print(f"[ERROR] C++ execution failed: {result.stderr}")
                return False
                
        except Exception as e:
            print(f"[ERROR] Error: {e}")
            return False
    
    def parse_output(self, output, language):
        """Parse benchmark output"""
        lines = output.split('\n')
        csv_start = False
        
        for line in lines:
            if 'CSV_OUTPUT:' in line:
                csv_start = True
                continue
            elif csv_start and 'Pattern,Time_ms' in line:
                continue
            elif csv_start and ',' in line and line.strip():
                parts = line.strip().split(',')
                if len(parts) == 2:
                    pattern, time_str = parts
                    try:
                        time_ms = float(time_str)
                        if pattern not in self.results:
                            self.results[pattern] = {}
                        self.results[pattern][language] = time_ms
                    except ValueError:
                        continue
    
    def create_comparative_chart(self):
        """Create side-by-side comparison chart"""
        if not self.results:
            print("[ERROR] No results to chart")
            return
        
        df = pd.DataFrame(self.results).T
        print("\nComparative Results:")
        print(df.round(2))
        
        # Create comprehensive comparison chart
        fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(16, 12))
        
        # Chart 1: Absolute performance comparison
        df.plot(kind='bar', ax=ax1, width=0.8, color=['#2E86AB', '#A23B72'])
        ax1.set_title('Absolute Performance: C vs C++ - LOWER IS BETTER ', fontsize=14, fontweight='bold')
        ax1.set_ylabel('Time (ms)', fontsize=12)
        ax1.legend(title='Language')
        ax1.grid(True, axis='y', alpha=0.3)
        ax1.tick_params(axis='x', rotation=45)
        
        # Add value labels
        for container in ax1.containers:
            ax1.bar_label(container, fmt='%.1f', fontsize=9)
        
        # Chart 2: Relative performance (Random = 1.0) for each language
        if 'Random' in df.index:
            df_relative = pd.DataFrame()
            for col in df.columns:
                df_relative[col] = df.loc['Random', col] / df[col]
            
            df_relative.plot(kind='bar', ax=ax2, width=0.8, color=['#F18F01', '#C73E1D'])
            ax2.set_title('Relative Performance (Random = 1.0) - HIGHER IS BETTER', fontsize=14, fontweight='bold')
            ax2.set_ylabel('Speedup Factor', fontsize=12)
            ax2.legend(title='Language')
            ax2.grid(True, axis='y', alpha=0.3)
            ax2.tick_params(axis='x', rotation=45)
            
            for container in ax2.containers:
                ax2.bar_label(container, fmt='%.1fx', fontsize=9)
        
        # Chart 3: C vs C++ Performance Ratio
        if len(df.columns) == 2:
            c_col = df.columns[0]
            cpp_col = df.columns[1]
            ratio = df[c_col] / df[cpp_col]
            
            colors = ['green' if r < 1 else 'red' if r > 1 else 'gray' for r in ratio]
            ax3.bar(ratio.index, ratio.values, color=colors, alpha=0.7)
            ax3.axhline(y=1.0, color='black', linestyle='--', alpha=0.5)
            ax3.set_title(f'{c_col} vs {cpp_col} Performance Ratio Values < 1.0:C is faster, > 1.0:C++ is faster', fontsize=14, fontweight='bold')
            ax3.set_ylabel('Time Ratio (< 1.0 = C faster)', fontsize=12)
            ax3.grid(True, axis='y', alpha=0.3)
            ax3.tick_params(axis='x', rotation=45)
            
            for i, v in enumerate(ratio.values):
                ax3.text(i, v + 0.02, f'{v:.2f}', ha='center', fontsize=9)
        
        # Chart 4: Memory Access Pattern Insights
        patterns = df.index.tolist()
        if 'Random' in patterns:
            patterns.remove('Random')
            
        insights = []
        for pattern in patterns:
            if 'Random' in df.index:
                speedup = df.loc['Random'].mean() / df.loc[pattern].mean()
                insights.append((pattern, speedup))
        
        if insights:
            insights.sort(key=lambda x: x[1], reverse=True)
            pattern_names, speedups = zip(*insights)
            
            bars = ax4.bar(pattern_names, speedups, color='#4CAF50', alpha=0.8)
            ax4.set_title('Hardware Prefetching Effectiveness - HIGHER IS BETTER', fontsize=14, fontweight='bold')
            ax4.set_ylabel('Average Speedup vs Random', fontsize=12)
            ax4.grid(True, axis='y', alpha=0.3)
            ax4.tick_params(axis='x', rotation=45)
            
            for bar, speedup in zip(bars, speedups):
                ax4.text(bar.get_x() + bar.get_width()/2, speedup + 0.05,
                        f'{speedup:.1f}x', ha='center', fontsize=10, fontweight='bold')
        
        plt.tight_layout()
        plt.savefig('complete_memory_benchmark_comparison.png', dpi=300, bbox_inches='tight')
        print("Chart saved: complete_memory_benchmark_comparison.png")
        plt.show()
        
        # Save detailed results
        df.to_csv('complete_benchmark_results.csv')
        if 'Random' in df.index:
            df_relative.to_csv('relative_performance_results.csv')
        print("Results saved to CSV files")
    
    def run_complete_suite(self):
        """Run both C and C++ benchmarks"""
        print("=== COMPLETE Memory Access Benchmark Suite ===\n")
        
        c_success = self.compile_and_run_c()
        print("\n" + "-"*50 + "\n")
        cpp_success = self.compile_and_run_cpp()
        
        print("\n" + "="*50)
        
        if c_success or cpp_success:
            self.create_comparative_chart()
            
            # Print summary insights
            if self.results:
                print("\nSummary Insights:")
                if 'Random' in [list(v.keys())[0] for v in self.results.values() if v]:
                    patterns = list(self.results.keys())
                    if 'Random' in patterns:
                        patterns.remove('Random')
                        random_times = self.results['Random']
                        
                        for pattern in patterns:
                            pattern_times = self.results.get(pattern, {})
                            for lang in pattern_times:
                                if lang in random_times:
                                    speedup = random_times[lang] / pattern_times[lang]
                                    print(f"   • {pattern} ({lang}): {speedup:.1f}x faster than Random")
            
            print("\nComplete benchmark suite finished!")
        else:
            print("\nNo successful benchmarks")

if __name__ == "__main__":
    runner = CompleteBenchmarkRunner()
    runner.run_complete_suite()
