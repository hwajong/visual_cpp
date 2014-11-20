#ifndef __POINT_H__
#define __POINT_H__

class Point
{
public:
	static int window_width;
	static int window_height;

	int x;
	int y;

	Point(int x, int y) : x(x), y(y)
	{
	}

	Point(const Point& p)
	{
		if (&p != this)
		{
			this->x = p.x;
			this->y = p.y;
		}
	}

	void set(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	float xf()
	{
		return (2 * x - window_width) / (float)window_width;
	}

	float yf()
	{
		return (window_height - 2 * y) / (float)window_height;
	}
};

int Point::window_width = 0;
int Point::window_height = 0;

#endif