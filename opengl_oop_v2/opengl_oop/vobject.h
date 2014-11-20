#ifndef __VOBJECT_H__
#define __VOBJECT_H__

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <gl/glut.h>
#include "point.h"
#include "vcolor.h"


using namespace std;

class VObject
{
public:
	virtual void draw() = 0;

	virtual string name() = 0;

	VObject(const Point& p1, const Point& p2, const VColor& c = VColor(254, 244, 156)) 
		: p1(p1), p2(p2), color(c), visible(true)
	{
	}

	void set_visible(bool tf)
	{
		visible = tf;
	}

	bool is_include_area(VObject* vobj2)
	{
		int x_min = min(p1.x, p2.x);
		int x_max = max(p1.x, p2.x);

		int y_min = min(p1.y, p2.y);
		int y_max = max(p1.y, p2.y);

		if (vobj2->p1.x < x_min || vobj2->p1.x > x_max) return false;
		if (vobj2->p2.x < x_min || vobj2->p2.x > x_max) return false;

		if (vobj2->p1.y < y_min || vobj2->p1.y > y_max) return false;
		if (vobj2->p2.y < y_min || vobj2->p2.y > y_max) return false;

		return true;
	}

	void set_start_point(const Point& p)
	{
		p1 = p;
	}

	void set_end_point(const Point& p)
	{
		p2 = p;
	}

protected:
	bool visible;
	Point p1;
	Point p2;
	VColor color;
};

class Rectangle : public VObject
{
public:
	Rectangle(const Point& p1, const Point& p2, const VColor& c = VColor(254, 244, 156))
		: VObject(p1, p2, c)
	{
	}

	virtual void draw()
	{
		if (!visible) return;

		glColor4f(color.r, color.g, color.b, color.a);
		glRectf(p1.xf(), p1.yf(), p2.xf(), p2.yf());
	}

	virtual string name() { return "Rectangle"; }
};

class Triangle : public VObject
{
public:
	Triangle(const Point& p1, const Point& p2, const VColor& c = VColor(254, 244, 156))
		: VObject(p1, p2, c)
	{
	}

	virtual void draw()
	{
		if (!visible) return;

		glColor4f(color.r, color.g, color.b, color.a);
		glBegin(GL_TRIANGLES);
		glVertex2f(p1.xf(), p2.yf());
		glVertex2f(p2.xf(), p2.yf());
		Point p3(p1.x + (p2.x - p1.x) / 2, p1.y);
		glVertex2f(p3.xf(), p3.yf());
		glEnd();
	}

	virtual string name() { return "Triangle"; }
};

class Line : public VObject
{
public:
	Line(const Point& p1, const Point& p2, const VColor& c = VColor(254,244,156) )
		: VObject(p1, p2, c)
	{
	}

	virtual void draw()
	{
		if (!visible) return;

		glColor4f(color.r, color.g, color.b, color.a);
		glBegin(GL_LINES);
		glVertex2f(p1.xf(), p1.yf());
		glVertex2f(p2.xf(), p2.yf());
		glEnd();
	}

	virtual string name() { return "Line"; }
};

#endif