#include <freeglut.h>
#include <stdio.h>

void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT);
	static GLclampf r = 0;
	r += 1.0 / 256.0;
	if (r >= 1)
	{
		r = 0;
	}
	glClearColor(r, r, r, 1);
	glutPostRedisplay();
	glutSwapBuffers();
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
	GLclampf red = 0, green = 0, blue = 0, alpha = 0;
	glClearColor(red, green, blue, alpha);


	glutDisplayFunc(RenderScene);

	glutMainLoop();

	return 0;
}
