#include <iostream>
#include <gl/glut.h>
#include <vector>

#include "vobject.h"
#include "point.h"
#include "vtree.h"

using namespace std;

const int VO_LINE = 2;
const int VO_TRIANGLE = 3;
const int VO_RECTANGLE = 4;
const int VO_REMOVE = 0;

int window_width = 1000;
int window_height = 800;

int Action = 1;

Point pressed(0, 0);
Point release(0, 0);

VTree vtree;

Line guide_line(pressed, release);
Triangle guide_triangle(pressed, release);
Rectangle guide_rectangle(pressed, release);
Rectangle guide_remove(pressed, release);

void displayText(float x, float y, const char *string)
{
	int j = strlen(string);

	glRasterPos2f(x, y);
	for (int i = 0; i < j; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
	}
}

Point make_rand_point()
{
	int x = rand() % window_width;
	int y = rand() % window_height;
	
	return Point(x, y);
}

void DoMenu(int value)
{
	Action = value;
	Point p1 = make_rand_point();
	Point p2 = make_rand_point();

	if (value == VO_LINE) vtree.add(new Line(p1, p2, VColor::random()));
	else if (value == VO_TRIANGLE) vtree.add(new Triangle(p1, p2, VColor::random()));
	else if (value == VO_RECTANGLE) vtree.add(new Rectangle(p1, p2, VColor::random()));
	// else if (value = VO_REMOVE);

	glutPostRedisplay();
}

void keyboardDown(unsigned char key, int x, int y)
{
	// printf("key pressed : %d\n", (int)key);

	switch (key)
	{
	case 'q':
	case 227: // �ѱ� q
		exit(0);
		break;

	case 'l':
		DoMenu(VO_LINE);
		break;

	case 't':
		DoMenu(VO_TRIANGLE);
		break;

	case 'r':
		DoMenu(VO_RECTANGLE);
		break;

	case 'd':
		DoMenu(VO_REMOVE);
		break;
	}
}

VObject* get_guide_vobj(int Action)
{
	if (Action == VO_LINE) return &guide_line;
	if (Action == VO_TRIANGLE) return &guide_triangle;
	if (Action == VO_RECTANGLE) return &guide_rectangle;
	if (Action == VO_REMOVE) return &guide_remove;

	return NULL;
}

void DoDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);

	vtree.draw();

	VObject* guid_vobj = get_guide_vobj(Action);
	if (guid_vobj) guid_vobj->draw();

	glutSwapBuffers();
}



VObject* create_new_vobj(int Action)
{
	if (Action == VO_LINE) return new Line(pressed, release, VColor::random());
	if (Action == VO_TRIANGLE) return new Triangle(pressed, release, VColor::random());
	if (Action == VO_RECTANGLE) return new Rectangle(pressed, release, VColor::random());

	return NULL;
}

// ���콺 Ŭ�� �̺�Ʈ ó��
void mouseClick(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) // pressed
	{
		pressed.set(x, y);

		VObject* guid_vobj = get_guide_vobj(Action);
		if (guid_vobj)
		{
			guid_vobj->set_start_point(pressed);
			guid_vobj->set_visible(true);
		}
	}
	else // released
	{
		release.set(x, y);

		VObject* guid_vobj = get_guide_vobj(Action);
		if (guid_vobj) guid_vobj->set_visible(false);
		
		if (Action == VO_REMOVE)
		{
			vtree.remove(pressed, release);
		}
		else
		{
			VObject* vobj = create_new_vobj(Action);
			if (vobj) vtree.add(vobj);
		}

		glutPostRedisplay();
	}
}

// ���콺 ��� �̺�Ʈ ó��
void mouseMotion(int x, int y)
{
	VObject* guid_vobj = get_guide_vobj(Action);
	if (guid_vobj) guid_vobj->set_end_point(Point(x, y));
	glutPostRedisplay();
}

void setMenu()
{
	glutCreateMenu(DoMenu);
	glutAddMenuEntry("��(L)", VO_LINE);
	glutAddMenuEntry("�ﰢ��(T)", VO_TRIANGLE);
	glutAddMenuEntry("�簢��(R)", VO_RECTANGLE);
	//glutAddMenuEntry("�ؽ�Ʈ", 5);
	glutAddMenuEntry("�����(D)", VO_REMOVE);

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
	glEnable(GL_POINT_SMOOTH); // ���� �ձ۰� �׸��� ���� ��Ƽ������� on
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

