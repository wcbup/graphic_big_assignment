#include <glew.h>
#include <freeglut.h>
#include <stdio.h>
#include <glm/vec3.hpp>
#include <string>
#include "my_utilis.h"

using namespace std;
using namespace glm;

GLuint VBO;
GLint gPositionLocation;
GLuint gScaleLocation;

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT);

	static float scale = 0;
	static float delta = 0.001f;
	scale += delta;
	if ((scale >= 1) || (scale <= -1))
	{
		delta *= -1;
	}

	//bind the uniform variable
	glUniform1f(gScaleLocation, scale);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableVertexAttribArray(gPositionLocation);
	glVertexAttribPointer(gPositionLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(gPositionLocation);

	glutPostRedisplay();

	glutSwapBuffers();
}

void createBuffer()
{
	glm::vec3 vertices[3];
	vertices[0] = glm::vec3(-1, -1, 0);
	vertices[1] = glm::vec3(1, -1, 0);
	vertices[2] = glm::vec3(0, 1, 0);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
		GL_STATIC_DRAW);
}

void addShader(GLuint shaderProgram, const char* pShaderText, GLenum shaderType)
{
	GLuint shaderObj = glCreateShader(shaderType);

	if (shaderObj == 0)
	{
		printf("Error creating shader type %d\n", shaderType);
	}
	
	const GLchar* p[1];
	p[0] = pShaderText;

	GLint lengh;
	lengh = (GLint)strlen(pShaderText);

	glShaderSource(shaderObj, 1, p, &lengh);

	glCompileShader(shaderObj);

	GLint success;
	glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		GLchar infoLog[1024];
		glGetShaderInfoLog(shaderObj, 1024, NULL, infoLog);
		printf("Error compling shader\n");
		exit(1);
	}

	glAttachShader(shaderProgram, shaderObj);
}

const char* pVSFileName = "shader.vs";
const char* pFSFileName = "shader.fs";

void compileShader()
{
	GLuint shaderProgram = glCreateProgram();

	if (shaderProgram == 0)
	{
		printf("Error creating shader program\n");
		exit(1);
	}

	string vs, fs;

	if (!readFile(pVSFileName, vs))
	{
		exit(1);
	}
	
	addShader(shaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	if (!readFile(pFSFileName, fs))
	{
		exit(1);
	}

	addShader(shaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint succuss = 0;
	GLchar errorLog[1024] = { 0 };

	glLinkProgram(shaderProgram);
	
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &succuss);
	if (succuss == 0)
	{
		glGetProgramInfoLog(shaderProgram, sizeof(errorLog),
			NULL, errorLog);
		printf("Error linking shader program: %s\n", errorLog);
		exit(1);
	}

	//get the location of variable in shader script
	gPositionLocation = glGetAttribLocation(shaderProgram, "Position");
	//printf("gPositionLocation is %d\n", gPositionLocation);
	if (gPositionLocation == -1)
	{
		printf("Error getting attri location of 'Postion'\n");
		exit(1);
	}

	gScaleLocation = glGetUniformLocation(shaderProgram, "gScale");
	if (gScaleLocation == -1)
	{
		printf("Error getting uniform location of 'gScale'\n");
		exit(1);
	}


	glValidateProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &succuss);
	if (succuss == 0)
	{
		glGetProgramInfoLog(shaderProgram, sizeof(errorLog),
			NULL, errorLog);
		exit(1);
	}

	glUseProgram(shaderProgram);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	int width = 1280;
	int height = 720;
	glutInitWindowSize(width, height);

	int x = 200;
	int y = 100;
	glutInitWindowPosition(x, y);
	int win = glutCreateWindow("Tutorial 1");
	printf("window id is %d\n", win);

	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		printf("Error: %s\n", glewGetErrorString(res));
		return 1;
	}

	GLclampf red = 0, green = 0, blue = 0, alpha = 0;
	glClearColor(red, green, blue, alpha);

	createBuffer();

	compileShader();

	glutDisplayFunc(renderScene);

	glutMainLoop();

	return 0;
}
