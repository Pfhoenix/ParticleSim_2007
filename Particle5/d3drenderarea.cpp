#include "includes.h"

D3DRenderArea::D3DRenderArea(HWND hW, int w, int h)
{
	hWindow = hW;

	if (!w || !h)
	{
		RECT r;
		GetClientRect(hWindow, &r);

		WinWidth = r.right - r.left;
		WinHeight = r.bottom - r.top;
	}
	else
	{
		WinWidth = w;
		WinHeight = h;
	}

	RenderFx = NULL;
	FieldFx = NULL;
	FieldVB = NULL;
	FieldTex = NULL;
	FieldSurface = NULL;
	SceneVB = NULL;
	SceneTex = NULL;
	SceneSurface = NULL;
	BackBuffer = NULL;

	if (S_OK != InitD3D())
	{
		hWindow = NULL;
		Log("D3D failed to initialize!");
	}

	return;
}

HRESULT D3DRenderArea::InitD3D()
{
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION))) return E_FAIL;

	D3DPRESENT_PARAMETERS d3dpp;
	memset(&d3dpp, 0, sizeof(d3dpp));

	d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &MainSurface)))
	{
		if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &MainSurface)))
			return E_FAIL;
	}

	D3DCAPS9 caps;
	MainSurface->GetDeviceCaps(&caps);

	MainSurface->GetViewport(&Viewport);

	Log("Viewport dimensions : " << Viewport.Width << "," << Viewport.Height);

	MainSurface->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	MainSurface->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	MainSurface->SetFVF(D3DFVF_CUSTOMVERTEX);
	MainSurface->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	MainSurface->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	MainSurface->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	MainSurface->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	MainSurface->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_BLENDFACTORALPHA);
	MainSurface->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	MainSurface->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
	MainSurface->SetRenderState(D3DRS_LIGHTING, FALSE);

	if ((caps.NumSimultaneousRTs > 1) && (caps.VertexShaderVersion >= D3DVS_VERSION(2, 0)))
	{
		LPD3DXBUFFER OutputBuffer = NULL;
		D3DXCreateEffectFromFile(MainSurface, "shaders\\render20.fx", NULL, NULL, 0, NULL, &RenderFx, &OutputBuffer);
		if (OutputBuffer)
		{
			Log((char*) OutputBuffer->GetBufferPointer());
			OutputBuffer->Release();
			OutputBuffer = NULL;
		}
		else { Log("object shader loaded successfully"); }
		if (caps.VertexShaderVersion >= D3DVS_VERSION(3, 0))
		{
			D3DXCreateEffectFromFile(MainSurface, "shaders\\field30.fx", NULL, NULL, 0, NULL, &FieldFx, &OutputBuffer);
			if (OutputBuffer)
			{
				Log((char*) OutputBuffer->GetBufferPointer());
				OutputBuffer->Release();
				OutputBuffer = NULL;
				Log("device reported SM3, but shader failed, defaulting to SM2 version");
				bRealField = false;
				// try shader model 2
				D3DXCreateEffectFromFile(MainSurface, "shaders\\field20.fx", NULL, NULL, 0, NULL, &FieldFx, &OutputBuffer);
				if (OutputBuffer)
				{
					Log((char*) OutputBuffer->GetBufferPointer());
					OutputBuffer->Release();
					OutputBuffer = NULL;
					Log("electric field shader (sm2) failed");
					bCanRenderField = false;
					bRenderField = false;
				}
				else
				{
					Log("electric field shader (sm2) loaded successfully");
					bCanRenderField = true;
					bRenderField = true;
				}
			}
			else
			{
				bCanRenderField = true;
				bRenderField = true;
				bRealField = true;
				Log("electric field shader (sm3) loaded successfully");
			}
		}
		else
		{
			bRealField = false;
			D3DXCreateEffectFromFile(MainSurface, "shaders\\field20.fx", NULL, NULL, 0, NULL, &FieldFx, &OutputBuffer);
			if (OutputBuffer)
			{
				Log((char*) OutputBuffer->GetBufferPointer());
				OutputBuffer->Release();
				OutputBuffer = NULL;
				Log("electric field shader (sm2) failed");
				bCanRenderField = false;
				bRenderField = false;
			}
			else
			{
				Log("electric field shader (sm2) loaded successfully");
				bCanRenderField = true;
				bRenderField = true;
			}
		}
	}
	else
	{
		bCanRenderField = false;
		bRenderField = false;
		bRealField = false;
	}

	D3DXMATRIX Identity;

	D3DXMatrixOrthoLH(&ProjMatrix, (float) WinWidth, (float) -WinHeight, 1.0f, -1.0f);
	D3DXMatrixIdentity(&Identity);

	if (RenderFx)
	{
		RenderFx->SetMatrix("g_mProjection", &ProjMatrix);
		if (FieldFx)
		{
			FieldFx->SetMatrix("g_mProjection", &ProjMatrix);
			FieldFx->SetFloat("g_ViewWidth", (float) Viewport.Width);
			FieldFx->SetFloat("g_ViewHeight", (float) Viewport.Height);
		}
	}
	else
	{
		MainSurface->SetTransform(D3DTS_PROJECTION, &ProjMatrix);
		MainSurface->SetTransform(D3DTS_WORLD, &Identity);
		MainSurface->SetTransform(D3DTS_VIEW, &Identity);
		SetD3DXMatrix(Vector3D<float>(), Vector3D<float>(1.0f, 1.0f, 1.0f), Vector3D<float>(), D3DXQUATERNION(0.0f, 0.0f, 0.0f, 0.0f));
	}

	CUSTOMVERTEX linecv[2];
	for (int i = 0; i < 2; i++)
	{
		linecv[i].x = (float) (i % 2);
		linecv[i].y = (float) (i % 2);
		linecv[i].z = (float) (i % 2);
		linecv[i].u = 0.0f;
		linecv[i].v = 0.0f;
	}

	LineVB = CreateVertexBuffer(linecv, 2);

