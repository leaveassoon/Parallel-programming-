#include<iostream>
#include<fstream>
#include<stdlib.h>
#include<sys/time.h>
#include<ctime>

#include<arm_neon.h>  


using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;

void reset(float** a, int n);
void c_reset(float** a, int n);
void gausselimination(float** a, int n);
void neon_n_gausselimination(float** a, int n);
void neon_gausselimination(float** a, int n);



int n = 2048;
int lim = 1;
ofstream out("output.txt");



void gausselimination(float** a, int n) { 
	for (int k = 0; k < n; k++)
	{
		for (int j = k + 1; j < n; j++)
		{
			a[k][j] = a[k][j] / a[k][k];
		}
		a[k][k] = 1.0;
		for (int i = k + 1; i < n; i++)
		{
			for (int j = k + 1; j < n; j++)
			{
				a[i][j] -= a[i][k] * a[k][j];
			}
			a[i][k] = 0;
		}
	}
}


void neon_gausselimination(float** a, int n) {
	float32x4_t va, vt, vaik, vakj, vaij, vx;
	float t1, t2;
	for (int k = 0; k < n; k++)
	{
		vt = vmovq_n_f32(a[k][k]);
		t1 = a[k][k];
		int j = 0;
		for (j = k + 1; j + 4 < n; j += 4)
		{
			if (j % 4 != 0) {
				a[k][j] = a[k][j] / t1;
				j -= 3;
				continue;
			}
			va = vld1q_f32(&a[k][j]);
			va = vdivq_f32(va, vt);
			vst1q_f32((float32_t*)&a[k][j], va);
		}
		for (j; j < n; j++)
		{
			a[k][j] = a[k][j] / a[k][k];
			a[k][k] = 1.0;
			for (int i = k + 1; i < n; i++)
			{
				vaik = vmovq_n_f32(a[i][k]);
				for (j = k + 1; j + 4 < n; j += 4)
				{
					if (j % 4 != 0) {
						a[i][j] -= t2 * a[k][j];
						j -= 3;
						continue;
					}
					vakj = vld1q_f32(&a[k][j]);
					vaij = vld1q_f32(&a[i][j]);
					vx = vmulq_f32(vakj, vaik);
					vaij = vsubq_f32(vaij, vx);
					vst1q_f32((float32_t*)&a[i][j], vaij);
				}
				for (j; j < n; j++)
				{
					a[i][j] -= a[i][k] * a[k][j];
				}
				a[i][k] = 0;
			}
		}
	}
}

void neon_n_gausselimination(float** a, int n) {  
	float32x4_t va, vt, vaik, vakj, vaij, vx;
	for (int k = 0; k < n; k++)
	{
		vt = vmovq_n_f32(a[k][k]);  
		int j = 0;
		for (j = k + 1; j + 4 < n; j += 4)
		{
			va = vld1q_f32(&a[k][j]);
			va = vdivq_f32(va, vt);
			vst1q_f32((float32_t*)&a[k][j], va);
		}
		for (j; j < n; j++)
		{
			a[k][j] = a[k][j] / a[k][k];  
		}
		a[k][k] = 1.0;
		for (int i = k + 1; i < n; i++)
		{
			vaik = vmovq_n_f32(a[i][k]);
			for (j = k + 1; j + 4 < n; j += 4)
			{
				vakj = vld1q_f32(&a[k][j]);
				vaij = vld1q_f32(&a[i][j]);
				vx = vmulq_f32(vakj, vaik);
				vaij = vsubq_f32(vaij, vx);
				vst1q_f32((float32_t*)&a[i][j], vaij);

			}
			for (j; j < n; j++)
			{
				a[i][j] -= a[i][k] * a[k][j];
			}
			a[i][k] = 0;
		}
	}
}



float** generate(int n) {
	float** m = new float* [n];
	for (int i = 0; i < n; i++)
	{
		m[i] = new float[n];
	}
	reset(m, n);
	return m;
}

float** aligned_generate(int n) {
	float** m = (float**)aligned_alloc(32 * n * sizeof(float**), 32);
	for (int i = 0; i < n; i++)
	{
		m[i] = (float*)aligned_alloc(32 * n * sizeof(float*), 32);
	}
	reset(m, n);
	return m;
}

float** proof_generate(int n) {
	ifstream inp("input.txt");
	inp >> n;
	float** m = new float* [n];
	for (int i = 0; i < n; i++)
	{
		m[i] = new float[n];
		for (int j = 0; j < n; j++)
		{
			inp >> m[i][j];
		}
	}
	inp.close();
	return m;
}

float** aligned_proof_generate(int n) {
	ifstream inp("input.txt");
	inp >> n;
	float** m = (float**)aligned_alloc(32 * n * sizeof(float**), 32);
	for (int i = 0; i < n; i++)
	{
		m[i] = (float*)aligned_alloc(32 * n * sizeof(float*), 32);
	}
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			inp >> m[i][j];
		}
	}
	inp.close();
	return m;
}

void reset(float** a, int n)  
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			a[i][j] = rand() % 10;
			
		}
	
	}
}

void c_reset(float** a, int n)  
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < i; j++)
			a[i][j] = 0;
		a[i][i] = 1.0;
		for (int j = i + 1; j < n; j++)
			a[i][j] = rand();
	}
	for (int k = 0; k < n; k++)
		for (int i = k + 1; i < n; i++)
			for (int j = 0; j < n; j++)
				a[i][j] += a[k][j];
}



int main() {
	srand(time(0));
	
	cin >> n;
	out << n << endl;
	float time_use = 0;
	struct timeval start1, start2, start3;
	struct timeval end1, end2, end3;

	float** m1 = proof_generate(n);
	gettimeofday(&start1, NULL); 
          gausselimination(m1, n);
	gettimeofday(&end1, NULL); 
	cout << endl << endl << endl;
	time_use = (end1.tv_sec - start1.tv_sec) * 1000000 + (end1.tv_usec - start1.tv_usec);//Î¢Ãë
	cout << "gausselimination: " << time_use / 1000<< "ms" << endl;




	float** m2 = proof_generate(n);
	gettimeofday(&start2, NULL);
	neon_n_gausselimination(m2, n);
	gettimeofday(&end2, NULL); 
	time_use = (end2.tv_sec - start2.tv_sec) * 1000000 + (end2.tv_usec - start2.tv_usec);
	cout << "neon_n_gausselimination: " << time_use / 1000<< "ms" << endl;


	float** m3 = aligned_proof_generate(n);
	gettimeofday(&start3, NULL); 
	neon_gausselimination(m3, n);
	gettimeofday(&end3, NULL); 
	time_use = (end3.tv_sec - start3.tv_sec) * 1000000 + (end3.tv_usec - start3.tv_usec);
	cout << "neon_gausselimination " << time_use / 1000<< "ms" << endl;


}