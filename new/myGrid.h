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
	grid()
	{
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
	}
private:
	rectangle recs[5][5];

};
