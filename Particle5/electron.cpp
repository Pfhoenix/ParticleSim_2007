#include "includes.h"

Electron::Electron()
{
	Charge = -5500.0f;
	T = Textures[ELECTRON_TEX];
	Init(1.0f, 8.0f);

	return;
}

Electron::~Electron()
{
	return;
}
