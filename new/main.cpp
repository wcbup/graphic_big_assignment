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

camera myCamera(WINDOW_WIDTH, WINDOW_HEIGHT);
projection myProjection(45.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1, 100);

modelLoader* myMesh = NULL;
shader* myShader = NULL;

//dirctional light come from left
dirctionalLight dirLight(0.1f,
	1.0f,
	vec3(1.0f, 0.0f, 0.0f));

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

	mat4 WVP;
	WVP = myProjection.getMatrix() * myCamera.getMatrix() * myWorldTransform.getMatrix();

	myShader->setWVP(WVP);

	dirLight.calLocation(WVP);
	myShader->setDirectionalLight(dirLight);

	myMesh->render();

	glutPostRedisplay();

	glutSwapBuffers();
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
		myShader->normalLocation,
		myShader->materialLocation
	);
	myMesh->loadMesh("../res/formula 1/Formula 1 mesh.obj");

	initGlut();

	glutMainLoop();

	return 0;
}
