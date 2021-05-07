#include "includes.h"

Emitter::Emitter()
{
	Wavelength = 0.0f;
	Frequency = 0.0f;
	OutputRate = 0.0f;
	Timer = 0.0f;
	bOn = false;

	return;
}

void Emitter::Tick(float DeltaTime, float LastDeltaTime)
{
	return;
}

void Emitter::Render(void)
{
	NxMat34 m = Actor->getGlobalPose();
	NxVec3 v(-13.0f, -13.0f, 0.0f);
	v = m.M * v;
	m.t += v;
	Physics->ConvertMatrix(m, &WorldMatrix);

	Texture *T = Textures[EMITTER_TEX];

	if (RenderArea->RenderFx)
	{
		RenderArea->MainSurface->SetRenderTarget(0, RenderArea->SceneSurface);
		RenderArea->RenderFx->SetMatrix("g_mWorld", &WorldMatrix);
		RenderArea->RenderFx->SetBool("g_bRecalcMx", TRUE);
		RenderArea->RenderFx->SetTexture("g_SpriteTex", T->d3dbits);
		RenderArea->RenderFx->BeginPass(0);
	}
	else
	{
		RenderArea->MainSurface->SetTransform(D3DTS_WORLD, &WorldMatrix);
		RenderArea->MainSurface->SetTexture(0, T->d3dbits);
	}
	RenderArea->MainSurface->SetStreamSource(0, T->d3dVB, 0, sizeof(CUSTOMVERTEX));
	RenderArea->MainSurface->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	if (RenderArea->RenderFx) RenderArea->RenderFx->EndPass();

	return;
}

Emitter::~Emitter()
{
	return;
}
