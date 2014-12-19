#include <iostream>
#include <gl/glut.h>
#include <vector>

#include "vobject.h"
#include "point.h"
#include "vtree.h"

using namespace std;

const int VO_POINT = 1;
const int VO_LINE = 2;
const int VO_TRIANGLE = 3;
const int VO_RECTANGLE = 4;
const int VO_TEXT = 5;
const int VO_REMOVE = 0;

int window_width = 1000;
int window_height = 800;

int Action = 1;

Point pressed(0, 0);
Point release(0, 0);

VTree vtree;

VLine guide_line(pressed, release);
VTriangle guide_triangle(pressed, release);
VRectangle guide_rectangle(pressed, release);
VRectangle guide_remove(pressed, release);

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

	if (value == VO_POINT) vtree.add(new VPoint(p1, VColor::random()));
	else if (value == VO_LINE) vtree.add(new VLine(p1, p2, VColor::random()));
	else if (value == VO_TRIANGLE) vtree.add(new VTriangle(p1, p2, VColor::random()));
	else if (value == VO_RECTANGLE) vtree.add(new VRectangle(p1, p2, VColor::random()));
	else if (value == VO_TEXT) vtree.add(new VText(p1, VColor::random()));
	// else if (value = VO_REMOVE);

	glutPostRedisplay();
}

void keyboardDown(unsigned char key, int x, int y)
{
	// printf("key pressed : %d\n", (int)key);

	switch (key)
	{
	case 'q':
	case 227: // 한글 q
		exit(0);
		break;

	case 'p':
		DoMenu(VO_POINT);
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

	case 'c':
		DoMenu(VO_TEXT);
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
	if (Action == VO_POINT) return new VPoint(pressed, VColor::random());
	if (Action == VO_LINE) return new VLine(pressed, release, VColor::random());
	if (Action == VO_TRIANGLE) return new VTriangle(pressed, release, VColor::random());
	if (Action == VO_RECTANGLE) return new VRectangle(pressed, release, VColor::random());
	if (Action == VO_TEXT) return new VText(pressed, VColor::random());

	return NULL;
}

// 마우스 클릭 이벤트 처리
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

// 마우스 모션 이벤트 처리
void mouseMotion(int x, int y)
{
	VObject* guid_vobj = get_guide_vobj(Action);
	if (guid_vobj) guid_vobj->set_end_point(Point(x, y));
	glutPostRedisplay();
}

void setMenu()
{
	glutCreateMenu(DoMenu);
	glutAddMenuEntry("점(P)", VO_POINT);
	glutAddMenuEntry("선(L)", VO_LINE);
	glutAddMenuEntry("삼각형(T)", VO_TRIANGLE);
	glutAddMenuEntry("사각형(R)", VO_RECTANGLE);
	glutAddMenuEntry("텍스트(C)", VO_TEXT);
	glutAddMenuEntry("지우기(D)", VO_REMOVE);

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

