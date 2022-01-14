#pragma once

#include <string>
#include <fstream>
#include <stdio.h>
#include <math.h>
#include <freeglut.h>

#define PI 3.14159265358979323846

using namespace std;

bool readFile(const char* pFileName, string& outFile)
{
	ifstream f(pFileName);

	bool ret = false;

	if (f.is_open())
	{
		string line;
		while (getline(f,line))
		{
			outFile.append(line);
			outFile.append("\n");
		}

		f.close();
		ret = true;
	}
	else
	{
		printf("open file:%s fails", pFileName);
	}
	return ret;
}

class mat4
{
public:
	float m[4][4];
	//init to be identity matrix
	mat4()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (i == j)
				{
					m[i][j] = 1;
				}
				else
				{
					m[i][j] = 0;
				}
			}
		}
	}
	mat4(float a00, float a01, float a02, float a03,
		float a10, float a11, float a12, float a13,
		float a20, float a21, float a22, float a23,
		float a30, float a31, float a32, float a33)
	{
		m[0][0] = a00; m[0][1] = a01; m[0][2] = a02; m[0][3] = a03;
		m[1][0] = a10; m[1][1] = a11; m[1][2] = a12; m[1][3] = a13;
		m[2][0] = a20; m[2][1] = a21; m[2][2] = a22; m[2][3] = a23;
		m[3][0] = a30; m[3][1] = a31; m[3][2] = a32; m[3][3] = a33;
	}

	mat4 operator*(const mat4& Right) const
	{
		mat4 Ret;
		for (unsigned int i = 0; i < 4; i++) 
		{
			for (unsigned int j = 0; j < 4; j++) 
			{
				Ret.m[i][j] = m[i][0] * Right.m[0][j] +
					m[i][1] * Right.m[1][j] +
					m[i][2] * Right.m[2][j] +
					m[i][3] * Right.m[3][j];
			}
		}
		return Ret;
	}
};

class vec3
{
public:
	float x;
	float y;
	float z;

	vec3()
	{
		x = y = z = 0;
	}

	vec3(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	//be aware that we are in the left hand coordinate!!
	vec3 cross(const vec3& v)
	{
		float _x = y * v.z - z * v.y;
		float _y = z * v.x - x * v.z;
		float _z = x * v.y - y * v.x;
		vec3 ret(_x, _y, _z);
		return ret;
	}

	void normalize()
	{
		float length = sqrtf(x * x + y * y + z * z);
		if (length == 0)
		{
			printf("normalize fail!\n");
			exit(1);
		}
		x /= length;
		y /= length;
		z /= length;
	}

	vec3 operator+(const vec3& r)
	{
		vec3 ret(
			x + r.x,
			y + r.y,
			z + r.z
		);
		return ret;
	}

	vec3 operator-(const vec3& r)
	{
		vec3 ret(
			x - r.x,
			y - r.y,
			z - r.z
		);
		return ret;
	}
};

vec3 operator*(const vec3& l, float r)
{
	vec3 ret(
		l.x * r,
		l.y * r,
		l.z * r
	);
	return ret;
}

//transform from local coordinate to world coordinate
class worldTransform
{
public:
	mat4& getMatrix()
	{
		return matrix;
	}

	void scale(float a)
	{
		matrix.m[0][0] *= a;
		matrix.m[1][1] *= a;
		matrix.m[2][2] *= a;
	}

	//transplate to (x, y, z)
	void transplate(float x, float y, float z)
	{
		matrix.m[0][3] += x;
		matrix.m[1][3] += y;
		matrix.m[2][3] += z;
	}

	//rotate a radians around x-zxis
	void rotateX(float a)
	{
		mat4 rotate(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, cosf(a), -sinf(a), 0.0f,
			0.0f, sinf(a), cosf(a), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
		matrix = rotate * matrix;
	}
	//rotate a radians around y-zxis
	void rotateY(float a)
	{
		mat4 rotate(
			cosf(a), 0.0f, -sinf(a), 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			sinf(a), 0.0f, cosf(a), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
		matrix = rotate * matrix;
	}
	//rotate a radians around z-zxis
	void rotateZ(float a)
	{
		mat4 rotate(
			cosf(a), -sinf(a), 0.0f, 0.0f,
			sinf(a), cosf(a), 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
		matrix = rotate * matrix;
	}
private:
	//init to be identity matrix
	mat4 matrix;
};

//generate the projection matrix
class projection
{
public:
	mat4& getMatrix()
	{
		return matrix;
	}

	//FOV: the vertical angle of field of the view, for example, 90 degree
	//width: the width of window, same as height
	//zNear: the closet z the camera can see
	//zFar: the farest z the camera can see
	projection(float FOV, float width, float height, float zNear,
		float zFar)
	{
		FOV = FOV * PI / 180.0f;
		float tanHalfFOV = tanf(FOV / 2.0f);
		float d = 1.0f / tanHalfFOV;
		//the ratio between the width and the height of the rectangular area which will be the target of projection
		float aspect_radio = width / height;
		float zRange = zNear - zFar;
		float A = (-zFar - zNear) / zRange;
		float B = 2.0f * zFar * zNear / zRange;

		matrix = mat4(
			d / aspect_radio, 0, 0, 0,
			0, d, 0, 0,
			0, 0, A, B,
			0, 0, 1, 0
		);
	}

private:
	mat4 matrix;
};

//generate the camera matrix
class camera
{
public:
	camera()
	{
		target.z = 1.0f;
		head_up.y = 1.0f;
	}

	//set the position of the camera
	void setPos(float x, float y, float z)
	{
		pos.x = x;
		pos.y = y;
		pos.z = z;
	}

	void handleKeyBoard(unsigned char key)
	{
		switch (key)
		{
		case 'w':
		{
			pos = pos + target * speed;
			break;
		}
		case 's':
		{
			pos = pos - target * speed;
			break;
		}
		case 'a':
		{
			vec3 left = target.cross(head_up);
			left.normalize();
			left = left * speed;
			pos = pos + left;
			break;
		}
		case 'd':
		{
			vec3 left = target.cross(head_up);
			left.normalize();
			left = left * speed;
			pos = pos - left;
			break;
		}
		case GLUT_KEY_UP:
		{
			pos = pos + head_up;
			break;
		}
		case GLUT_KEY_DOWN:
		{
			pos = pos - head_up;
			break;
		}
		case '+':
		{
			speed += 0.1f;
			printf("speed up to %f\n", speed);
			break;
		}
		case '-':
		{
			if (speed > 0.1f)
			{
				speed -= 0.1f;
				printf("speed down to %f\n", speed);
				break;
			}
		}
		default:
			break;
		}
	}

	mat4 getMatrix()
	{
		//be aware that we are in the left hand coordinate!!!!
		vec3 N = target;
		N.normalize();
		vec3 U;
		U = head_up.cross(N);
		U.normalize();
		vec3 V = N.cross(U);

		mat4 ret(
			U.x, U.y, U.z, -pos.x,
			V.x, V.y, V.z, -pos.y,
			N.x, N.y, N.z, -pos.z,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		return ret;
	}

private:
	//the position of the camera
	vec3 pos;

	//the target the camera looking at
	vec3 target;

	//the orientation the camera head up
	vec3 head_up;

	//the speed of the camera movitation
	float speed = 0.2f;
};
