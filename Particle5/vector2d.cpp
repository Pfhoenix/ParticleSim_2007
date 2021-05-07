#include "includes.h"

Vector2D::Vector2D(void)
{
	X = 0.0;
	Y = 0.0;

	return;
}

Vector2D::Vector2D(double nX, double nY)
{
	SetValue(nX, nY);

	return;
}

void Vector2D::SetValue(double nX, double nY)
{
	X = nX;
	Y = nY;

	return;
}

double Vector2D::Length(void)
{
	return (sqrt(X * X + Y * Y));
}

Vector2D Vector2D::Normal(void)
{
	double f;

	f = Length();

	if (f != 0.0)
	{
		X *= 1.0 / f;
		Y *= 1.0 / f;
	}

	return Vector2D(X, Y);
}

bool Vector2D::operator == (const Vector2D V)
{
	return ((X == V.X) && (Y == V.Y));
}

bool Vector2D::operator != (const Vector2D V)
{
	return ((X != V.X) && (Y != V.Y));
}

Vector2D Vector2D::operator + (const Vector2D V)
{
	return Vector2D(X + V.X, Y + V.Y);
}

Vector2D Vector2D::operator - (const Vector2D V)
{
	return Vector2D(X - V.X, Y - V.Y);
}

Vector2D Vector2D::operator * (int i)
{
	return Vector2D(X * i, Y * i);
}

Vector2D Vector2D::operator * (float f)
{
	return Vector2D(X * f, Y * f);
}

Vector2D Vector2D::operator * (double d)
{
	return Vector2D(X * d, Y * d);
}

Vector2D Vector2D::operator / (int i)
{
	return Vector2D(X / i, Y / i);
}

Vector2D Vector2D::operator / (float f)
{
	return Vector2D(X / f, Y / f);
}

Vector2D Vector2D::operator / (double d)
{
	return Vector2D(X / d, Y / d);
}

void Vector2D::operator -= (const Vector2D v)
{
	X -= v.X;
	Y -= v.Y;

	return;
}

void Vector2D::operator += (const Vector2D v)
{
	X += v.X;
	Y += v.Y;

	return;
}

void Vector2D::operator *= (int i)
{
	X *= i;
	Y *= i;

	return;
}

void Vector2D::operator *= (float f)
{
	X *= f;
	Y *= f;

	return;
}

void Vector2D::operator *= (double d)
{
	X *= d;
	Y *= d;

	return;
}

void Vector2D::operator /= (int i)
{
	X /= i;
	Y /= i;

	return;
}

void Vector2D::operator /= (float f)
{
	X /= f;
	Y /= f;

	return;
}

void Vector2D::operator /= (double d)
{
	X /= d;
	Y /= d;

	return;
}

Vector2D::~Vector2D(void)
{
	return;
}
