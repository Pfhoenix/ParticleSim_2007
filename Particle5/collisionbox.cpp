#include "includes.h"

CollisionBox::CollisionBox()
{
	return;
}

CollisionBox::CollisionBox(const Vector3D<float> &dim)
{
	Dimensions = dim;

	return;
}

Vector3D<float> CollisionBox::GetPointClosestTo(const Vector3D<float> &Ol)
{
	Vector3D<float> loc = GetWorldLocation();
	return loc;
}

Vector3D<float> CollisionBox::GetPointFurthestFrom(const Vector3D<float> &Ol)
{
	// calculate the squared distances of all the corners
	// the largest value is the farthest away
	float ds = 0.0f;
	Vector3D<float> o = GetWorldLocation();
	Vector3D<float> c;
	Vector3D<float> dc;

	c = o + Xaxis * Dimensions.X + Yaxis * Dimensions.Y + Zaxis * Dimensions.Z;
	if ((c - Ol).LengthSquared() > ds)
	{
		dc = c;
		ds = (c - Ol).LengthSquared();
	}
	c = o - Xaxis * Dimensions.X + Yaxis * Dimensions.Y + Zaxis * Dimensions.Z;
	if ((c - Ol).LengthSquared() > ds)
	{
		dc = c;
		ds = (c - Ol).LengthSquared();
	}
	c = o + Xaxis * Dimensions.X - Yaxis * Dimensions.Y + Zaxis * Dimensions.Z;
	if ((c - Ol).LengthSquared() > ds)
	{
		dc = c;
		ds = (c - Ol).LengthSquared();
	}
	c = o - Xaxis * Dimensions.X - Yaxis * Dimensions.Y + Zaxis * Dimensions.Z;
	if ((c - Ol).LengthSquared() > ds)
	{
		dc = c;
		ds = (c - Ol).LengthSquared();
	}
	c = o + Xaxis * Dimensions.X + Yaxis * Dimensions.Y - Zaxis * Dimensions.Z;
	if ((c - Ol).LengthSquared() > ds)
	{
		dc = c;
		ds = (c - Ol).LengthSquared();
	}
	c = o - Xaxis * Dimensions.X + Yaxis * Dimensions.Y - Zaxis * Dimensions.Z;
	if ((c - Ol).LengthSquared() > ds)
	{
		dc = c;
		ds = (c - Ol).LengthSquared();
	}
	c = o + Xaxis * Dimensions.X - Yaxis * Dimensions.Y - Zaxis * Dimensions.Z;
	if ((c - Ol).LengthSquared() > ds)
	{
		dc = c;
		ds = (c - Ol).LengthSquared();
	}
	c = o - Xaxis * Dimensions.X - Yaxis * Dimensions.Y - Zaxis * Dimensions.Z;
	if ((c - Ol).LengthSquared() > ds)
	{
		dc = c;
		ds = (c - Ol).LengthSquared();
	}

	return dc;
}

bool CollisionBox::IsWithin(const Vector3D<float> &p)
{
	Vector3D<float> o = GetWorldLocation();
	Vector3D<float> d = o - p;
	Vector3D<float> v;

	v.X = d | Dimensions.X;
	v.Y = d | Dimensions.Y;
	v.Z = d | Dimensions.Z;

	if (fabs(v.X) > Dimensions.X) return false;
	if (fabs(v.Y) > Dimensions.Y) return false;
	if (fabs(v.Z) > Dimensions.Z) return false;

	return true;
}

CollisionBox::~CollisionBox()
{
	return;
}
