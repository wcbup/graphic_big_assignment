#pragma once

#include <glew.h>
#include <freeglut.h>
#include "my_utilis.h"

class grid
{
public:
	grid(GLint xPositionLoc, GLint xIsManSetColorLoc, GLint xMyColorLoc)
	{
		positionLoc = xPositionLoc;
		isManSetColorLoc = xIsManSetColorLoc;
		myColorLoc = xMyColorLoc;

		initVBO();
	}

	void render()
	{
		vec3 color(1.0f, 0.0f, 0.0f);
		glUniform1i(isManSetColorLoc, 1);
		glUniform3f(myColorLoc, color.x, color.y, color.z);
		//glBindBuffer(GL_ARRAY_BUFFER, VBOLineX[0]);
		//glEnableVertexAttribArray(positionLoc);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		//glDrawArrays(GL_LINES, 0, 2);
		//glDisableVertexAttribArray(positionLoc);
		for (int i = 0; i < 6; i++)
		{
			glBindBuffer(GL_ARRAY_BUFFER, VBOLineX[i]);
			glEnableVertexAttribArray(positionLoc);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			glDrawArrays(GL_LINES, 0, 2);
			glDisableVertexAttribArray(positionLoc);
		}
		for (int i = 0; i < 6; i++)
		{
			glBindBuffer(GL_ARRAY_BUFFER, VBOLineY[i]);
			glEnableVertexAttribArray(positionLoc);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 03 * sizeof(float), 0);
			glDrawArrays(GL_LINES, 0, 2);
			glDisableVertexAttribArray(positionLoc);
		}
		glUniform1i(isManSetColorLoc, 0);
	}
private:
	GLint positionLoc;
	GLuint VBOLineX[6];
	GLuint VBOLineY[6];

	//the uniform location
	GLint isManSetColorLoc;
	GLint myColorLoc;

	void initVBO()
	{
		//set the lines horizontally
		vec3 lines[2];
		lines[0] = vec3(0.0f, 5.0f, 0.0f);
		lines[1] = vec3(5.0f, 5.0f, 0.0f);
		for (int i = 0; i < 6; i++)
		{
			glGenBuffers(1, &VBOLineX[i]);
			glBindBuffer(GL_ARRAY_BUFFER, VBOLineX[i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lines), lines, GL_STATIC_DRAW);
			lines[0].y--;
			lines[1].y--;
		}

		//set the lines vertically
		lines[0] = vec3(0.0f, 5.0f, 0.0f);
		lines[1] = vec3(0.0f, 0.0f, 0.0f);
		for (int i = 0; i < 6; i++)
		{
			glGenBuffers(1, &VBOLineY[i]);
			glBindBuffer(GL_ARRAY_BUFFER, VBOLineY[i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lines), lines, GL_STATIC_DRAW);
			lines[0].x++;
			lines[1].x++;
		}
	}
};
