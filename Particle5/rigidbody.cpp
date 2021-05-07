#include "includes.h"

RigidBody::RigidBody(float M)
{
	ParentObject = NULL;
	Mass = M;
	memset(&MoI, 0, sizeof(D3DXMATRIX));
	Rotation.x = 0.0f;
	Rotation.y = 0.0f;
	Rotation.z = 0.0f;
	Rotation.w = 1.0f;
	Restitution = 0.8f;
	ColSphereRadius = 1.0f;
	Xaxis.SetValue(1.0f, 0.0f, 0.0f);
	Yaxis.SetValue(0.0f, 1.0f, 0.0f);
	Zaxis.SetValue(0.0f, 0.0f, 1.0f);
	AngularVelScalar = 0.0f;
	FixedType = RBFT_NONE;

	return;
}

void RigidBody::AddCollisionBody(CollisionBody *CB, const Vector3D<float> &lo, bool bP, const D3DXQUATERNION &ro)
{
	if (!CB) return;

	CB->Parent = this;
	CB->LocationOffset = lo;
	if (bP) CB->SetParentRotation(bP);
	else CB->RotationOffset = ro;
	ColBodies.push_back(CB);
	Vector3D<float> p = CB->GetPointFurthestFrom(Location);
	if ((Location - p).LengthSquared() > ColSphereRadius * ColSphereRadius) ColSphereRadius = (Location - p).Length();

	return;
}

void RigidBody::RemoveCollisionBody(CollisionBody *CB)
{
	if (!CB) return;

	for (int i = 0; i < (int) ColBodies.size(); i++)
		if (ColBodies[i] == CB)
		{
			delete ColBodies[i];
			vector<CollisionBody*>::iterator it = ColBodies.begin();
			it += i;
			ColBodies.erase(it);
			break;
		}

	return;
}

void RigidBody::SetRotation(const D3DXQUATERNION &q)
{
	Rotation = q;
	GetAxes(q, &Xaxis, &Yaxis, &Zaxis);
	for (int i = 0; i < (int) ColBodies.size(); i++)
		ColBodies[i]->UpdateRotation();

	return;
}

void RigidBody::SetFixedType(void)
{
	FixedType = RBFT_NONE;

	return;
}

void RigidBody::SetFixedType(const Vector3D<float> &fl)
{
	FixedType = RBFT_FREESPINNER;
	Location = fl;
	LastLocation = fl;

	return;
}

void RigidBody::SetFixedType(const D3DXQUATERNION &fr)
{
	FixedType = RBFT_MOVER;
	SetRotation(fr);

	return;
}

void RigidBody::SetFixedType(const Vector3D<float> &fl, const D3DXQUATERNION &fr)
{
	FixedType = RBFT_FROZEN;
	Location = fl;
	LastLocation = fl;
	SetRotation(fr);

	return;
}

void RigidBody::SetFixedType(const Vector3D<float> &fl, const Vector3D<float> &frr)
{
	FixedType = RBFT_FIXEDSPINNER;
	Location = fl;
	LastLocation = fl;
	FixedRotRate = frr;

	return;
}

void RigidBody::ApplyImpulse(Vector3D<float> fl, Vector3D<float> fv)
{
	Vector3D<float> LinearAccel;
	D3DXQUATERNION AngularAccel;

	if (!ParentObject->bUserHeld && ((FixedType == RBFT_NONE) || (FixedType == RBFT_MOVER))) LinearAccel = fv / Mass * DeltaTime;
	if ((FixedType == RBFT_NONE) || (FixedType == RBFT_FREESPINNER))
	{
		if (fl != Location)
		{
			Vector3D<float> axis = (Location - fl).Normal() ^ fv.Normal();
			Vector3D<float> r = (Location - fl).Normal() ^ axis;
			float f = r | fv.Normal() * 0.5f * fv.Length() * DeltaTime * DeltaTime;
			D3DXQuaternionRotationAxis(&AngularAccel, &D3DXVECTOR3(axis.X, axis.Y, axis.Z), f);
		}
		else
		{
			AngularAccel.x = 0.0f;
			AngularAccel.y = 0.0f;
			AngularAccel.z = 0.0f;
			AngularAccel.w = 1.0f;
		}
	}

	// process linear movement
	Vector3D<float> OL = Location;
	if (!ParentObject->bUserHeld && ((FixedType == RBFT_NONE) || (FixedType == RBFT_MOVER)))
	{
		Vector3D<float> tv, DesiredLocation = Location;
		DesiredLocation += (Location - LastLocation) * (DeltaTime / LastDeltaTime) + LinearAccel * DeltaTime * DeltaTime;
		tv = DesiredLocation - Location;
		if (tv.LengthSquared() > (MaxLinearSpeed * MaxLinearSpeed * DeltaTime * DeltaTime)) DesiredLocation = tv.Normal() * MaxLinearSpeed * DeltaTime + Location;
		Space->Move(ParentObject, DesiredLocation);
	}
	LastLocation = OL;
	// process angular movement
	if (FixedType == RBFT_FIXEDSPINNER)
	{
		D3DXQUATERNION q;
		D3DXQuaternionRotationYawPitchRoll(&q, FixedRotRate.X * DeltaTime, FixedRotRate.Y * DeltaTime, FixedRotRate.Z * DeltaTime);
		SetRotation(Rotation * q);
	}
	else if ((FixedType == RBFT_NONE) || (FixedType == RBFT_FREESPINNER))
	{
		D3DXQUATERNION AngVel;
		D3DXQuaternionRotationAxis(&AngVel, &D3DXVECTOR3(AngularVelAxis.X, AngularVelAxis.Y, AngularVelAxis.Z), AngularVelScalar * DeltaTime);
		AngVel *= AngularAccel;
		SetRotation(Rotation * AngVel);
		Vector3D<float>v(AngVel.x, AngVel.y, AngVel.z);
		float f = v.LengthSquared();
		if (f == 0.0f)
		{
			AngularVelAxis.SetValue(0.0f, 0.0f, 0.0f);
			AngularVelScalar = 0.0f;
		}
		else
		{
			AngularVelAxis.X = AngVel.x;// / f;
			AngularVelAxis.Y = AngVel.y;// / f;
			AngularVelAxis.Z = AngVel.z;// / f;
			AngularVelAxis.SetLength(1.0f);
			if (AngVel.w >= 1.0f) AngularVelScalar = 1.99999f;
			else if (AngVel.w <= -1.0f) AngularVelScalar = -1.99999f;
			else AngularVelScalar = 2.0f * acos(AngVel.w);
		}
	}

	return;
}