/*	D3DXCreateFont(MainSurface, 32, 16, 400, 0, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, 0, DEFAULT_QUALITY, "Lucida Console", &Font);
	Font->PreloadCharacters(0, 255);*/
	Font = NULL;

	if (RenderFx)
	{
		float x = Viewport.Width * 0.5f;
		float y = Viewport.Height * 0.5f;

		CUSTOMVERTEX fieldcv[4];
		fieldcv[0].x = -x-0.5f;
		fieldcv[0].y = -y-0.5f;
		fieldcv[0].z = 0.0f;
		fieldcv[0].color = 0xffffffff;
		fieldcv[0].u = 0.0f;
		fieldcv[0].v = 0.0f;
		fieldcv[1].x = x-0.5f;
		fieldcv[1].y = -y-0.5f;
		fieldcv[1].z = 0.0f;
		fieldcv[1].color = 0xffffffff;
		fieldcv[1].u = 1.0f;
		fieldcv[1].v = 0.0f;
		fieldcv[2].x = -x-0.5f;
		fieldcv[2].y = y-0.5f;
		fieldcv[2].z = 0.0f;
		fieldcv[2].color = 0xffffffff;
		fieldcv[2].u = 0.0f;
		fieldcv[2].v = 1.0f;
		fieldcv[3].x = x-0.5f;
		fieldcv[3].y = y-0.5f;
		fieldcv[3].z = 0.0f;
		fieldcv[3].color = 0xffffffff;
		fieldcv[3].u = 1.0f;
		fieldcv[3].v = 1.0f;
		FieldVB = CreateVertexBuffer(fieldcv, 4);
		fieldcv[0].z = 1.0f;
		fieldcv[1].z = 1.0f;
		fieldcv[2].z = 1.0f;
		fieldcv[3].z = 1.0f;
		SceneVB = CreateVertexBuffer(fieldcv, 4);

		if (FieldFx)
		{
			D3DXCreateTexture(MainSurface, Viewport.Width, Viewport.Height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &FieldTex);
			if (FieldTex)
			{
				FieldFx->SetTexture("g_FieldTex", FieldTex);
				FieldTex->GetSurfaceLevel(0, &FieldSurface);
				FieldFx->SetFloatArray("g_PosiColor", PosiColor, 4);
				FieldFx->SetFloatArray("g_NegaColor", NegaColor, 4);
				if (bRealField)
				{
					FieldFx->SetFloatArray("g_FieldHColor", ForceHColor, 4);
					FieldFx->SetFloatArray("g_FieldVColor", ForceVColor, 4);
				}
				Log("Field texture created succesfully");
				bRenderField = true;
			}
			else
			{
				FieldFx->Release();
				FieldFx = NULL;
				bRenderField = false;
				Log("Field texture didn't create!");
			}
		}

		D3DXCreateTexture(MainSurface, Viewport.Width, Viewport.Height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &SceneTex);
		if (SceneTex)
		{
			SceneTex->GetSurfaceLevel(0, &SceneSurface);
			RenderFx->SetTexture("g_SceneTex", SceneTex);
			Log("Scene texture created successfully");
		}
		else
		{
			FieldFx->Release();
			FieldFx = NULL;
			FieldTex->Release();
			FieldTex = NULL;
			RenderFx->Release();
			RenderFx = NULL;
			bRenderField = false;
			Log("Scene texture didn't create!");
		}
	}

	return S_OK;
}

