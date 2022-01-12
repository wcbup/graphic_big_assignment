#pragma once

struct Vector3f
{
	float x;
	float y;
	float z;
	Vector3f()
	{
		x = y = z = 0;
	}
	Vector3f(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}
};
