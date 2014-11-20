#ifndef __VOBJECT_H__
#define __VOBJECT_H__

#include <iostream>
#include <vector>
#include <gl/glut.h>
#include "point.h"
#include "vcolor.h"

using namespace std;

class VObject
{
public:
	virtual void draw() = 0;

	VObject() : visible(true)
	{
	}

	void set_visible(bool tf)
	{
		visible = tf;
	}

protected:
	bool visible;
};

class Rectangle : public VObject
{
public:
	Rectangle(const Point& p1, const Point& p2, const VColor& c = VColor(254, 244, 156))
		: p1(p1), p2(p2), color(c)
	{
	}

	virtual void draw()
	{
		if (!visible) return;

		glColor4f(color.r, color.g, color.b, color.a);
		glRectf(p1.xf(), p1.yf(), p2.xf(), p2.yf());
	}

	void set_start_point(const Point& p)
	{
		p1 = p;
	}

	void set_end_point(const Point& p)
	{
		p2 = p;
	}

private:
	Point p1;
	Point p2;
	VColor color;
};

class Triangle : public VObject
{
public:
	Triangle(const Point& p1, const Point& p2, const VColor& c = VColor(254, 244, 156))
		: p1(p1), p2(p2), color(c)
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

	void set_start_point(const Point& p)
	{
		p1 = p;
	}

	void set_end_point(const Point& p)
	{
		p2 = p;
	}

private:
	Point p1;
	Point p2;
	VColor color;
};

class Line : public VObject
{
public:
	Line(const Point& s, const Point& e, const VColor& c = VColor(254,244,156) )
		: point_start(s), point_end(e), color(c)
	{
	}

	virtual void draw()
	{
		if (!visible) return;

		glColor4f(color.r, color.g, color.b, color.a);
		glBegin(GL_LINES);
		glVertex2f(point_start.xf(), point_start.yf());
		glVertex2f(point_end.xf(), point_end.yf());
		glEnd();
	}

	void set_start_point(const Point& p)
	{
		point_start = p;
	}

	void set_end_point(const Point& p)
	{
		point_end = p;
	}

private:
	Point point_start;
	Point point_end;
	VColor color;
};

class Dots : public VObject
{
public:
	Dots(const VColor& c) : color(c)
	{
	}

	virtual void draw()
	{
		if (!visible) return;

		glPointSize(5.0);
		glColor4f(color.r, color.g, color.b, color.a);
		glBegin(GL_POINTS);
		int size = points.size();
		for (int i = 0; i < size; i++)
		{
			glVertex2f(points[i].xf(), points[i].yf());
			// printf("(%f %f)\n", points[i].xf(), points[i].yf());
		}
		glEnd();
	}

	void add_point(const Point& p)
	{
		points.push_back(p);
	}

	void add_point(int x, int y)
	{
		points.push_back(Point(x, y));
	}

private:
	vector<Point> points;
	VColor color;;
};

#endif