#include "includes.h"

GenericObject::GenericObject()
{
	//RB = NULL;
	bUserHeld = false;
	Actor = NULL;

	Objects.push_back(this);

	return;
}

void GenericObject::Init(float f)
{
	return;
}

void GenericObject::Tick(float DeltaTime, float LastDeltaTime)
{
	return;
}

void GenericObject::Render(void)
{
	return;
}

GenericObject::~GenericObject()
{
	Physics->gScene->releaseActor(*Actor);
	Actor = NULL;

	return;
}
