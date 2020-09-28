// ----------------------------------------------------
// Point class    -----------
// ----------------------------------------------------

#ifndef __P2POINT_H__
#define __P2POINT_H__

#include <math.h>

template<class TYPE>
class p2Point
{
public:

	TYPE x, y;

	p2Point() : x(0), y(0)
	{}

	p2Point()
	{}

	p2Point(const p2Point& p)
	{
		x = p.x;
		y = p.y;
	}

	p2Point(const p2Point<TYPE>& v)
	{
		this->x = v.x;
		this->y = v.y;
	}

	p2Point(const TYPE& x, const TYPE& y)
	{
		this->x = x;
		this->y = y;
	}

	p2Point& create(const TYPE& x, const TYPE& y)
	{
		this->x = x;
		this->y = y;

		return(*this);
	}

	operator p2Point<float>() const
	{
		return { (float)x, (float)y };
	}

	// Math ------------------------------------------------
	p2Point operator -(const p2Point& v) const
	{
		p2Point r;

		r.x = x - v.x;
		r.y = y - v.y;

		return(r);
	}

	p2Point operator + (const p2Point& v) const
	{
		p2Point r;

		r.x = x + v.x;
		r.y = y + v.y;

		return(r);
	}

	const p2Point& operator -=(const p2Point& v)
	{
		x -= v.x;
		y -= v.y;

		return(*this);
	}

	const p2Point& operator +=(const p2Point& v)
	{
		x += v.x;
		y += v.y;

		return(*this);
	}

	bool operator ==(const p2Point& v) const
	{
		return (x == v.x && y == v.y);
	}

	bool operator !=(const p2Point& v) const
	{
		return (x != v.x || y != v.y);
	}

	//Operators w/ Template -----------

	p2Point operator - (const TYPE& v) const
	{
		p2Point r;

		r.x = x - v;
		r.y = y - v;

		return(r);
	}

	p2Point operator + (const TYPE& v) const
	{
		p2Point r;

		r.x = x + v;
		r.y = y + v;

		return(r);
	}


	p2Point operator * (const TYPE& v) const
	{
		p2Point r;

		r.x = x * v;
		r.y = y * v;

		return(r);
	}

	p2Point operator / (const TYPE& v) const
	{
		p2Point r;

		r.x = x / v;
		r.y = y / v;

		return(r);
	}


	const p2Point& operator +=(const TYPE& v)
	{
		x += v;
		y += v;

		return(*this);
	}

	const p2Point& operator -=(const TYPE& v)
	{
		x -= v;
		y -= v;

		return(*this);
	}

	const p2Point& operator *=(const TYPE& v)
	{
		x *= v;
		y *= v;

		return(*this);
	}

	const p2Point& operator /=(const TYPE& v)
	{
		x /= v;
		y /= v;

		return(*this);
	}



	// Utils ------------------------------------------------
	bool IsZero() const
	{
		return (x == 0 && y == 0);
	}

	bool IsZeroAccurate() const
	{
		return ((x <= 0.001f && x >= -0.001f) && (y <= 0.001f && y >= -0.001f));
	}

	p2Point& SetToZero()
	{
		x = y = 0;
		return(*this);
	}

	p2Point& Negate()
	{
		x = -x;
		y = -y;

		return(*this);
	}

	p2Point& Abs()
	{
		x = abs(x);
		y = abs(y);

		return(*this);
	}

	p2Point GetInverse()
	{
		p2Point r;
		r.x = -x;
		r.y = -y;

		return(r);
	}

	double Dot(p2Point& v)
	{
		return((x * v.x) + (y * v.y));
	}

	double Module()
	{
		if ((x <= 0.01f && x >= -0.01f) && (y <= 0.01f && y >= -0.01f))//if the vector is nearly 0
			return 0;
		else
			return sqrtf((x * x) + (y * y));
	}

	void Normalize()
	{
		double mod = Module();
		if (mod != 0)
		{
			x = (x / mod);
			y = (y / mod);
		}
	}

	void RoundPoint()
	{
		x = round(x);
		y = round(y);
	}


	p2Point LerpPoint(p2Point& p1, float t)
	{
		return  { lerp(x, p1.x, t), lerp(y, p1.y, t) };
	}

	// Distances ---------------------------------------------
	TYPE DistanceTo(const p2Point& v) const
	{
		TYPE fx = x - v.x;
		TYPE fy = y - v.y;

		return (TYPE) sqrt((float) (fx*fx) + (fy*fy));
	}

	float OctileDistance(const p2Point& v) const
	{
		int dx = abs(v.x - x);
		int dy = abs(v.y - y);

		return  max(dx, dy) + (0.41f) * min(dx, dy);
	}

	TYPE DistanceNoSqrt(const p2Point& v) const
	{
		TYPE fx = x - v.x;
		TYPE fy = y - v.y;

		return (fx * fx) + (fy * fy);
	}

	TYPE DistanceManhattan(const p2Point& v) const
	{
		return abs(v.x - x) + abs(v.y - y);
	}

	float DiagonalDistance(const p2Point& v) const
	{
		float dx = v.x - x, dy = v.y - y;
		return (MAX(abs(dx), abs(dy)));
	}

};

#endif // __P2POINT_H__