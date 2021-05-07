#include "includes.h"

Particle::Particle()
{
	Radius = 0.0f;
	T = NULL;

	return;
}

void Particle::Init(float D, float r)
{
	Radius = r;
	Actor = Physics->CreateSphereActor(NxVec3(), r, D);
	if (!Actor) return;
	Actor->userData = (void*)(this);

	return;
}

void Particle::Render(void)
{
	NxMat34 m = Actor->getGlobalPose();
	NxVec3 v(-Radius, -Radius, 0.0f);
	D3DXVECTOR4 loc(m.t.x, m.t.y, 0.0f, 0.0f);
	v = m.M * v;
	m.t += v;
	Physics->ConvertMatrix(m, &WorldMatrix);

	if (RenderArea->RenderFx)
	{
		RenderArea->MainSurface->SetRenderTarget(0, RenderArea->SceneSurface);
		RenderArea->RenderFx->SetMatrix("g_mWorld", &WorldMatrix);
		RenderArea->RenderFx->SetBool("g_bRecalcMx", TRUE);
		RenderArea->RenderFx->SetTexture("g_SpriteTex", T->d3dbits);
		if (UnderMouse == this) RenderArea->RenderFx->BeginPass(2);
		else RenderArea->RenderFx->BeginPass(0);
	}
	else
	{
		RenderArea->MainSurface->SetTransform(D3DTS_WORLD, &WorldMatrix);
		RenderArea->MainSurface->SetTexture(0, T->d3dbits);
	}
	RenderArea->MainSurface->SetStreamSource(0, T->d3dVB, 0, sizeof(CUSTOMVERTEX));
	RenderArea->MainSurface->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	if (RenderArea->RenderFx) RenderArea->RenderFx->EndPass();
	BufferShaderParticle(loc, Charge);

	return;
}

void Particle::AffectOther(ChargedObject *co)
{
	if (!co) return;

	co->GetAffectedBy(Actor->getGlobalPosition(), Charge);

	return;
}

Particle::~Particle()
{
	return;
}
