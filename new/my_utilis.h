#pragma once

#include <string>
#include <fstream>
#include <stdio.h>
#include <math.h>
#include <freeglut.h>
#include <assimp/Importer.hpp> // loading model
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <stb_image.h> //loading texture

#define PI 3.14159265358979323846
#define ToRadian(x) (float)(((x) * PI / 180.0f))
#define ToDegree(x) (float)(((x) * 180.0f / PI))
#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals |  aiProcess_JoinIdenticalVertices )

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

class mat3
{
public:
	float m[3][3];

	//init to be indentity matrix
	mat3()
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
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

	//init from the left top corner;
	mat3(const mat4& a)
	{
		m[0][0] = a.m[0][0]; m[0][1] = a.m[0][1]; m[0][2] = a.m[0][2];
		m[1][0] = a.m[1][0]; m[1][1] = a.m[1][1]; m[1][2] = a.m[1][2];
		m[2][0] = a.m[2][0]; m[2][1] = a.m[2][1]; m[2][2] = a.m[2][2];
	}

	vec3 operator*(const vec3& v)
	{
		vec3 r;

		r.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z;
		r.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z;
		r.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z;

		return r;
	}

	mat3 getTranspose()
	{
		mat3 n;

		for (unsigned int i = 0; i < 3; i++) {
			for (unsigned int j = 0; j < 3; j++) {
				n.m[i][j] = m[j][i];
			}
		}

		return n;
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

struct vec2
{
	float x;
	float y;

	vec2()
	{
		x = y = 0;
	}
	vec2(float _x, float _y)
	{
		x = _x;
		y = _y;
	}
};

//use for rotate the camera
struct quaternion
{
	float x;
	float y;
	float z;
	float w;
	
	quaternion(float angle, const vec3& v)
	{
		float halfAngleInRadions = ToRadian(angle / 2);
		float sineHalfAngle = sinf(halfAngleInRadions);
		float cosHalfAngle = cosf(halfAngleInRadions);

		x = v.x * sineHalfAngle;
		y = v.y * sineHalfAngle;
		z = v.z * sineHalfAngle;
		w = cosHalfAngle;
	}

	quaternion(float _x, float _y, float _z, float _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	void normalize()
	{
		float length = sqrtf(x * x + y * y + z * z + w * w);

		x /= length;
		y /= length;
		z /= length;
		w /= length;
	}

	quaternion conjugate()
	{
		quaternion ret(-x, -y, -z, w);
		return ret;
	}
};

quaternion operator*(const quaternion& q, const vec3& v)
{
	float w = -(q.x * v.x) - (q.y * v.y) - (q.z * v.z);
	float x = (q.w * v.x) + (q.y * v.z) - (q.z * v.y);
	float y = (q.w * v.y) + (q.z * v.x) - (q.x * v.z);
	float z = (q.w * v.z) + (q.x * v.y) - (q.y * v.x);

	quaternion ret(x, y, z, w);

	return ret;
}


quaternion operator*(const quaternion& l, const quaternion& r)
{
	float w = (l.w * r.w) - (l.x * r.x) - (l.y * r.y) - (l.z * r.z);
	float x = (l.x * r.w) + (l.w * r.x) + (l.y * r.z) - (l.z * r.y);
	float y = (l.y * r.w) + (l.w * r.y) + (l.z * r.x) - (l.x * r.z);
	float z = (l.z * r.w) + (l.w * r.z) + (l.x * r.y) - (l.y * r.x);

	quaternion ret(x, y, z, w);

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

	//resize
	void scale(float a)
	{
		matrix.m[0][0] *= a;
		matrix.m[1][1] *= a;
		matrix.m[2][2] *= a;
	}

	//transplate (x, y, z)
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

	//reset the matrix
	void reset()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (i == j)
				{
					matrix.m[i][j] = 1;
				}
				else
				{
					matrix.m[i][j] = 0;
				}
			}
		}
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
		m_FOV = FOV;
		m_width = width;
		m_height = height;
		m_zNear = zNear;
		m_zFar = zFar;

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

	//reduce the FOV
	void zoomIn()
	{
		if (m_FOV <= 2.0f)
		{
			return;//minimun value is 5.0f
		}
		(*this) = projection(m_FOV - 1, m_width, m_height, m_zNear, m_zFar);
	}

	//increase the FOV
	void zoomOut()
	{
		if (m_FOV >= 170.0f)
		{
			return;//maximun value is 170.0f		}
		}
		(*this) = projection(m_FOV + 1, m_width, m_height, m_zNear, m_zFar);
	}

private:
	mat4 matrix;
	float m_FOV;
	float m_width;
	float m_height;
	float m_zNear;
	float m_zFar;
};


class texture
{
public:
	texture(const string& _fileName)
	{ 
		fileName = _fileName;
	}

