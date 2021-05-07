#include "includes.h"

CollisionSphere::CollisionSphere()
{
	Radius = 1.0f;

	return;
}

CollisionSphere::CollisionSphere(float r)
{
	Radius = r;

	return;
}

Vector3D<float> CollisionSphere::GetPointClosestTo(const Vector3D<float> &Ol)
{
	if (!Parent) return Vector3D<float>(0.0f);

	return (Ol - GetWorldLocation()).Normal() * Radius + GetWorldLocation();
}

Vector3D<float> CollisionSphere::GetPointFurthestFrom(const Vector3D<float> &p)
{
	if (!Parent) return Vector3D<float>(0.0f);

	if (p == GetWorldLocation()) return Vector3D<float>(Radius, 0.0f, 0.0f) + GetWorldLocation();

	return (GetWorldLocation() - p).Normal() * Radius + GetWorldLocation();
}


bool CollisionSphere::IsWithin(const Vector3D<float> &v)
{
	if (Radius * Radius >= (v - GetWorldLocation()).LengthSquared()) return true;
	else return false;
}

/*void CollisionSphere::CheckAgainstWorld(void)
{
	float w = RenderArea->Viewport.Width * 0.5f;
	float h = RenderArea->Viewport.Height * 0.5f;
	Vector3D<float> vel = Parent->Location - Parent->LastLocation;
	Vector3D<float> loc = GetWorldLocation();
	Vector3D<float> fl, fv;
	bool bBlock = false;

	fl = loc;

	if ((vel.X > 0.0f) && (loc.X > w - Radius))
	{
		bBlock = true;
		fl.X = w;
		fv.X = (w - Radius) - loc.X;
	}
	else if ((vel.X < 0.0f) && (loc.X < -w + Radius))
	{
		bBlock = true;
		fl.X = -w;
		fv.X = (-w + Radius) + loc.X;
	}
	if ((vel.Y > 0.0f) && (loc.Y > h - Radius))
	{
		bBlock = true;
		fl.Y = h;
		fv.Y = (h - Radius) - loc.Y;
	}
	else if ((vel.Y < 0.0f) && (loc.Y < -h + Radius))
	{
		bBlock = true;
		fl.Y = -h;
		fv.Y = (-h + Radius) + loc.Y;
	}

	fv

	return;
}*/

CollisionSphere::~CollisionSphere()
{
	return;
}
