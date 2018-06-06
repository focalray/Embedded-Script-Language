// PerlinNoise.cpp : Defines the entry point for the console application.
//

#pragma once
#include "memory.h"
#include "stdlib.h"
#include "math.h"


class CCPUPerlinNoise
{
	#define SIZE 4096
	float gx[SIZE];
	float gy[SIZE];
	int permtbl[SIZE];

public:
	CCPUPerlinNoise()
	{
		memset(gx, 0, sizeof(float)*SIZE);
		memset(gy, 0, sizeof(float)*SIZE);
		memset(permtbl, 0, sizeof(int)*SIZE);

		// Initialize the permutation table
		for(int i = 0; i < SIZE; i++)
			permtbl[i] = i;

		for(int i = 0; i < SIZE; i++)
		{
			int j = rand() % SIZE;
			int nSwap = permtbl[i];
			permtbl[i]  = permtbl[j];
			permtbl[j]  = nSwap;
		}

		// Generate the gradient look-up tables
		for(int i = 0; i < SIZE; i++)
		{
			gx[i] = float(rand())/(RAND_MAX/2) - 1.0f; 
			gy[i] = float(rand())/(RAND_MAX/2) - 1.0f;
		}
	}

	float Noise(float x, float y)
	{
		// Compute the integer positions of the four surrounding points
		int qx0 = (int)floorf(x);
		int qx1 = qx0 + 1;
		int qy0 = (int)floorf(y);
		int qy1 = qy0 + 1;

		// Permutate values to get indices to use with the gradient look-up tables
		int q00 = permtbl[(qy0 + permtbl[qx0 % SIZE]) % SIZE];
		int q01 = permtbl[(qy0 + permtbl[qx1 % SIZE]) % SIZE];
		int q10 = permtbl[(qy1 + permtbl[qx0 % SIZE]) % SIZE];
		int q11 = permtbl[(qy1 + permtbl[qx1 % SIZE]) % SIZE];


		// Computing vectors from the four points to the input point
		float tx0 = x - floorf(x);
		float tx1 = tx0 - 1;
		float ty0 = y - floorf(y);
		float ty1 = ty0 - 1;

		// Compute the dot-product between the vectors and the gradients
		float v00 = gx[q00]*tx0 + gy[q00]*ty0;
		float v01 = gx[q01]*tx1 + gy[q01]*ty0;
		float v10 = gx[q10]*tx0 + gy[q10]*ty1;
		float v11 = gx[q11]*tx1 + gy[q11]*ty1;

		// Do the bi-cubic interpolation to get the final value
		float wx = (3 - 2*tx0)*tx0*tx0;
		float v0 = v00 - wx*(v00 - v01);
		float v1 = v10 - wx*(v10 - v11);
		float wy = (3 - 2*ty0)*ty0*ty0;
		float v = v0 - wy*(v0 - v1);

		return v;
	}
};

/*
int _tmain()
{
	CCPUPerlinNoise pnoise;

	for(float x=0.0f; x<10.0f; x+=0.0001f)
	{
		printf("pnoise = %f\n", pnoise.Noise(x, x+0.1f));
	}

	return 0;
}

*/
