#include <iostream>
#include <vector>
#include <windows.h> 
#include <iomanip>

using namespace std;

const long long MAX_N = 100000000;
float a[MAX_N];

void init(long long n) {
    for (long long i = 0; i < n; i++) a[i] = 1.0f;
}

float naive_sum(long long n) {
    float s = 0;
    for (long long i = 0; i < n; i++) s += a[i];
    return s;
}

float tree_parallel_sum(long long n) {
    float s1 = 0, s2 = 0;
    for (long long i = 0; i < n; i += 2) {
        s1 += a[i]; s2 += a[i+1];
    }
    return s1 + s2;
}

int main() {
    // 获取计时器频率（每秒多少个刻度）
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    double freq = (double)frequency.QuadPart;

    vector<long long> sizes = {10000000, 30000000, 50000000, 80000000, 100000000};
    cout << "N,Naive(s),TreeParallel(s),Speedup" << endl;

    for (long long n : sizes) {
        init(n);
        const int REPEATS = 100;
        float dummy = 0;

        LARGE_INTEGER start, end;

        // 测试 Naive
        QueryPerformanceCounter(&start);
        for(int r = 0; r < REPEATS; r++) dummy += naive_sum(n);
        QueryPerformanceCounter(&end);
        double t1 = (end.QuadPart - start.QuadPart) / freq / REPEATS;

        // 测试 Tree-Parallel
        QueryPerformanceCounter(&start);
        for(int r = 0; r < REPEATS; r++) dummy += tree_parallel_sum(n);
        QueryPerformanceCounter(&end);
        double t2 = (end.QuadPart - start.QuadPart) / freq / REPEATS;

        cout << n << "," << fixed << setprecision(8) << t1 << "," << t2 << "," << t1/t2 << endl;
        if(dummy == 0) cout << " ";
    }
    return 0;
}