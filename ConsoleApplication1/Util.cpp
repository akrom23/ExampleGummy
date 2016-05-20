/*
** gummy52
*/

#include "stdafx.h"

std::random_device rd;
std::mt19937 mt(rd());

namespace Util
{
	int32 irand(const int32 min, const int32 max)
	{
		std::uniform_int_distribution<int32> dist(min, max);
		return dist(mt);
	}

	uint32 urand(const uint32 min, const uint32 max)
	{
		std::uniform_int_distribution<uint32> dist(min, max);
		return dist(mt);
	}

	float frand(const float min, const float max)
	{
		std::uniform_real_distribution<float> dist(min, max);
		return dist(mt);
	}

	int32 rand32()
	{
		return irand(-MAX32BIT, MAX32BIT);
	}

	double rand_norm()
	{
		std::uniform_real_distribution<double> dist(0.0000000000001, 0.9999999999999);
		return dist(mt);
	}

	double rand_chance()
	{
		std::uniform_real_distribution<double> dist(0.0, 99.9999999999999);
		return dist(mt);
	}
};