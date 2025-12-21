#include <cstdint>
#include <gtest/gtest.h>
#include "../../remc2/utilities/Maths.h"

unsigned int pseudoRand()
{
	static unsigned int nSeed = 5323;
	nSeed = (8253729 * nSeed + 2396403);
	return nSeed % 32767;
}

TEST(Math, RotateX)
{
	Maths::Vec3 point{ 0, 1, 1 };
	auto rotatedPoint = Maths::RotateX(point, 90);

	ASSERT_EQ((int)round(rotatedPoint.x), 0);
	ASSERT_EQ((int)round(rotatedPoint.y), 1);
	ASSERT_EQ((int)round(rotatedPoint.z), -1);
}

TEST(Math, RotateY)
{
	Maths::Vec3 point{ 1, 0, 1};
	auto rotatedPoint = Maths::RotateY(point, 90);

	ASSERT_EQ((int)round(rotatedPoint.x), 0);
	ASSERT_EQ((int)round(rotatedPoint.y), 1);
	ASSERT_EQ((int)round(rotatedPoint.z), 1);
}