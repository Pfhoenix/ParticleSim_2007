#include "includes.h"

Nucleus::Nucleus()
{
	AtomicNumber = 0;
	Element = "none";
	return;
}

Nucleus::Nucleus(int an)
{
	AtomicNumber = an;
	// need to do an element lookup here
	Element = Elements[an]->Name;
	Charge = 5500.0f * an;
	T = Textures[PROTON_TEX];
//	Log("nucleus initting");
	Init(Elements[an]->AtomicWeight * 1846.0f, 16.0f);

	return;
}

Nucleus::~Nucleus()
{
	return;
}
