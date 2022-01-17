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

//GLint gPositionLocation;
//GLint gTexCoordLocation;
//GLuint gWorldLocation;
//GLuint gSamplerLocation;

camera myCamera(WINDOW_WIDTH, WINDOW_HEIGHT);
projection myProjection(45.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1, 100);

modelLoader* myMesh = NULL;
shader* myShader = NULL;

//set the ambient intensity
baseLight myLight(1.0f);
//baseLight myLight(1.0f, vec3(1.0f, 0.0f, 0.0f));

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	myCamera.updateAtEdge();

	static float angleInRadians = 0;
	static float delta = 0.005f;
	angleInRadians += delta;

	//transform from local coordinate to world coordinate
	worldTransform myWorldTransform;
	myWorldTransform.scale(0.05);
	myWorldTransform.rotateY(angleInRadians);
	myWorldTransform.transplate(-1, 0, 25);

	//mat4 WVP;
	//WVP = myProjection.getMatrix() * myCamera.getMatrix() * myWorldTransform.getMatrix();
	////bind the uniform variable
	//glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &WVP.m[0][0]);

	mat4 WVP;
	WVP = myProjection.getMatrix() * myCamera.getMatrix() * myWorldTransform.getMatrix();
	
	myShader->setWVP(WVP);
	myShader->setLight(myLight);

	myMesh->render();

	glutPostRedisplay();

	glutSwapBuffers();
}

//void addShader(GLuint shaderProgram, const char* pShaderText, GLenum shaderType)
//{
//	GLuint shaderObj = glCreateShader(shaderType);
//
//	if (shaderObj == 0)
//	{
//		printf("Error creating shader type %d\n", shaderType);
//	}
//	
//	const GLchar* p[1];
//	p[0] = pShaderText;
//
//	GLint lengh;
//	lengh = (GLint)strlen(pShaderText);
//
//	glShaderSource(shaderObj, 1, p, &lengh);
//
//	glCompileShader(shaderObj);
//
//	GLint success;
//	glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &success);
//
//	if (!success)
//	{
//		GLchar infoLog[1024];
//		glGetShaderInfoLog(shaderObj, 1024, NULL, infoLog);
//		printf("Error compling shader\n");
//		exit(1);
//	}
//
//	glAttachShader(shaderProgram, shaderObj);
//}
//
//const char* pVSFileName = "shader.vs";
//const char* pFSFileName = "shader.fs";
//
//void compileShader()
//{
//	GLuint shaderProgram = glCreateProgram();
//
//	if (shaderProgram == 0)
//	{
//		printf("Error creating shader program\n");
//		exit(1);
//	}
//
//	string vs, fs;
//
//	if (!readFile(pVSFileName, vs))
//	{
//		exit(1);
//	}
//	
//	addShader(shaderProgram, vs.c_str(), GL_VERTEX_SHADER);
//
//	if (!readFile(pFSFileName, fs))
//	{
//		exit(1);
//	}
//
//	addShader(shaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);
//
//	GLint succuss = 0;
//	GLchar errorLog[1024] = { 0 };
//
//	glLinkProgram(shaderProgram);
//	
//	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &succuss);
//	if (succuss == 0)
//	{
//		glGetProgramInfoLog(shaderProgram, sizeof(errorLog),
//			NULL, errorLog);
//		printf("Error linking shader program: %s\n", errorLog);
//		exit(1);
//	}
//
//	//get the location of variable in shader script
//	gPositionLocation = glGetAttribLocation(shaderProgram, "Position");
//	if (gPositionLocation == -1)
//	{
//		printf("Error getting attri location of 'Postion'\n");
//		exit(1);
//	}
//	gTexCoordLocation = glGetAttribLocation(shaderProgram, "TexCoord");
//	if (gTexCoordLocation == -1)
//	{
//		printf("Error getting attri location of 'TexCoord'\n");
//		exit(1);
//	}
//
//	gWorldLocation = glGetUniformLocation(shaderProgram, "gWVP");
//	if (gWorldLocation == -1)
//	{
//		printf("Error getting uniform location of 'gWVP'\n");
//		exit(1);
//	}
//
//	gSamplerLocation = glGetUniformLocation(shaderProgram, "gSampler");
//	if (gSamplerLocation == -1)
//	{
//		printf("Error getting uniform location of 'gSampler'\n");
//		exit(1);
//	}
//
//	glValidateProgram(shaderProgram);
//	glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &succuss);
//	if (succuss == 0)
//	{
//		glGetProgramInfoLog(shaderProgram, sizeof(errorLog),
//			NULL, errorLog);
//		exit(1);
//	}
//
//	glUseProgram(shaderProgram);
//}

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
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	glutInitWindowPosition(0, 0);
	int win = glutCreateWindow("example");

	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		printf("Error: %s\n", glewGetErrorString(res));
		return 1;
	}

	GLclampf red = 0, green = 0, blue = 0, alpha = 0;
	glClearColor(red, green, blue, alpha);

	//remember to enable all below
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	//compileShader();

	myShader = new shader();
	myShader->init();
	myShader->enable();
	myShader->setTextureUnit(GL_TEXTURE0);

	//pass in the uniform location in the shader
	myMesh = new modelLoader(
		myShader->positionLoc,
		myShader->texCoorLocation,
		myShader->samplerLoc,
		myShader->materialAmbientColorLoc
	);
	myMesh->loadMesh("../res/formula 1/Formula 1 mesh.obj");

	initGlut();

	glutMainLoop();

	return 0;
}
