#ifdef ADEOED_BUILD
	#include "mapedit\includes.h"
#else
	#include "includes.h"
#endif

Texture::Texture(void)
{
	d3dbits = NULL;
	Width = 0;
	Height = 0;
	memset(Vertices, 0, sizeof(Vertices));
	d3dVB = NULL;

	return;
}

void Texture::Clear(void)
{
	if (d3dbits) d3dbits->Release();
	d3dbits = NULL;

	if (d3dVB) d3dVB->Release();
	d3dVB = NULL;

	return;
}

Texture::~Texture(void)
{
	if (d3dbits) d3dbits->Release();
	d3dbits = NULL;

	if (d3dVB) d3dVB->Release();
	d3dVB = NULL;

	return;
}
