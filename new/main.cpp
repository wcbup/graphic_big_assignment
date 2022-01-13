#include <glew.h>
#include <freeglut.h>
#include <stdio.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <string>
#include <math.h>
#include "my_utilis.h"

using namespace std;
using namespace glm;

GLuint VBO;
GLint gPositionLocation;
GLuint gScaleLocation;
GLuint gTranslationLocation;
GLuint gRotationLocation;
GLuint gWorldLocation;

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT);

	static float angleInRadians = 0;
	static float delta = 0.005f;
	angleInRadians += delta;
	//if ((angleInRadians >= 1.5708f) || (angleInRadians <= -1.5708f))
	//{
	//	delta *= -1;
	//}
	mat4 transplate(1, 0, 0, 1,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
	mat4 rotate(cosf(angleInRadians), -sinf(angleInRadians), 0, 0,
		sinf(angleInRadians), cosf(angleInRadians), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
	mat4 scale(0.2, 0, 0, 0,
		0, 0.2, 0, 0,
		0, 0, 0.2, 0,
		0, 0, 0, 1);
	mat4 world = scale * transplate * rotate;
	//mat4 world = scale * rotate * transplate;
	//mat4 world = rotate * transplate * scale;

	//bind the uniform variable
	glUniformMatrix4fv(gTranslationLocation, 1, GL_TRUE, &world[0][0]);

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

	//gScaleLocation = glGetUniformLocation(shaderProgram, "gScale");
	//if (gScaleLocation == -1)
	//{
	//	printf("Error getting uniform location of 'gScale'\n");
	//	exit(1);
	//}

	//gTranslationLocation = glGetUniformLocation(shaderProgram, "gTranslation");
	//if (gTranslationLocation = == -1)
	//{
	//	printf("Error getting uniform location of 'gTranslation'\n");
	//	exit(1);
	//}

	//gRotationLocation = glGetUniformLocation(shaderProgram, "gRotation");
	//if (gRotationLocation == -1)
	//{
	//	printf("Error getting uniform location of 'gRotatioin'\n");
	//	exit(1);
	//}

	gWorldLocation = glGetUniformLocation(shaderProgram, "gWorld");
	if (gWorldLocation == -1)
	{
		printf("Error getting uniform location of 'gWorld'\n");
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

	int width = 1000;
	int height = 1000;
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