LPDIRECT3DVERTEXBUFFER9 D3DRenderArea::CreateVertexBuffer(CUSTOMVERTEX *vertices, int num)
{
	if (!vertices) return NULL;

	LPDIRECT3DVERTEXBUFFER9 vb;

	if (FAILED(MainSurface->CreateVertexBuffer(num * sizeof(CUSTOMVERTEX), D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX, D3DPOOL_MANAGED, &vb, NULL)))
		return NULL;

    void* pVertices = NULL;

	if (FAILED(vb->Lock(0, sizeof(CUSTOMVERTEX) * num, (void**) &pVertices, 0))) return NULL;
	memcpy(pVertices, vertices, num * sizeof(CUSTOMVERTEX));
	vb->Unlock();

	return vb;
}

LPDIRECT3DINDEXBUFFER9 D3DRenderArea::CreateIndexBuffer(unsigned int *index, int num)
{
	if (!index) return NULL;

	LPDIRECT3DINDEXBUFFER9 ib = NULL;

	if (FAILED(MainSurface->CreateIndexBuffer(num * sizeof(unsigned int), 0, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &ib, NULL))) return NULL;

	void *pIndices = NULL;

	if (FAILED(ib->Lock(0, sizeof(unsigned int) * num, (void**) &pIndices, 0))) return NULL;
	memcpy(pIndices, index, num * sizeof(unsigned int));
	ib->Unlock();

	return ib;
}

void D3DRenderArea::SetD3DXMatrix(Vector3D<float> Trans, Vector3D<float> Scale, Vector3D<float> RotLoc, D3DXQUATERNION rot)
{
	//D3DXMATRIX M;
	D3DXVECTOR3 v(Trans.X, Trans.Y, Trans.Z);
	D3DXVECTOR3 s(Scale.X, Scale.Y, Scale.Z);
	D3DXVECTOR3 r(RotLoc.X, RotLoc.Y, RotLoc.Z);
	//D3DXQUATERNION q;
	//D3DXQuaternionRotationYawPitchRoll(&q, rot.X, rot.Y, rot.Z);
	D3DXMatrixTransformation(&WorldMatrix, NULL, NULL, &s, &r, &rot, &v);
	MainSurface->SetTransform(D3DTS_WORLD, &WorldMatrix);

	return;
}

