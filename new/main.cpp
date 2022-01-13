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
GLuint IBO;
GLint gPositionLocation;
GLint gColorLocation;
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
	mat4 transplate(1, 0, 0, 0,
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
	//mat4 world = scale * transplate * rotate;
	//mat4 world = scale * rotate * transplate;
	//mat4 world = rotate * transplate * scale;
	mat4 world = transplate;

	//bind the uniform variable
	glUniformMatrix4fv(gTranslationLocation, 1, GL_TRUE, &world[0][0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	//position
	glEnableVertexAttribArray(gPositionLocation);
	glVertexAttribPointer(gPositionLocation, 3, GL_FLOAT, GL_FALSE,
		6*sizeof(float), 0);

	//color
	glEnableVertexAttribArray(gColorLocation);
	glVertexAttribPointer(gColorLocation, 3, GL_FLOAT, GL_FALSE,
		6*sizeof(float), (void *)(sizeof(float)));

	glDrawElements(GL_TRIANGLES, 54, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(gPositionLocation);
	glDisableVertexAttribArray(gColorLocation);

	glutPostRedisplay();

	glutSwapBuffers();
}

struct Vertex {
	vec3 pos;
	vec3 color;

	Vertex() {}

	Vertex(float x, float y)
	{
		pos = vec3(x, y, 0.0f);

		float red = (float)rand() / (float)RAND_MAX;
		float green = (float)rand() / (float)RAND_MAX;
		float blue = (float)rand() / (float)RAND_MAX;
		color = vec3(red, green, blue);
	}
};

void createVertexBuffer()
{
	Vertex Vertices[19];

	// Center
	Vertices[0] = Vertex(0.0f, 0.0);

	// Top row
	Vertices[1] = Vertex(-1.0f, 1.0f);
	Vertices[2] = Vertex(-0.75f, 1.0f);
	Vertices[3] = Vertex(-0.50f, 1.0f);
	Vertices[4] = Vertex(-0.25f, 1.0f);
	Vertices[5] = Vertex(-0.0f, 1.0f);
	Vertices[6] = Vertex(0.25f, 1.0f);
	Vertices[7] = Vertex(0.50f, 1.0f);
	Vertices[8] = Vertex(0.75f, 1.0f);
	Vertices[9] = Vertex(1.0f, 1.0f);

	// Bottom row
	Vertices[10] = Vertex(-1.0f, -1.0f);
	Vertices[11] = Vertex(-0.75f, -1.0f);
	Vertices[12] = Vertex(-0.50f, -1.0f);
	Vertices[13] = Vertex(-0.25f, -1.0f);
	Vertices[14] = Vertex(-0.0f, -1.0f);
	Vertices[15] = Vertex(0.25f, -1.0f);
	Vertices[16] = Vertex(0.50f, -1.0f);
	Vertices[17] = Vertex(0.75f, -1.0f);
	Vertices[18] = Vertex(1.0f, -1.0f);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices,
		GL_STATIC_DRAW);
}

void createIndexBuffer()
{
	unsigned int Indices[] = { // Top triangles
							   0, 2, 1,
							   0, 3, 2,
							   0, 4, 3,
							   0, 5, 4,
							   0, 6, 5,
							   0, 7, 6,
							   0, 8, 7,
							   0, 9, 8,

							   // Bottom triangles
							   0, 10, 11,
							   0, 11, 12,
							   0, 12, 13,
							   0, 13, 14,
							   0, 14, 15,
							   0, 15, 16,
							   0, 16, 17,
							   0, 17, 18,

							   // Left triangle
							   0, 1, 10,

							   // Right triangle
							   0, 18, 9 };

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices),
		Indices, GL_STATIC_DRAW);
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
	gColorLocation = glGetAttribLocation(shaderProgram, "inColor");
	//printf("gPositionLocation is %d\n", gPositionLocation);
	if (gColorLocation == -1)
	{
		printf("Error getting attri location of 'inColor'\n");
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
	srand(GetCurrentProcessId());

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

	createVertexBuffer();
	createIndexBuffer();

	compileShader();

	glutDisplayFunc(renderScene);

	glutMainLoop();

	return 0;
}
