#include "includes.h"

CollisionRod::CollisionRod()
{
	Radius = 1.0f;
	Length = 1.0f;

	return;
}

CollisionRod::CollisionRod(float R, float L)
{
	Radius = R;
	Length = L;

	return;
}

Vector3D<float> CollisionRod::GetPointClosestTo(const Vector3D<float> &Ol)
{
	if (!Parent) return Vector3D<float>(0.0f);

	Vector3D<float> o(GetWorldLocation());
	Vector3D<float> p, pt = (Ol - o);

	// get the distance along each axis
	p.X = pt | Xaxis;
	//p.Y = pt | y;
	//p.Z = pt | z;
	// we can specialize this due to being essentially a cylinder
	if (fabs(p.X) > (Length * 0.5f - Radius))
	{
		pt = o + Xaxis * (Length * 0.5f - Radius);
		return (Ol - pt).Normal() * Radius + o;
	}
	else return (Ol - (o + Xaxis * p.X)).Normal() * Radius + o;
}

Vector3D<float> CollisionRod::GetPointFurthestFrom(const Vector3D<float> &Ol)
{
	if (!Parent) return Vector3D<float>(0.0f);

	Vector3D<float> o(GetWorldLocation());
	Vector3D<float> p, pt = (Ol - o);

	// get the distance along each axis
	p.X = pt | Xaxis;
	//p.Y = pt | y;
	//p.Z = pt | z;
	// we can specialize this due to being essentially a cylinder
	if (fabs(p.X) > (Length * 0.5f - Radius))
	{
		pt = o - Xaxis * (Length * 0.5f - Radius);
		return (Ol - pt).Normal() * Radius + o;
	}
	else return (Ol - (o - Xaxis * p.X)).Normal() * Radius + o;

}

bool CollisionRod::IsWithin(const Vector3D<float> &v)
{
	Vector3D<float> o = v - GetWorldLocation();

	if (Radius * Radius >= (o - Xaxis * (Length * 0.5f - Radius)).LengthSquared()) return true;
	if (Radius * Radius >= (o + Xaxis * (Length * 0.5f - Radius)).LengthSquared()) return true;

	float x = v | Xaxis;

	if (x * x > (Length * 0.5f - Radius) * (Length * 0.5f - Radius)) return false;

	if (Radius * Radius < (o - Xaxis * x).LengthSquared()) return false;

	return true;
}

CollisionRod::~CollisionRod()
{
	return;
}