bool D3DRenderArea::LoadTexture(string p, Texture **tex)
{
	(*tex) = new Texture;
	HRESULT h = D3DXCreateTextureFromFileEx(MainSurface, p.c_str(), 0, 0, 0, 0, D3DFMT_A8B8G8R8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0xFF000000, NULL, NULL, &((*tex)->d3dbits));
	if (h != D3D_OK)
	{
		delete (*tex);
		(*tex) = NULL;

		return false;
	}

	D3DSURFACE_DESC d3dsd;
	(*tex)->d3dbits->GetLevelDesc(0, &d3dsd);
	(*tex)->Width = d3dsd.Width;
	(*tex)->Height = d3dsd.Height;
	(*tex)->Vertices[0].x = 0.0f;
	(*tex)->Vertices[0].y = 0.0f;
	(*tex)->Vertices[0].z = 1.0f;
	(*tex)->Vertices[0].color = 0xffffffff;
	(*tex)->Vertices[0].u = 0.0f;
	(*tex)->Vertices[0].v = 0.0f;
	(*tex)->Vertices[1].x = (float) d3dsd.Width;
	(*tex)->Vertices[1].y = 0.0f;
	(*tex)->Vertices[1].z = 1.0f;
	(*tex)->Vertices[1].color = 0xffffffff;
	(*tex)->Vertices[1].u = 1.0f;
	(*tex)->Vertices[1].v = 0.0f;
	(*tex)->Vertices[2].x = 0.0f;
	(*tex)->Vertices[2].y = (float) d3dsd.Height;
	(*tex)->Vertices[2].z = 1.0f;
	(*tex)->Vertices[2].color = 0xffffffff;
	(*tex)->Vertices[2].u = 0.0f;
	(*tex)->Vertices[2].v = 1.0f;
	(*tex)->Vertices[3].x = (float) d3dsd.Width;
	(*tex)->Vertices[3].y = (float) d3dsd.Height;
	(*tex)->Vertices[3].z = 1.0f;
	(*tex)->Vertices[3].color = 0xffffffff;
	(*tex)->Vertices[3].u = 1.0f;
	(*tex)->Vertices[3].v = 1.0f;
	(*tex)->d3dVB = CreateVertexBuffer((*tex)->Vertices, 4);

	return true;
}

/*void D3DRenderArea::RenderTexture(Texture *T, float X, float Y, float SX, float SY, DWORD color)
{
	if (T == NULL) return;

	MainSurface->SetRenderState(D3DRS_TEXTUREFACTOR, color);
	MainSurface->SetTexture(0, T->d3dbits);
	MainSurface->SetStreamSource(0, T->d3dVB, 0, sizeof(CUSTOMVERTEX));

	SetD3DXMatrix(X, Y, SX, SY);
	MainSurface->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	return;
}*/

void D3DRenderArea::RenderLines(CUSTOMVERTEX *Lines, int num)
{
	LPDIRECT3DVERTEXBUFFER9 vb = CreateVertexBuffer(Lines, num * 2);

	MainSurface->SetStreamSource(0, vb, 0, sizeof(CUSTOMVERTEX));
	MainSurface->DrawPrimitive(D3DPT_LINELIST, 0, num);
	vb->Release();

	return;
}

void D3DRenderArea::RenderLines(LPDIRECT3DVERTEXBUFFER9 vb, int num)
{
	MainSurface->SetStreamSource(0, vb, 0, sizeof(CUSTOMVERTEX));
	MainSurface->DrawPrimitive(D3DPT_LINELIST, 0, num);

	return;
}

void D3DRenderArea::RenderLine(const Vector3D<float> &start, const Vector3D<float> &offset)
{
	MainSurface->SetStreamSource(0, LineVB, 0, sizeof(CUSTOMVERTEX));
	SetD3DXMatrix(start, offset, Vector3D<float>(0.0f), D3DXQUATERNION());
	MainSurface->DrawPrimitive(D3DPT_LINELIST, 0, 1);

	return;
}

void D3DRenderArea::RenderPoint(float X, float Y, DWORD Color)
{
	CUSTOMVERTEX vert;

	memset(&vert, 0, sizeof(vert));

	vert.x = X;
	vert.y = Y;
	vert.z = 1.0f;
//	vert.color = Color;

	LPDIRECT3DVERTEXBUFFER9 vb = CreateVertexBuffer(&vert, 1);

	//MainSurface->SetTexture(0, NULL);
	MainSurface->SetStreamSource(0, vb, 0, sizeof(CUSTOMVERTEX));
	//SetD3DXMatrix(1.0f, 1.0f, 1.0f, 1.0f);
	MainSurface->DrawPrimitive(D3DPT_POINTLIST, 0, 1);
	vb->Release();

	return;
}

