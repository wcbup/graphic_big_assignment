#pragma once

#include <string>
#include <fstream>
#include <stdio.h>
#include <math.h>
#include <freeglut.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <stb_image.h>

#define PI 3.14159265358979323846
#define ToRadian(x) (float)(((x) * PI / 180.0f))
#define ToDegree(x) (float)(((x) * 180.0f / PI))

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
	camera(int window_width, int window_height)
	{
		windowWidth = window_width;
		windowHeight = window_height;
		target.z = 1.0f;
		head_up.y = 1.0f;

		init();
	}
	camera(int window_width, int window_height, const vec3& pos,
		const vec3& target, const vec3& up)
	{
		windowWidth = window_width;
		windowHeight = window_height;
		this->pos = pos;
		this->pos.normalize();
		this->target = target;
		this->target.normalize();
		this->head_up = up;
		this->head_up.normalize();

		init();
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
			//only move horizontally
			vec3 tmp = target;
			tmp.y = 0;
			pos = pos + tmp * speed;
			break;
		}
		case 's':
		{
			vec3 tmp = target;
			tmp.y = 0;
			pos = pos - tmp * speed;
			break;
		}
		case 'a':
		{
			vec3 left = target.cross(head_up);
			left.normalize();
			left = left * speed;
			left.y = 0;
			pos = pos + left;
			break;
		}
		case 'd':
		{
			vec3 left = target.cross(head_up);
			left.normalize();
			left = left * speed;
			left.y = 0;
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
		mat4 translate(
			1, 0, 0, -pos.x,
			0, 1, 0, -pos.y,
			0, 0, 1, -pos.z,
			0, 0, 0, 1
		);

		//be aware that we are in the left hand coordinate!!!!
		vec3 N = target;
		N.normalize();
		vec3 U;
		U = head_up.cross(N);
		U.normalize();
		vec3 V = N.cross(U);

		mat4 transform(
			U.x, U.y, U.z, -pos.x,
			V.x, V.y, V.z, -pos.y,
			N.x, N.y, N.z, -pos.z,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		return transform * translate;
	}

	void onMouse(int x, int y)
	{
		int deltaX = x - mouse_x;
		int deltaY = y - mouse_y;

		mouse_x = x;
		mouse_y = y;

		angle_horizon += (float)deltaX / 20.0f;
		angle_vertical += (float)deltaY / 20.0f;

		if (deltaX == 0)
		{
			if (x <= MARGIN)
			{
				onLeftEdge = true;
			}
			else if (x >= (windowWidth - MARGIN))
			{
				onRightEdge = true;
			}
		}
		else
		{
			onLeftEdge = false;
			onRightEdge = false;
		}

		if (deltaY == 0)
		{
			if (y <= MARGIN)
			{
				onUpperEdge = true;
			}
			else if (y >= windowHeight - MARGIN)
			{
				onLowerEdge = true;
			}
		}
		else
		{
			onUpperEdge = false;
			onLowerEdge = false;
		}

		update();
	}

	//update angle if the mouse is at the edge
	void updateAtEdge()
	{
		bool shouldUpdate = false;

		if (onLeftEdge)
		{
			angle_horizon -= EDGE_SPEED;
			shouldUpdate = true;
		}
		else if (onRightEdge)
		{
			angle_horizon += EDGE_SPEED;
			shouldUpdate = true;
		}
		else if (onUpperEdge)
		{
			if (angle_vertical > -90.0f)
			{
				angle_vertical -= EDGE_SPEED;
				shouldUpdate = true;
			}
		}
		else if (onLowerEdge)
		{
			if (angle_vertical < 90.0f)
			{
				angle_vertical += EDGE_SPEED;
				shouldUpdate = true;
			}
		}

		if (shouldUpdate)
		{
			update();
		}
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

	int windowWidth;
	int windowHeight;
	//the angle the camera turn horizontally or vertically
	float angle_horizon;
	float angle_vertical;

	//if the mouse is on the edge
	bool onUpperEdge;
	bool onLowerEdge;
	bool onLeftEdge;
	bool onRightEdge;

	//the position of the mouse
	int mouse_x;
	int mouse_y;

	//the margin trigger the mouse to move
	int MARGIN = 80;
	//the speed of the camera rotate when the mouse is at edge
	int EDGE_SPEED = 1.0f;

	void init()
	{
		//set the rotate degree according to the target
		vec3 horizontal_target(target.x, 0, target.z);
		horizontal_target.normalize();
		float angle = ToDegree(asinf(abs(horizontal_target.z)));
		if (horizontal_target.z >= 0.0f)
		{
			if (horizontal_target.x >= 0.0f)
			{
				angle_horizon = 360 - angle;
			}
			else
			{
				angle_horizon = 180 + angle;
			}
		}
		else
		{
			if (horizontal_target.x >= 0)
			{
				angle_horizon = angle;
			}
			else
			{
				angle_horizon = 180.0f - angle;
			}
		}

		angle_vertical = -ToDegree(asinf(target.y));

		onUpperEdge = false;
		onLowerEdge = false;
		onLeftEdge = false;
		onRightEdge = false;
		mouse_x = windowWidth / 2;
		mouse_y = windowHeight / 2;
	}

	//roate the view using the quaternion
	void rotateWithQuaternion(vec3& view, float angle, const vec3& axis)
	{
		quaternion rotationQ(angle, axis);
		quaternion conjugateQ = rotationQ.conjugate();
		quaternion result = rotationQ * view * conjugateQ;

		view.x = result.x;
		view.y = result.y;
		view.z = result.z;
	}

	void update()
	{
		vec3 yAxis(0, 1, 0);

		//rotate horizontally
		vec3 view(1, 0, 0);
		rotateWithQuaternion(view, angle_horizon, yAxis);
		view.normalize();

		//rotate vertically
		vec3 U = yAxis.cross(view);
		U.normalize();
		rotateWithQuaternion(view, angle_vertical, U);

		target = view;
		target.normalize();

		head_up = target.cross(U);
		head_up.normalize();
	}
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
