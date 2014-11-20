#include <iostream>
#include <gl/glut.h>
#include <vector>

#include "vobject.h"
#include "point.h"

using namespace std;

int window_width = 600;
int window_height = 600;

int Action = 1;

Point pressed(0, 0);
Point release(0, 0);

vector<VObject*> vobjs;

Line guide_line(pressed, release);
Triangle guide_triangle(pressed, release);
Rectangle guide_rectangle(pressed, release);

void displayText(float x, float y, const char *string)
{
	int j = strlen(string);

	glRasterPos2f(x, y);
	for (int i = 0; i < j; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
	}
}

void keyboardDown(unsigned char key, int x, int y)
{
	printf("key pressed : %d\n", (int)key);

	switch (key)
	{
	case 'q':
	case 227: // 한글 q
		exit(0);
		break;
	}
}

void DoMenu(int value)
{
	Action = value;
	// glutPostRedisplay();
}


void DoDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);

	int size = vobjs.size();
	for (int i = 0; i < size; i++)
	{
		vobjs[i]->draw();
	}

	guide_line.draw();
	guide_triangle.draw();
	guide_rectangle.draw();

	glutSwapBuffers();
}

// 마우스 클릭 이벤트 처리
void mouseClick(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) // pressed
	{
		if (Action == 1) // 점
		{
			// 마우스 클릭을 하면 Dots 를 새로 생성
			Dots* dots = new Dots(VColor::random());
			dots->add_point(x, y);
			vobjs.push_back((VObject*)dots);
			glutPostRedisplay();
		}
		else if (Action == 2) // 선
		{
			pressed.set(x, y);
			guide_line.set_start_point(pressed);
			guide_line.set_visible(true);
		}
		else if (Action == 3) // 삼각형
		{
			pressed.set(x, y);
			guide_triangle.set_start_point(pressed);
			guide_triangle.set_visible(true);
		}
		else if (Action == 4) // 사각형
		{
			pressed.set(x, y);
			guide_rectangle.set_start_point(pressed);
			guide_rectangle.set_visible(true);
		}
	}
	else // released
	{
		if (Action == 2) // 선
		{
			// 마우스 클릭을 release 하면 라인을 새로 생성
			release.set(x, y);
			Line* line = new Line(pressed, release, VColor::random());
			vobjs.push_back((VObject*)line);
			guide_line.set_visible(false);
			glutPostRedisplay();
		}
		if (Action == 3) // 삼각형
		{
			release.set(x, y);
			Triangle* triangle = new Triangle(pressed, release, VColor::random());
			vobjs.push_back((VObject*)triangle);
			guide_triangle.set_visible(false);
			glutPostRedisplay();
		}
		else if (Action == 4) // 사각형
		{
			release.set(x, y);
			Rectangle* rect = new Rectangle(pressed, release, VColor::random());
			vobjs.push_back((VObject*)rect);
			guide_rectangle.set_visible(false);
			glutPostRedisplay();
		}
	}
}

// 마우스 모션 이벤트 처리
void mouseMotion(int x, int y)
{
	if (Action == 1)
	{
		// printf("mouse motion pos = (%d,%d)\n", x, y);

		Dots* dots = (Dots*)vobjs.at(vobjs.size() - 1);
		dots->add_point(x, y);
		glutPostRedisplay();
	}
	else if (Action == 2)
	{
		guide_line.set_end_point(Point(x, y));
		glutPostRedisplay();
	}
	else if (Action == 3)
	{
		guide_triangle.set_end_point(Point(x, y));
		glutPostRedisplay();
	}
	else if (Action == 4) // 사각형
	{
		guide_rectangle.set_end_point(Point(x, y));
		glutPostRedisplay();
	}
}

void setMenu()
{
	glutCreateMenu(DoMenu);
	glutAddMenuEntry("점", 1);
	glutAddMenuEntry("선", 2);
	glutAddMenuEntry("삼각형", 3);
	glutAddMenuEntry("사각형", 4);
	//glutAddMenuEntry("텍스트", 5);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void reshape(int width, int height)
{
	window_width = width;
	window_height = height;
	Point::window_width = window_width;
	Point::window_height = window_height;
	glViewport(0, 0, window_width, window_height);
}

void init(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("OpenGL");

	glutReshapeFunc(reshape);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMotion);
	glutKeyboardFunc(keyboardDown);
	glutDisplayFunc(DoDisplay);

	glEnable(GL_BLEND);
	glEnable(GL_POINT_SMOOTH); // 점을 둥글게 그리기 위해 안티엘리어싱 on
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);

	glClearColor(0.8, 0.8, 0.8, 0.8);


	Point::window_width = window_width;
	Point::window_height = window_height;
	guide_line.set_visible(false);
	guide_triangle.set_visible(false);
	guide_rectangle.set_visible(false);

	setMenu();
}

int main(int argc, char** argv)
{
	init(argc, argv);

	glutMainLoop();

	return 0;
}

