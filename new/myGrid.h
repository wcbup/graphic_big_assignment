#pragma once

#include <glew.h>
#include <freeglut.h>
#include "my_utilis.h"

struct rectangle
{
	vec2 vertices[4];
};

class grid
{
public:
	bool isClick[5][5];//is the rectangle clicked

	grid(int xWindowWidth, int xWindowHeight)
	{
		windowWidth = xWindowWidth;
		windowHeight = xWindowHeight;

		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				isClick[i][j] = false;
			}
		}

		recs[0][0].vertices[0] = vec2(-1, 1);
		recs[0][0].vertices[1] = vec2(-1 + 0.4, 1);
		recs[0][0].vertices[2] = vec2(-1 + 0.4, 1 - 0.4);
		recs[0][0].vertices[3] = vec2(-1, 1 - 0.4);
		for (int i = 0; i < 5; i++)
		{
			for (int j = 1; j < 5; j++)
			{
				recs[i][j] = recs[i][j - 1];
				for (int k = 0; k < 4; k++)
				{
					recs[i][j].vertices[k].x += 0.4;
				}
			}
			if (i == 4)
			{
				break;
			}
			recs[i + 1][0] = recs[i][0];
			for (int k = 0; k < 4; k++)
			{
				recs[i + 1][0].vertices[k].y -= 0.4;
			}
		}
	}

	void render()
	{
		//draw lines
		glColor3f(1.0f, 1.0f, 1.0f);
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				glBegin(GL_LINES);
				glVertex2f(recs[i][j].vertices[0].x, recs[i][j].vertices[0].y);
				glVertex2f(recs[i][j].vertices[1].x, recs[i][j].vertices[1].y);
				glEnd();

				glBegin(GL_LINES);
				glVertex2f(recs[i][j].vertices[1].x, recs[i][j].vertices[1].y);
				glVertex2f(recs[i][j].vertices[2].x, recs[i][j].vertices[2].y);
				glEnd();

				glBegin(GL_LINES);
				glVertex2f(recs[i][j].vertices[2].x, recs[i][j].vertices[2].y);
				glVertex2f(recs[i][j].vertices[3].x, recs[i][j].vertices[3].y);
				glEnd();

				glBegin(GL_LINES);
				glVertex2f(recs[i][j].vertices[3].x, recs[i][j].vertices[3].y);
				glVertex2f(recs[i][j].vertices[0].x, recs[i][j].vertices[0].y);
				glEnd();
			}
		}

		//draw rectangles
		glColor3f(0.98f, 0.625f, 0.12f);
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				if (isClick[i][j])
				{
					glBegin(GL_POLYGON);
					glVertex2f(recs[i][j].vertices[0].x, recs[i][j].vertices[0].y);
					glVertex2f(recs[i][j].vertices[1].x, recs[i][j].vertices[1].y);
					glVertex2f(recs[i][j].vertices[2].x, recs[i][j].vertices[2].y);
					glVertex2f(recs[i][j].vertices[3].x, recs[i][j].vertices[3].y);
					glEnd();
				}
			}
		}
	}

	void handleClick(int mouseX, int mouseY)
	{
		//printf("mouse clicked x:%d, y:%d\n", mouseX, mouseY);
		int xIndex = 0, yIndex = 0;
		float recWidth = windowWidth / 5;
		float recHeight = windowHeight / 5;
		yIndex = mouseX / recWidth;
		xIndex = mouseY / recHeight;
		isClick[xIndex][yIndex] = !isClick[xIndex][yIndex];
		//printf("xIndex:%d, yIndex:%d\n", xIndex, yIndex);
	}
private:
	rectangle recs[5][5];
	int windowWidth;
	int windowHeight;

};
