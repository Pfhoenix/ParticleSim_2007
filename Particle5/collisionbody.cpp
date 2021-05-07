#include "includes.h"

CollisionBody::CollisionBody()
{
	Parent = NULL;
	
	return;
}

void CollisionBody::SetParentRotation(bool b)
{
	if (b && Parent)
	{
		Xaxis = Parent->Xaxis;
		Yaxis = Parent->Yaxis;
		Zaxis = Parent->Zaxis;
		RotationOffset.x = 0.0f;
		RotationOffset.y = 0.0f;
		RotationOffset.z = 0.0f;
		RotationOffset.w = 1.0f;
		bParentRotation = true;
	}
	else
	{
		bParentRotation = false;
	}

	return;
}

Vector3D<float> CollisionBody::GetWorldLocation(void)
{
	return Parent->Location + Xaxis * LocationOffset.X + Yaxis * LocationOffset.Y;// + LocationOffset.Z + Zaxis;
}

Vector3D<float> CollisionBody::GetPointClosestTo(const Vector3D<float> &Op)
{
	if (!Parent) return Vector3D<float>(0.0f);

	Vector3D<float> p(Parent->Location + LocationOffset);

	return p;
}

Vector3D<float> CollisionBody::GetPointFurthestFrom(const Vector3D<float> &Op)
{
	if (!Parent) return Vector3D<float>(0.0f);

	Vector3D<float> p(Parent->Location + LocationOffset);

	return p;
}

bool CollisionBody::IsWithin(const Vector3D<float> &v)
{
	return false;
}

void CollisionBody::UpdateRotation(void)
{
	if (!Parent) return;

	if (bParentRotation)
	{
		Xaxis = Parent->Xaxis;
		Yaxis = Parent->Yaxis;
		Zaxis = Parent->Zaxis;
	}
	else
	{
		GetAxes(Parent->Rotation * RotationOffset, &Xaxis, &Yaxis, &Zaxis);
	}

	return;
}

CollisionBody::~CollisionBody()
{
	Parent = NULL;

	return;
}
