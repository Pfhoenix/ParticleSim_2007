#include "includes.h"

Magnet::Magnet()
{
	Charge = 5500.0f;

	vector<NxShapeDesc*> shapes;
	NxSphereShapeDesc *sphere = new NxSphereShapeDesc;
	sphere->radius = 8.0f;
	sphere->localPose.t = NxVec3(-13.0f, 0.0f, 0.0f);
	shapes.push_back(sphere);
	sphere = new NxSphereShapeDesc;
	sphere->radius = 8.0f;
	sphere->localPose.t = NxVec3(13.0f, 0.0f, 0.0f);
	shapes.push_back(sphere);
	NxBoxShapeDesc *box = new NxBoxShapeDesc;
	box->dimensions.set(0.0f, 0.0f, 0.0f);
	shapes.push_back(box);
	Actor = Physics->CreateComplexActor(NxVec3(), 3.0f, shapes);
	if (Actor) Actor->userData = (void*)(this);

	return;
}

void Magnet::Render(void)
{
	NxShape *const *shapes = Actor->getShapes();
	if (!shapes) return;

	NxMat34 m;
	NxVec3 v(0.0f, 0.0f, 0.0f);
	D3DXVECTOR4 loc(0.0f, 0.0f, 0.0f, 0.0f);

	Texture *t;

	// bar
	t = Textures[SQUARE_TEX];
	m = shapes[2]->getGlobalPose();
	v.x = -7.0f;
	v.y = -2.0f;
	m.t += m.M * v;
	Physics->ConvertMatrix(m, &WorldMatrix);
	D3DXMATRIX M;
	D3DXMatrixScaling(&M, 3.5f, 1.0f, 1.0f);
	WorldMatrix = M * WorldMatrix;
	if (RenderArea->RenderFx)
	{
		RenderArea->RenderFx->SetMatrix("g_mWorld", &WorldMatrix);
		RenderArea->RenderFx->SetBool("g_bRecalcMx", TRUE);
		RenderArea->RenderFx->SetTexture("g_SpriteTex", t->d3dbits);
		if (UnderMouse == this) RenderArea->RenderFx->BeginPass(2);
		else RenderArea->RenderFx->BeginPass(0);
	}
	else
	{
		RenderArea->MainSurface->SetTransform(D3DTS_WORLD, &WorldMatrix);
		RenderArea->MainSurface->SetTexture(0, t->d3dbits);
	}
	RenderArea->MainSurface->SetStreamSource(0, t->d3dVB, 0, sizeof(CUSTOMVERTEX));
	RenderArea->MainSurface->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	// positive end
	t = Textures[POSITRON_TEX];
	m = shapes[0]->getGlobalPose();
	loc.x = m.t.x;
	loc.y = m.t.y;
	v.x = -8.0f;
	v.y = -8.0f;
	m.t += m.M * v;
	Physics->ConvertMatrix(m, &WorldMatrix);
	if (RenderArea->RenderFx)
	{
		RenderArea->RenderFx->SetMatrix("g_mWorld", &WorldMatrix);
		RenderArea->RenderFx->SetBool("g_bRecalcMx", TRUE);
		RenderArea->RenderFx->SetTexture("g_SpriteTex", t->d3dbits);
		RenderArea->RenderFx->CommitChanges();
	}
	else
	{
		RenderArea->MainSurface->SetTransform(D3DTS_WORLD, &WorldMatrix);
		RenderArea->MainSurface->SetTexture(0, t->d3dbits);
	}
	RenderArea->MainSurface->SetStreamSource(0, t->d3dVB, 0, sizeof(CUSTOMVERTEX));
	RenderArea->MainSurface->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	BufferShaderParticle(loc, Charge);
	// negative end
	t = Textures[ELECTRON_TEX];
	m = shapes[1]->getGlobalPose();
	loc.x = m.t.x;
	loc.y = m.t.y;
	m.t += m.M * v;
	Physics->ConvertMatrix(m, &WorldMatrix);
	if (RenderArea->RenderFx)
	{
		RenderArea->RenderFx->SetMatrix("g_mWorld", &WorldMatrix);
		RenderArea->RenderFx->SetBool("g_bRecalcMx", TRUE);
		RenderArea->RenderFx->SetTexture("g_SpriteTex", t->d3dbits);
		RenderArea->RenderFx->CommitChanges();
		//RenderArea->RenderFx->BeginPass(0);
	}
	else
	{
		RenderArea->MainSurface->SetTransform(D3DTS_WORLD, &WorldMatrix);
		RenderArea->MainSurface->SetTexture(0, t->d3dbits);
	}
	RenderArea->MainSurface->SetStreamSource(0, t->d3dVB, 0, sizeof(CUSTOMVERTEX));
	RenderArea->MainSurface->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	BufferShaderParticle(loc, -Charge);

	if (RenderArea->RenderFx) RenderArea->RenderFx->EndPass();

	return;
}

void Magnet::GetAffectedBy(const NxVec3 &source, float sc)
{
	NxShape *const *shapes = Actor->getShapes();
	if (!shapes) return;

	SingularAffectedBy(shapes[0]->getGlobalPosition(), source, Charge, sc);
	SingularAffectedBy(shapes[1]->getGlobalPosition(), source, -Charge, sc);

	return;
}

void Magnet::AffectOther(ChargedObject *co)
{
	if (!co) return;

	NxShape *const *shapes = Actor->getShapes();
	if (!shapes) return;

	co->GetAffectedBy(shapes[0]->getGlobalPosition(), Charge);
	co->GetAffectedBy(shapes[1]->getGlobalPosition(), -Charge);

	return;
}

Magnet::~Magnet()
{
	return;
}