void D3DRenderArea::RenderPoints(CUSTOMVERTEX *Points, int Num)
{
	LPDIRECT3DVERTEXBUFFER9 vb = CreateVertexBuffer(Points, Num);
	//unsigned int i = vb->Length;

	//MainSurface->SetTexture(0, NULL);
	MainSurface->SetStreamSource(0, vb, 0, sizeof(CUSTOMVERTEX));
	//MainSurface->SetFVF(D3DFVF_CUSTOMVERTEX);
	//SetD3DXMatrix(0.0f, 0.0f, 1.0f, 1.0f);
	MainSurface->DrawPrimitive(D3DPT_POINTLIST, 0, Num);
	vb->Release();

	return;
}

/*void D3DRenderArea::RenderMesh(Mesh *M)
{
	if (!M->VB || !M->IB)
	{
		if (!M->VB) { Log("VertexBuffer for mesh is NULL"); }
		if (!M->IB) { Log("IndexBuffer for mesh is NULL"); }
		return;
	}

	MainSurface->SetStreamSource(0, M->VB, 0, sizeof(CUSTOMVERTEX));
	MainSurface->SetIndices(M->IB);
	MainSurface->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, M->NumIndices, 0, M->NumIndices / 3);

	return;
}*/

void D3DRenderArea::GetTextSize(string s, RECT *r)
{
	Font->DrawText(NULL, s.c_str(), -1, r, DT_CALCRECT | DT_LEFT | DT_NOCLIP, 0);

	return;
}

void D3DRenderArea::RenderText(string s, RECT *r, DWORD color)
{
	Font->DrawText(NULL, s.c_str(), -1, r, DT_CENTER, color);

	return;
}

void D3DRenderArea::StartRenderScene(DWORD color)
{
	if (SceneTex)
	{
		MainSurface->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);
		MainSurface->GetRenderTarget(0, &BackBuffer);
		if (FieldType)
		{
			MainSurface->SetRenderTarget(0, FieldSurface);
			MainSurface->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);
		}
		MainSurface->SetRenderTarget(0, SceneSurface);
		RenderFx->SetTechnique("T0");
		RenderFx->Begin(NULL, 0);
	}
	MainSurface->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
	MainSurface->BeginScene();

	return;
}

void D3DRenderArea::EndRenderScene(void)
{
	if (RenderFx && SceneTex)
	{
		//MainSurface->EndScene();
		//MainSurface->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		RenderFx->End();
		//MainSurface->BeginScene();
		// either we render the field first or skip to rendering the scene alone
		if (FieldType)
		{
			MainSurface->SetRenderTarget(0, FieldSurface);
			MainSurface->SetStreamSource(0, FieldVB, 0, sizeof(CUSTOMVERTEX));
			FieldFx->SetBool("g_bRecalcMx", TRUE);
			if (FieldType == IDC_FIELD_REAL) FieldFx->SetTechnique("T1");
			else if (FieldType == IDC_FIELD_LINES) FieldFx->SetTechnique("T2");
			else
			{
				FieldFx->SetBool("g_bSubFieldLines", (BOOL) bSubFieldLines);
				FieldFx->SetInt("g_SFL_Multiplier", SubFieldLinesMulti);
				FieldFx->SetInt("g_SFL_Modulus", SubFieldLinesMod);
				FieldFx->SetTechnique("T0");
			}
			FieldFx->Begin(NULL, 0);
			RenderField();
			MainSurface->SetRenderTarget(0, BackBuffer);
			FieldFx->BeginPass(1);
			MainSurface->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
			FieldFx->EndPass();
			FieldFx->End();
		}
		else MainSurface->SetRenderTarget(0, BackBuffer);
		RenderFx->SetBool("g_bRecalcMx", TRUE);
		RenderFx->Begin(NULL, 0);
		RenderFx->BeginPass(1);
		MainSurface->SetStreamSource(0, SceneVB, 0, sizeof(CUSTOMVERTEX));
		MainSurface->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		RenderFx->EndPass();
		RenderFx->End();
	}
	MainSurface->EndScene();
	MainSurface->Present(NULL, NULL, NULL, NULL);

	return;
}

