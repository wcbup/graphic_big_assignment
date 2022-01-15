#include <glew.h>
#include <freeglut.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <iostream>
#include "my_utilis.h"

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720

using namespace std;

GLuint VBO;
GLuint IBO;
GLint gPositionLocation;
GLint gColorLocation;
GLuint gWorldLocation;

camera myCamera(WINDOW_WIDTH, WINDOW_HEIGHT);
projection myProjection(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1, 100);

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT);

	myCamera.updateAtEdge();

	static float angleInRadians = 0;
	static float delta = 0.005f;
	angleInRadians += delta;

	//transform from local coordinate to world coordinate
	worldTransform myWorldTransform;
	myWorldTransform.scale(1);
	myWorldTransform.rotateY(angleInRadians);
	myWorldTransform.transplate(-1, 1, 10);

	mat4 world;
	world = myProjection.getMatrix() * myCamera.getMatrix() * myWorldTransform.getMatrix();

	//bind the uniform variable
	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &world.m[0][0]);

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

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(gPositionLocation);
	glDisableVertexAttribArray(gColorLocation);

	glutPostRedisplay();

	glutSwapBuffers();
}

struct Vertex {
	vec3 pos;
	vec3 color;

	Vertex() {}

	Vertex(float x, float y, float z)
	{
		pos = vec3(x, y, z);

		float red = (float)rand() / (float)RAND_MAX;
		float green = (float)rand() / (float)RAND_MAX;
		float blue = (float)rand() / (float)RAND_MAX;
		color = vec3(red, green, blue);
	}
};

void createVertexBuffer()
{
	Vertex Vertices[8];

	Vertices[0] = Vertex(0.5f, 0.5f, 0.5f);
	Vertices[1] = Vertex(-0.5f, 0.5f, -0.5f);
	Vertices[2] = Vertex(-0.5f, 0.5f, 0.5f);
	Vertices[3] = Vertex(0.5f, -0.5f, -0.5f);
	Vertices[4] = Vertex(-0.5f, -0.5f, -0.5f);
	Vertices[5] = Vertex(0.5f, 0.5f, -0.5f);
	Vertices[6] = Vertex(0.5f, -0.5f, 0.5f);
	Vertices[7] = Vertex(-0.5f, -0.5f, 0.5f);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices,
		GL_STATIC_DRAW);
}

void createIndexBuffer()
{
	unsigned int Indices[] = {
							  0, 1, 2,
							  1, 3, 4,
							  5, 6, 3,
							  7, 3, 6,
							  2, 4, 7,
							  0, 7, 6,
							  0, 5, 1,
							  1, 5, 3,
							  5, 0, 6,
							  7, 4, 3,
							  2, 1, 4,
							  0, 2, 7
	};

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
	if (gPositionLocation == -1)
	{
		printf("Error getting attri location of 'Postion'\n");
		exit(1);
	}
	gColorLocation = glGetAttribLocation(shaderProgram, "inColor");
	if (gColorLocation == -1)
	{
		printf("Error getting attri location of 'inColor'\n");
		exit(1);
	}

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

void keyboard(unsigned char key, int mouse_x, int mouse_y)
{
	myCamera.handleKeyBoard(key);
}

void specialKeyboard(int key, int mouse_x, int mouse_y)
{
	myCamera.handleKeyBoard(key);
}

void passiveMouse(int x, int y)
{
	myCamera.onMouse(x, y);
}

void initGlut()
{
	glutDisplayFunc(renderScene);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeyboard);
	glutPassiveMotionFunc(passiveMouse);
}

int main(int argc, char** argv)
{
	srand(GetCurrentProcessId());

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	glutInitWindowPosition(0, 0);
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

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);

	createVertexBuffer();
	createIndexBuffer();

	compileShader();

	initGlut();

	glutMainLoop();

	return 0;
}
