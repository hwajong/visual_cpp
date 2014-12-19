#ifndef __VCOLOR_H__
#define __VCOLOR_H__

class VColor
{
public:
	float r;
	float g;
	float b;
	float a;

	VColor(float r, float g, float b, float a = 1)
		: r(r), g(g), b(b), a(a)
	{
	}

	VColor(int r, int g, int b, float a = 1)
		: r(r / 255.0), g(g / 255.0), b(b / 255.0), a(a / 255.0)
	{
	}

	VColor(const VColor& c)
	{
		if (&c != this)
		{
			this->r = c.r;
			this->g = c.g;
			this->b = c.b;
			this->a = c.a;
		}
	}

	static VColor random()
	{
		float rand_r = (float)rand() / RAND_MAX;
		float rand_g = (float)rand() / RAND_MAX;
		float rand_b = (float)rand() / RAND_MAX;

		//printf("%3.2f %3.2f %3.2f\n", rand_r, rand_g, rand_b);

		return VColor(rand_r, rand_g, rand_b);
	}
};

#endif