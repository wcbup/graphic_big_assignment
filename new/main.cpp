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

GLint gPositionLocation;
GLint gTexCoordLocation;
GLuint gWorldLocation;
GLuint gSamplerLocation;

GLuint cubeVAO;
GLuint cubeVBO;
GLuint cubeIBO;

GLuint pyramidVAO;
GLuint pyramidVBO;
GLuint pyramidIBO;

camera myCamera(WINDOW_WIDTH, WINDOW_HEIGHT);
projection myProjection(45.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1, 100);

texture* pTexture = nullptr;

struct Vertex {
	vec3 pos;
	vec2 tex;

	Vertex() {}

	Vertex(const vec3& _pos, const vec2& _tex)
	{
		pos = _pos;

		tex = _tex;
	}
};

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
	myWorldTransform.transplate(-1, 0, 5);

	mat4 world;
	world = myProjection.getMatrix() * myCamera.getMatrix() * myWorldTransform.getMatrix();
	//bind the uniform variable
	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &world.m[0][0]);

	GLint currentVAO;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
	if (currentVAO == cubeVAO)
	{
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
	}

	glutPostRedisplay();

	glutSwapBuffers();
}

void createCubeVAO()
{
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);

	Vertex Vertices[8];

	vec2 t00 = vec2(0.0f, 0.0f);  // Bottom left
	vec2 t01 = vec2(0.0f, 1.0f);  // Top left
	vec2 t10 = vec2(1.0f, 0.0f);  // Bottom right
	vec2 t11 = vec2(1.0f, 1.0f);  // Top right

	Vertices[0] = Vertex(vec3(0.5f, 0.5f, 0.5f), t00);
	Vertices[1] = Vertex(vec3(-0.5f, 0.5f, -0.5f), t01);
	Vertices[2] = Vertex(vec3(-0.5f, 0.5f, 0.5f), t10);
	Vertices[3] = Vertex(vec3(0.5f, -0.5f, -0.5f), t11);
	Vertices[4] = Vertex(vec3(-0.5f, -0.5f, -0.5f), t00);
	Vertices[5] = Vertex(vec3(0.5f, 0.5f, -0.5f), t10);
	Vertices[6] = Vertex(vec3(0.5f, -0.5f, 0.5f), t01);
	Vertices[7] = Vertex(vec3(-0.5f, -0.5f, 0.5f), t11);

	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	// position
	glEnableVertexAttribArray(gPositionLocation);
	glVertexAttribPointer(gPositionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	// tex coords
	glEnableVertexAttribArray(gTexCoordLocation);
	glVertexAttribPointer(gTexCoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));

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

	glGenBuffers(1, &cubeIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	//unbind the vertex arry
	glBindVertexArray(0);
	glDisableVertexAttribArray(gPositionLocation);
	glDisableVertexAttribArray(gTexCoordLocation);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void createPyramidVAO()
{
	glGenVertexArrays(1, &pyramidVAO);
	glBindVertexArray(pyramidVAO);

	vec2 t00 = vec2(0.0f, 0.0f);
	vec2 t050 = vec2(0.5f, 0.0f);
	vec2 t10 = vec2(1.0f, 0.0f);
	vec2 t051 = vec2(0.5f, 1.0f);

	Vertex Vertices[4] = { Vertex(vec3(-1.0f, -1.0f, 0.5773f), t00),
						   Vertex(vec3(0.0f, -1.0f, -1.15475f), t050),
						   Vertex(vec3(1.0f, -1.0f, 0.5773f), t10),
						   Vertex(vec3(0.0f, 1.0f, 0.0f), t051) };

	glGenBuffers(1, &pyramidVBO);
	glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices),
		Vertices, GL_STATIC_DRAW);

	//position
	glEnableVertexAttribArray(gPositionLocation);
	glVertexAttribPointer(gPositionLocation, 3, GL_FLOAT,
		GL_FALSE, sizeof(Vertex), 0);
	
	//tex coords
	glEnableVertexAttribArray(gTexCoordLocation);
	glVertexAttribPointer(gTexCoordLocation, 2, GL_FLOAT,
		GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));

	unsigned int Indices[] = { 0, 3, 1,
							   1, 3, 2,
							   2, 3, 0,
							   0, 1, 2 };

	glGenBuffers(1, &pyramidIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramidIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	//unbind the vertex array object
	glBindVertexArray(0);
	glDisableVertexAttribArray(gPositionLocation);
	glDisableVertexAttribArray(gTexCoordLocation);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
	gTexCoordLocation = glGetAttribLocation(shaderProgram, "TexCoord");
	if (gTexCoordLocation == -1)
	{
		printf("Error getting attri location of 'TexCoord'\n");
		exit(1);
	}

	gWorldLocation = glGetUniformLocation(shaderProgram, "gWorld");
	if (gWorldLocation == -1)
	{
		printf("Error getting uniform location of 'gWorld'\n");
		exit(1);
	}

	gSamplerLocation = glGetUniformLocation(shaderProgram, "gSampler");
	if (gSamplerLocation == -1)
	{
		printf("Error getting uniform location of 'gSampler'\n");
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
	if (key == '1')
	{
		glBindVertexArray(cubeVAO);
	}
	else if (key == '2')
	{
		glBindVertexArray(pyramidVAO);
	}
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

	compileShader();

	createCubeVAO();
	createPyramidVAO();
	glBindVertexArray(cubeVAO);

	pTexture = new texture("../res/bricks.jpg");
	pTexture->load();	
	pTexture->bind(GL_TEXTURE0);
	glUniform1i(gSamplerLocation, 0);


	initGlut();

	glutMainLoop();

	return 0;
}