void RigidBody::QueueForce(const Vector3D<float> &fl, const Vector3D<float> &fv)
{
	ForceLocs.push_back(fl);
	ForceVects.push_back(fv);

	return;
}

void RigidBody::ProcessPhysics(float DeltaTime, float LastDeltaTime)
{
	Vector3D<float> LinearAccel;
	D3DXQUATERNION AngularAccel;
	AngularAccel.x = AngularAccel.y = AngularAccel.z = 0.0f;
	AngularAccel.w = 1.0f;

	if (Particle *p = dynamic_cast<Particle*>(ParentObject))
	{
		stringstream ss;
		ss << "particle with charge " << p->Charge << " has " << ForceLocs.size() << " forces of :\n";
		for (int i = 0; i < (int) ForceLocs.size(); i++)
			ss << ForceLocs[i].X << "," << ForceLocs[i].Y << "," << ForceLocs[i].Z << "\n";
		Log(ss.str());
	}

	for (int i = 0; i < (int) ForceLocs.size(); i++)
	{
		if (!ParentObject->bUserHeld && ((FixedType == RBFT_NONE) || (FixedType == RBFT_MOVER))) LinearAccel += ForceVects[i] / Mass * DeltaTime;
		if ((FixedType == RBFT_NONE) || (FixedType == RBFT_FREESPINNER))
		{
			if (ForceLocs[i] != Location)
			{
				D3DXQUATERNION q;

				Vector3D<float> axis = (Location - ForceLocs[i]).Normal() ^ ForceVects[i].Normal();
				Vector3D<float> r = (Location - ForceLocs[i]).Normal() ^ axis;
				float f = r | ForceVects[i].Normal() * 0.5f * ForceVects[i].Length() * DeltaTime * DeltaTime;
				D3DXQuaternionRotationAxis(&q, &D3DXVECTOR3(axis.X, axis.Y, axis.Z), f);
				AngularAccel *= q;
			}
		}
	}

	ForceLocs.clear();
	ForceVects.clear();

	// process linear movement
	Vector3D<float> OL = Location;
	if (!ParentObject->bUserHeld && ((FixedType == RBFT_NONE) || (FixedType == RBFT_MOVER)))
	{
		Vector3D<float> tv, DesiredLocation = Location;
		DesiredLocation += (Location - LastLocation) * (DeltaTime / LastDeltaTime) + LinearAccel * DeltaTime * DeltaTime;
		tv = DesiredLocation - OL;
		if (tv.LengthSquared() > (MaxLinearSpeed * MaxLinearSpeed * DeltaTime * DeltaTime)) DesiredLocation = tv.Normal() * MaxLinearSpeed * DeltaTime + OL;
		Space->Move(ParentObject, DesiredLocation);
	}
	LastLocation = OL;
	// process angular movement
	if (FixedType == RBFT_FIXEDSPINNER)
	{
		D3DXQUATERNION q;
		D3DXQuaternionRotationYawPitchRoll(&q, FixedRotRate.X * DeltaTime, FixedRotRate.Y * DeltaTime, FixedRotRate.Z * DeltaTime);
		SetRotation(Rotation * q);
	}
	else if ((FixedType == RBFT_NONE) || (FixedType == RBFT_FREESPINNER))
	{
		D3DXQUATERNION AngVel;
		D3DXQuaternionRotationAxis(&AngVel, &D3DXVECTOR3(AngularVelAxis.X, AngularVelAxis.Y, AngularVelAxis.Z), AngularVelScalar * DeltaTime);
		AngVel *= AngularAccel;
		SetRotation(Rotation * AngVel);
		Vector3D<float>v(AngVel.x, AngVel.y, AngVel.z);
		float f = v.LengthSquared();
		if (f == 0.0f)
		{
			AngularVelAxis.SetValue(0.0f, 0.0f, 0.0f);
			AngularVelScalar = 0.0f;
		}
		else
		{
			AngularVelAxis.X = AngVel.x;// / f;
			AngularVelAxis.Y = AngVel.y;// / f;
			AngularVelAxis.Z = AngVel.z;// / f;
			AngularVelAxis.SetLength(1.0f);
			if (AngVel.w >= 1.0f) AngularVelScalar = 1.99999f;
			else if (AngVel.w <= -1.0f) AngularVelScalar = -1.99999f;
			else AngularVelScalar = 2.0f * acos(AngVel.w);
		}
	}

	return;
}

bool RigidBody::IsWithin(const Vector3D<float> &v)
{
	for (int i = 0; i < (int) ColBodies.size(); i++)
		if (ColBodies[i]->IsWithin(v)) return true;

	return false;
}

RigidBody::~RigidBody()
{
	for (int i = 0; i < (int) ColBodies.size(); i++)
		delete ColBodies[i];

	ColBodies.clear();

	return;
}
