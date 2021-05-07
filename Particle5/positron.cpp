#include "includes.h"

Positron::Positron()
{
	Charge = 5500.0f;
	T = Textures[POSITRON_TEX];
	Init(1.0f, 8.0f);

	return;
}

Positron::~Positron()
{
	return;
}