	void load()
	{
		stbi_set_flip_vertically_on_load(1);
		int width = 0, height = 0, bit_per_pixel = 0;
		unsigned char* image_data = stbi_load(fileName.c_str(),
			&width, &height, &bit_per_pixel, 0);
		if (!image_data)
		{
			printf("can't load texture fromm '%s': '%s'\n",
				fileName.c_str(), stbi_failure_reason());
			exit(1);
		}

		glGenTextures(1, &textureObj);
		glBindTexture(GL_TEXTURE_2D, textureObj);
		switch (bit_per_pixel) {
		case 1:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, image_data);
			break;

		case 3:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
			break;

		case 4:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
			break;

		default:
			printf("didn't support texture of bit per pixel: %d\n",
				bit_per_pixel);
			exit(1);
		}

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//unbind the texture
		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(image_data);
	}

	void bind(GLenum textureUnit)
	{
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_2D, textureObj);
	}
private:
	string fileName;
	GLuint textureObj;
};

class material
{
public:
	vec3 ambientColor = vec3(0.0f, 0.0f, 0.0f);
	vec3 diffuseColor = vec3(0.0f, 0.0f, 0.0f);

	texture* pDiffuse = NULL; //the base texture
};

struct materialLoc
{
	GLuint ambientColor;
	GLuint diffuseColor;
};

struct dirLightLoc
{
	GLuint color;
	GLuint ambientIntensity;
	GLuint direction;
	GLuint diffuseIntensity;
};

class baseLight
{
public:
	vec3 color;
	float ambientIntensity;

	baseLight(float _ambientIntensity)
	{
		color = vec3(1.0f, 1.0f, 1.0f);
		ambientIntensity = _ambientIntensity;
	}

	baseLight(float _ambientIntensity, const vec3& _color)
	{
		ambientIntensity = _ambientIntensity;
		color = _color;
	}
};

class dirctionalLight : public baseLight
{
public:
	//the direction of light in world coordinate
	vec3 worldDirection;
	float diffuseIntensity;

	dirctionalLight(float _ambientIntensity,
		float _diffuseIntensity,
		const vec3& _worldDirection):baseLight(_ambientIntensity)
	{
		diffuseIntensity = _diffuseIntensity;
		worldDirection = _worldDirection;
	}

	dirctionalLight(float _ambientIntensity,
		const vec3& _color,
		float _diffuseIntensity,
		const vec3& _worldDirection) :
		baseLight(_ambientIntensity, _color)
	{
		diffuseIntensity = _diffuseIntensity;
		worldDirection = _worldDirection;
	}

	//calculate the light direction in local coordinate
	void calLocation(const mat4& world)
	{
		mat3 world3f(world);//init from top left corner
		
		//inverse local-to-world transformation
		mat3 worldToLocal = world3f.getTranspose();
		
		vec3 localDirection = worldToLocal * worldDirection;
		localDirection.normalize();
		m_localdirection =  localDirection;
	}

	const vec3& getlocalDirection() const
	{
		return m_localdirection;
	}

private:
	vec3 m_localdirection = vec3(0.0f, 0.0f, 0.0f);
};


