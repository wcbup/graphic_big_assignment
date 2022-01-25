#include <glew.h>
#include <freeglut.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <iostream>
#include "my_utilis.h"
#include "myCamera.h"
#include "modelLoader.h"
#include "myShader.h"
#include "myGrid.h"
#include "basicUnit.h"

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720

using namespace std;

camera myCamera(WINDOW_WIDTH, WINDOW_HEIGHT);
projection myProjection(45.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1, 1000);

modelLoader* myMesh = NULL;
shader* myShader = NULL;
grid* myGrid = NULL;
basicUnit* myUnit = NULL;

bool isInEditMode = false;

//dirctional light come from left
dirctionalLight dirLight(0.2f,
	1.0f,
	vec3(1.0f, -1.0f, -1.0f));

void renderInNormalMode()
{
	myShader->enable();
	myCamera.updateAtEdge();

	static float angleInRadians = 0;
	static float delta = 0.005f;
	angleInRadians += delta;

	//transform from local coordinate to world coordinate
	worldTransform myWorldTransform;
	myWorldTransform.scale(1);
	//myWorldTransform.rotateY(angleInRadians);
	myWorldTransform.transplate(0, -0.5, 5);

	mat4 WVP;
	WVP = myProjection.getMatrix() * myCamera.getMatrix() * myWorldTransform.getMatrix();

	worldTransform transform2;
	transform2.transplate(sinf(angleInRadians), cosf(angleInRadians), 0);
	WVP = WVP * transform2.getMatrix();

	myShader->setWVP(WVP);

	dirLight.calLocation(WVP);
	myShader->setDirectionalLight(dirLight);

	myMesh->render();

	myUnit->render();
}

void renderInEditMode()
{
	glUseProgram(0);
	
	myGrid->render();
}

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (isInEditMode)
	{
		renderInEditMode();
	}
	else
	{
		renderInNormalMode();
	}
	glutPostRedisplay();

	glutSwapBuffers();
}

void keyboard(unsigned char key, int mouse_x, int mouse_y)
{
	if (key == 'i')
	{
		myProjection.zoomIn();
	}
	else if (key == 'o')
	{
		myProjection.zoomOut();
	}
	else if (key == 'e')
	{
		isInEditMode = !isInEditMode;
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

void myMouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
	{
		myGrid->handleClick(x, y);
	}
}

void initGlut()
{
	glutDisplayFunc(renderScene);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeyboard);
	glutPassiveMotionFunc(passiveMouse);
	glutMouseFunc(myMouse);
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
		myShader->materialLocation,
		myShader->hasTexLoc,
		myShader->isManualSetColorLoc
	);
	myMesh->loadMesh("../res/wine_barrel_01_4k.blend/wine_barrel_01_4k.obj");

	myGrid = new grid(WINDOW_WIDTH, WINDOW_HEIGHT);
	myUnit = new basicUnit(myShader);

	initGlut();

	glutMainLoop();

	return 0;
}
