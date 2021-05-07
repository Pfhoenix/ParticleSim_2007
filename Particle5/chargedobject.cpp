#include "includes.h"

ChargedObject::ChargedObject()
{
	Charge = 0.0f;

	ChargedObjects.push_back(this);

	return;
}

void ChargedObject::Tick(float DeltaTime, float LastDeltaTime)
{
	if (bFreezeEngine) return;

	for (int i = 0; i < (int) ChargedObjects.size(); i++)
	{
		if (ChargedObjects[i] == this) continue;
		AffectOther(ChargedObjects[i]);
	}

	return;
}

void ChargedObject::SingularAffectedBy(const NxVec3 &loc, const NxVec3 &source, float c, float sc)
{
	float d, f;
	NxVec3 v;

	d = loc.distanceSquared(source);
	// try to prevent super crazy fast motion that results from the objects being too close
	if (d < 0.00001f) d = 0.00001f;
	f = (c * sc) / d;
	v = loc - source;
	v.setMagnitude(1.0f);
	v *= f;
	Actor->addForceAtPos(v, loc, NX_SMOOTH_IMPULSE);
	//RB->QueueForce(loc, v.Normal() * f);

	return;
}

void ChargedObject::GetAffectedBy(const NxVec3 &source, float sc)
{
	SingularAffectedBy(Actor->getGlobalPosition(), source, Charge, sc);

	return;
}

void ChargedObject::AffectOther(ChargedObject *CO)
{
	return;
}

ChargedObject::~ChargedObject()
{
	return;
}
