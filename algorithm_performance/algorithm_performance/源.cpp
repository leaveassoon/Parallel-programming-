#include <iostream>
#include <windows.h>
#include <stdlib.h>

using namespace std;

const int N = 10240; // matrix size
const int A = 64000;
double b[N][N], col_sum[A],a[A];
void init(int n) // generate a N∗N matrix
{
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        b[i][j] = i + j;
    for (int i = 0; i < A; i++)
        col_sum[i] = 0;
    for (int i = 0; i < A; i++)
        a[i] = i;
    }
 int main()
{
    long long head, tail, freq; // timers
    init(N);
    int num = 20000;
   // similar to CLOCKS_PER_SEC
        QueryPerformanceFrequency((LARGE_INTEGER *) & freq);
    // start time
       QueryPerformanceCounter((LARGE_INTEGER *) & head);
    // for (int i = 0; i < num; i++) {
    //    col_sum[i] = 0.0;
    //    for (int j = 0; j < num; j++)
    //         col_sum[i] += b[j][i]* a[j];
    //    
    //}//矩阵向量乘法平凡算法
    // 
    //for (int i = 0; i < num; i++)
    //     col_sum[i] = 0.0;
    //for (int j = 0; j < num; j++)
    //   for (int i = 0; i < num; i++)
    //     col_sum[i] += b[j][i] * a[j];//矩阵向量乘法优化算法


       //for (int i = 0; i < num; i++)
       //    col_sum[0] += a[i];//加法平凡算法
       for (int i = 0; i < num; i += 2) {
            col_sum[1] += a[i];
            col_sum[2] += a[i + 1];
           
       }
       col_sum[0] =col_sum[1] + col_sum[2];
     // end time
         QueryPerformanceCounter((LARGE_INTEGER *) & tail);
     cout <<  "col:" << (tail - head) * 1000.0 / freq
         << "ms" << endl;

   }