/*Sprite* D3DRenderArea::FindSprite(string s)
{
	for (int i = 0; i < (int) SpriteList.size(); i++)
		if (SpriteList[i]->Name == s) return SpriteList[i];

	return NULL;
}

void D3DRenderArea::ClearSprites(void)
{
	for (int i = 0; i < (int) SpriteList.size(); i++)
	{
		SpriteList[i]->ClearFrames(true);
		delete SpriteList[i];
	}

	SpriteList.clear();

	return;
}*/

D3DCOLORVALUE D3DRenderArea::BuildColorValue(float r, float g, float b, float a)
{
	D3DCOLORVALUE c;

	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;

	return c;
}

void D3DRenderArea::ScreenToWorld(int sx, int sy, NxVec3 *loc, NxVec3 *norm)
{
	D3DXVECTOR3 v, l, s;
	D3DXMATRIX Identity;

	s.x = (float) sx;
	s.y = (float) sy;
	s.z = 1.0f;

	D3DXMatrixIdentity(&Identity);
	D3DXMatrixScaling(&Identity, CamObj.ScaleVec.X, CamObj.ScaleVec.Y, CamObj.ScaleVec.Z);
	D3DXVec3Unproject(&l, &s, &Viewport, &ProjMatrix, &ViewMatrix, &Identity);
	s.z = 0.0f;
	D3DXVec3Unproject(&v, &s, &Viewport, &ProjMatrix, &ViewMatrix, &Identity);
	loc->set(l.x, l.y, l.z);
	// fix really tiny values
	if ((loc->x < 0.00001) && (loc->x > 0)) loc->x = 0.0f;
	if ((loc->x > -0.00001) && (loc->x < 0)) loc->x = 0.0f;
	if ((loc->y < 0.00001) && (loc->y > 0)) loc->y = 0.0f;
	if ((loc->y > -0.00001) && (loc->y < 0)) loc->y = 0.0f;
	// because we're restricted to 2d, this needs to be ensured at 0
	//loc->z = 0.0f;
	//if ((loc->Z < 0.00001) && (loc->Z > 0)) loc->Z = 0.0f;
	//if ((loc->Z > -0.00001) && (loc->Z < 0)) loc->Z = 0.0f;
	*norm = NxVec3(v.x, v.y, v.z) - (*loc);
	norm->setMagnitude(1.0f);
	// because we're restricted to 2d, X and Y will always be 0
	//if ((norm->X < 0.00001) && (norm->X > 0)) norm->X = 0.0f;
	//if ((norm->X > -0.00001) && (norm->X < 0)) norm->X = 0.0f;
	//if ((norm->Y < 0.00001) && (norm->Y > 0)) norm->Y = 0.0f;
	//if ((norm->Y > -0.00001) && (norm->Y < 0)) norm->Y = 0.0f;
	if ((norm->z < 0.00001) && (norm->z > 0)) norm->z = 0.0f;
	if ((norm->z > -0.00001) && (norm->z < 0)) norm->z = 0.0f;

	return;
}

D3DRenderArea::~D3DRenderArea()
{
	if (Font) Font->Release();
	Font = NULL;

	if (LineVB) LineVB->Release();
	LineVB = NULL;

	if (FieldVB) FieldVB->Release();
	FieldVB = NULL;

	if (SceneVB) SceneVB->Release();
	SceneVB = NULL;

	if (FieldSurface) FieldSurface->Release();
	FieldSurface = NULL;

	if (SceneSurface) SceneSurface->Release();
	SceneSurface = NULL;

	if (FieldTex) FieldTex->Release();
	FieldTex = NULL;

	if (SceneTex) SceneTex->Release();
	SceneTex = NULL;

	if (RenderFx) RenderFx->Release();
	RenderFx = NULL;

	if (MainSurface) MainSurface->Release();
	MainSurface = NULL;

	if (g_pD3D) g_pD3D->Release();
	g_pD3D = NULL;

	return;
}
