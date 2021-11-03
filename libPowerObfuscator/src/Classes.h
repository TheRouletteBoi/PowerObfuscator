#pragma once

template<int A, int C, int M> 
class Random
{
public:
	Random(unsigned int pSeed) : seed(pSeed) {}

	unsigned int next()
	{
		return seed = (A * seed + C) % M;
	}

private:
	unsigned int seed;
};