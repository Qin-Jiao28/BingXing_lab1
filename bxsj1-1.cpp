#include <iostream>
#include <vector>
#include <windows.h> 
#include <iomanip>

using namespace std;

#define VOLATILE_CHECK(x) __asm__ __volatile__("" : : "g"(x) : "memory")

void naive_matrix_vector(int n, const vector<vector<double>>& A, const vector<double>& b, vector<double>& sum) {
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < n; i++) {
            sum[i] += A[i][j] * b[j];
        }
    }
}

void optimized_matrix_vector(int n, const vector<vector<double>>& A, const vector<double>& b, vector<double>& sum) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            sum[i] += A[i][j] * b[j];
        }
    }
}

int main() {
    // 1. 获取 QPC 频率（每秒的刻度数）
    LARGE_INTEGER freq_info;
    QueryPerformanceFrequency(&freq_info);
    double frequency = (double)freq_info.QuadPart;

    vector<int> sizes = {256, 512, 1024, 1500, 2000, 2500, 3000, 4000, 6000, 8000, 10000};

    cout << "N,Naive_Time(s),Optimized_Time(s),Speedup,DataSize(MB)" << endl;

    for (int n : sizes) {
        vector<vector<double>> A(n, vector<double>(n, 1.2));
        vector<double> b(n, 1.1);
        vector<double> sum(n, 0.0);

        int repeats = (n < 2000) ? 50 : 1;
        LARGE_INTEGER start, end;

        // --- 测试 Naive ---
        QueryPerformanceCounter(&start); // 记录开始刻度
        for(int r = 0; r < repeats; r++) {
            naive_matrix_vector(n, A, b, sum);
        }
        QueryPerformanceCounter(&end);   // 记录结束刻度
        // 计算平均耗时：(结束-开始) / 频率 / 重复次数
        double t1 = (double)(end.QuadPart - start.QuadPart) / frequency / repeats;
        VOLATILE_CHECK(sum[0]);

        // 重置 sum
        fill(sum.begin(), sum.end(), 0.0);

        // --- 测试 Optimized ---
        QueryPerformanceCounter(&start);
        for(int r = 0; r < repeats; r++) {
            optimized_matrix_vector(n, A, b, sum);
        }
        QueryPerformanceCounter(&end);
        double t2 = (double)(end.QuadPart - start.QuadPart) / frequency / repeats;
        VOLATILE_CHECK(sum[0]);

        double data_mb = (double)n * n * 8 / 1024 / 1024;
        
        // 输出结果，保持 8 位小数
        cout << n << "," 
             << fixed << setprecision(8) << t1 << "," 
             << t2 << "," 
             << (t2 > 0 ? t1/t2 : 0) << "," 
             << data_mb << endl;
    }
    return 0;
}
