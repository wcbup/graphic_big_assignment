#pragma once

#include "my_utilis.h"

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
