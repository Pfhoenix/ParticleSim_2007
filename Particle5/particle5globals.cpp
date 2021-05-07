#include "includes.h"

ofstream LogFile("particle5_debug.log", ios::out);
HWND hMainWnd = NULL, hToolbox = NULL, hSFLToolbox = NULL;
HMENU hMainMenu = NULL;
HINSTANCE hMainInst = NULL;
int Width = 640, Height = 480;
D3DRenderArea *RenderArea = NULL;
PhysX *Physics = NULL;
Camera CamObj;
MT19937A mt;
D3DXMATRIX ViewMatrix;
D3DXMATRIX ProjMatrix;
D3DXMATRIX WorldMatrix;
float DeltaTime = 0.0f, LastDeltaTime = -1.0f;
bool bUpdateCamera = true;
bool bFreezeEngine = false, bForceSmallStep = false;
vector<Texture*> Textures;
vector<GenericObject*> Objects;
vector<ChargedObject*> ChargedObjects;
bool bEnableCollisions = false;
float MaxLinearSpeed = 300.0f;
GenericObject *UnderMouse = NULL;
GenericObject *HeldObject = NULL;
POINT LastMouse;
NxVec3 Mouse3D, MouseNormal;
vector<SHADERPARTICLE> ShaderParticles;
bool bCanRenderField = true;
bool bRenderField = true;
bool bRealField = true;
bool bSubFieldLines = false;
int SubFieldLinesMulti = 1000;
int SubFieldLinesMod = 50;
float FieldScale = 1.0f;
int FieldType = 0;
int PlaceType = 0;
int NucleusType = 0;
float PosiColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
float NegaColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
float ForceHColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
float ForceVColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
map<int,ELEMENTINFO*> Elements;
bool bGravity = false;

void InitEngine(HWND hMainWnd)
{
	RenderArea = new D3DRenderArea(hMainWnd);
	if (!RenderArea)
	{
		Log("Unable to create RenderArea!");
		return;
	}

	Physics = new PhysX;
	if (!Physics)
	{
		Log("Unable to create physics!");
		delete RenderArea;
		return;
	}

	Physics->InitSceneBounds();

	// load textures
	Log("Loading textures ...");
	Texture *t;
	if (RenderArea->LoadTexture(".\\tex\\positron.png", &t))
	{
		//if (RenderArea->RenderFx) RenderArea->RenderFx->SetTexture("g_PosiChargeTex", t->d3dbits);
		Textures.push_back(t);
		Log("... positron loaded");
	}
	if (RenderArea->LoadTexture(".\\tex\\electron.png", &t))
	{
		//if (RenderArea->RenderFx) RenderArea->RenderFx->SetTexture("g_NegaChargeTex", t->d3dbits);
		Textures.push_back(t);
		Log("... electron loaded");
	}
	if (RenderArea->LoadTexture(".\\tex\\proton.png", &t))
	{
		Textures.push_back(t);
		Log("... proton loaded");
	}
	if (RenderArea->LoadTexture(".\\tex\\square.png", &t))
	{
		Textures.push_back(t);
		Log("... square loaded");
	}
	if (RenderArea->LoadTexture(".\\tex\\emitter_circle.png", &t))
	{
		Textures.push_back(t);
		Log("... emitter loaded");
	}
	if (RenderArea->LoadTexture(".\\tex\\radiation.png", &t))
	{
		Textures.push_back(t);
		Log("... radiation loaded");
	}

	InitElements();

	return;
}

void CleanupEngine(void)
{
	ChargedObjects.clear();
	Objects.clear();

	//delete Space;
	//Space = NULL;

	for (int i = 0; i < (int) Textures.size(); i++)
		delete Textures[i];

	Textures.clear();

	delete Physics;

	delete RenderArea;

	ClearElements();

	return;
}

void RenderScreen(void)
{
	if (DeltaTime > 0.0f) CamObj.Tick(DeltaTime);

	if (RenderArea->RenderFx) RenderArea->RenderFx->SetMatrix("g_mView", &ViewMatrix);
	if (RenderArea->FieldFx) RenderArea->FieldFx->SetMatrix("g_mView", &ViewMatrix);

	for (int i = 0; i < (int) Objects.size(); i++)
	{
		if (!RenderArea->RenderFx)
		{
			if (HeldObject)
			{
				if (HeldObject == Objects[i]) RenderArea->MainSurface->SetRenderState(D3DRS_TEXTUREFACTOR, 0x5FFFFFFF);
				else RenderArea->MainSurface->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);
			}
			else if (UnderMouse == Objects[i]) RenderArea->MainSurface->SetRenderState(D3DRS_TEXTUREFACTOR, 0x5FFFFFFF);
			else RenderArea->MainSurface->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);
		}
		Objects[i]->Render();
	}

	return;
}

void RenderField(void)
{
	if (!bRenderField) return;

	if (ShaderParticles.size())
	{
		D3DXVECTOR4 Locs[MAX_SHADER_PARTICLES];
		BOOL Pos[MAX_SHADER_PARTICLES];
		float Charge[MAX_SHADER_PARTICLES];
		BOOL On[MAX_SHADER_PARTICLES];
		RenderArea->FieldFx->SetFloat("g_FieldScale", FieldScale);
		RenderArea->FieldFx->BeginPass(0);
		for (int j = 0; j <= (int) ShaderParticles.size() / MAX_SHADER_PARTICLES; j++)
		{
			if (j * MAX_SHADER_PARTICLES >= (int) ShaderParticles.size()) break;
			for (int i = 0; i < MAX_SHADER_PARTICLES; i++)
			{
				if ((j * MAX_SHADER_PARTICLES + i) >= (int) ShaderParticles.size()) On[i] = FALSE;
				else
				{
					Locs[i] = ShaderParticles[j * MAX_SHADER_PARTICLES + i].Location;
					Pos[i] = ShaderParticles[j * MAX_SHADER_PARTICLES + i].bPositive;
					if (bRealField) Charge[i] = ShaderParticles[j * MAX_SHADER_PARTICLES + i].Charge;
					On[i] = TRUE;
				}
			}
			RenderArea->FieldFx->SetVectorArray("g_SourceLocation", Locs, MAX_SHADER_PARTICLES);
			RenderArea->FieldFx->SetBoolArray("g_SourcePositive", Pos, MAX_SHADER_PARTICLES);
			if (bRealField) RenderArea->FieldFx->SetFloatArray("g_SourceCharge", Charge, MAX_SHADER_PARTICLES);
			RenderArea->FieldFx->SetBoolArray("g_SourceOn", On, MAX_SHADER_PARTICLES);
			RenderArea->FieldFx->CommitChanges();
			RenderArea->MainSurface->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		}
		RenderArea->FieldFx->EndPass();
		ShaderParticles.clear();
	}

	return;
}

void TickAll(void)
{
	for (int i = 0; i < (int) Objects.size(); i++)
		Objects[i]->Tick(DeltaTime, LastDeltaTime);

	return;
}

/*void ProcessPhysics(void)
{
	for (int i = 0; i < (int) Objects.size(); i++)
		if (Objects[i]->RB) Objects[i]->RB->ProcessPhysics(DeltaTime, LastDeltaTime);

	return;
}*/

void SaveSettings(void)
{
	return;
}

void LoadSettings(void)
{
	return;
}

void Interpolate(Vector3D<float> *v, float t)
{
	if (t < 0.0f)
	{
		v->SetValue(0.0f, 0.0f, 0.0f);
		return;
	}

	if (t > 1.0f)
	{
		v->SetValue(1.0f, 1.0f, 1.0f);
		return;
	}

	float f = 3 * (t * t) - 2 * (t * t * t);
	v->SetValue(f, f, f);

	return;
}

void GetAxes(const D3DXQUATERNION &q, Vector3D<float> *x, Vector3D<float> *y, Vector3D<float> *z)
{
	D3DXQUATERNION qc;
	D3DXQuaternionConjugate(&qc, &q);
	D3DXQUATERNION qx(1.0f, 0.0f, 0.0f, 0.0f);
	D3DXQUATERNION qy(0.0f, 1.0f, 0.0f, 0.0f);
	//D3DXQUATERNION qz(0.0f, 0.0f, 1.0f, 0.0f);
	qx = qc * qx * q;
	qy = qc * qy * q;
	//qz = q * qz * qc;
	x->SetValue(qx.x, qx.y, qx.z);
	x->SetLength(1.0f);
	y->SetValue(qy.x, qy.y, qy.z);
	y->SetLength(1.0f);
	//z->SetValue(qz.x, qz.y, qz.z);
	//z->SetLength(1.0f);

	return;
}

void DeleteObject(GenericObject *go)
{
	for (int i = 0; i < (int) Objects.size(); i++)
		if (Objects[i] == go)
		{
			vector<GenericObject*>::iterator it = Objects.begin();
			it += i;
			Objects.erase(it);
			break;
		}

	if (ChargedObject *co = dynamic_cast<ChargedObject*>(go))
	{
		for (int i = 0; i < (int) ChargedObjects.size(); i++)
			if (ChargedObjects[i] == co)
			{
				vector<ChargedObject*>::iterator it = ChargedObjects.begin();
				it += i;
				ChargedObjects.erase(it);
				break;
			}
	}

	delete go;

	return;
}

void BufferShaderParticle(D3DXVECTOR4 Loc, float Charge)
{
	if (!bCanRenderField || !bRenderField || !FieldType) return;

	SHADERPARTICLE sp;

	sp.Location = Loc;
	sp.bPositive = (Charge >= 0.0f ? TRUE : FALSE);
	sp.Charge = Charge;
	sp.bOn = TRUE;

	ShaderParticles.push_back(sp);

	return;
}

void ToggleToolbox(void)
{
	if (hToolbox)
	{
		DestroyWindow(hToolbox);
		hToolbox = NULL;
		CheckMenuItem(hMainMenu, ID_VIEW_TOOLBOX, MF_UNCHECKED);
	}
	else
	{
		hToolbox = CreateDialog(hMainInst, "TOOLBOX", hMainWnd, &ToolboxWndProc);
		CheckMenuItem(hMainMenu, ID_VIEW_TOOLBOX, MF_CHECKED);
	}

	return;
}

void InitElements(void)
{
	ELEMENTINFO *ei = NULL;

	Elements.insert(map<int,ELEMENTINFO*>::value_type(1, new ELEMENTINFO(1, 1.01f, "hydrogen")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(2, new ELEMENTINFO(2, 4.0f, "helium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(3, new ELEMENTINFO(3, 6.94f, "lithium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(4, new ELEMENTINFO(4, 9.01f, "beryllium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(5, new ELEMENTINFO(5, 10.81f, "boron")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(6, new ELEMENTINFO(6, 12.01f, "carbon")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(7, new ELEMENTINFO(7, 14.01f, "nitrogen")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(8, new ELEMENTINFO(8, 16.0f, "oxygen")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(9, new ELEMENTINFO(9, 19.0f, "fluorine")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(10, new ELEMENTINFO(10, 20.18f, "neon")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(11, new ELEMENTINFO(11, 22.99f, "sodium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(12, new ELEMENTINFO(12, 24.31f, "magnesium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(13, new ELEMENTINFO(13, 26.98f, "aluminum")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(14, new ELEMENTINFO(14, 28.09f, "silicon")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(15, new ELEMENTINFO(15, 30.97f, "phosphorus")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(16, new ELEMENTINFO(16, 32.07f, "sulfur")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(17, new ELEMENTINFO(17, 35.45f, "chlorine")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(18, new ELEMENTINFO(18, 39.95f, "argon")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(19, new ELEMENTINFO(19, 39.1f, "potassium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(20, new ELEMENTINFO(20, 40.08f, "calcium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(21, new ELEMENTINFO(21, 44.96f, "scandium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(22, new ELEMENTINFO(22, 47.88f, "titanium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(23, new ELEMENTINFO(23, 50.94f, "vanadium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(24, new ELEMENTINFO(24, 52.0f, "chromium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(25, new ELEMENTINFO(25, 54.94f, "manganese")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(26, new ELEMENTINFO(26, 55.85f, "iron")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(27, new ELEMENTINFO(27, 58.93f, "cobalt")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(28, new ELEMENTINFO(28, 58.69f, "nickel")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(29, new ELEMENTINFO(29, 63.46f, "copper")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(30, new ELEMENTINFO(30, 65.39f, "zinc")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(31, new ELEMENTINFO(31, 69.72f, "gallium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(32, new ELEMENTINFO(32, 72.61f, "germanium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(33, new ELEMENTINFO(33, 74.92f, "arsenic")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(34, new ELEMENTINFO(34, 78.96f, "selenium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(35, new ELEMENTINFO(35, 79.9f, "bromine")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(36, new ELEMENTINFO(36, 83.8f, "krypton")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(37, new ELEMENTINFO(37, 85.47f, "rubidium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(38, new ELEMENTINFO(38, 87.62f, "strontium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(39, new ELEMENTINFO(39, 88.91f, "yttrium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(40, new ELEMENTINFO(40, 91.22f, "zirconium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(41, new ELEMENTINFO(41, 92.91f, "niobium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(42, new ELEMENTINFO(42, 95.94f, "molybdenum")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(43, new ELEMENTINFO(43, 98.0f, "technetium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(44, new ELEMENTINFO(44, 101.07f, "ruthenium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(45, new ELEMENTINFO(45, 102.91f, "rhodium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(46, new ELEMENTINFO(46, 106.42f, "palladium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(47, new ELEMENTINFO(47, 107.87f, "silver")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(48, new ELEMENTINFO(48, 112.41f, "cadmium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(49, new ELEMENTINFO(49, 114.82f, "indium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(50, new ELEMENTINFO(50, 118.71f, "tin")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(51, new ELEMENTINFO(51, 121.76f, "antimony")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(52, new ELEMENTINFO(52, 127.6f, "tellurium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(53, new ELEMENTINFO(53, 126.9f, "iodine")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(54, new ELEMENTINFO(54, 131.29f, "xenon")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(55, new ELEMENTINFO(55, 132.91f, "cesium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(56, new ELEMENTINFO(56, 137.33f, "barium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(57, new ELEMENTINFO(57, 138.91f, "lanthanum")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(58, new ELEMENTINFO(58, 140.12f, "cerium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(59, new ELEMENTINFO(59, 140.91f, "praseodymium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(60, new ELEMENTINFO(60, 144.24f, "neodymium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(61, new ELEMENTINFO(61, 145.0f, "promethium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(62, new ELEMENTINFO(62, 150.36f, "samarium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(63, new ELEMENTINFO(63, 151.97f, "europium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(64, new ELEMENTINFO(64, 157.25f, "gadolinium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(65, new ELEMENTINFO(65, 158.93f, "terbium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(66, new ELEMENTINFO(66, 162.5f, "dysprosium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(67, new ELEMENTINFO(67, 164.93f, "holmium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(68, new ELEMENTINFO(68, 167.26f, "erbium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(69, new ELEMENTINFO(69, 168.93f, "thulium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(70, new ELEMENTINFO(70, 173.04f, "ytterbium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(71, new ELEMENTINFO(71, 174.97f, "lutetium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(72, new ELEMENTINFO(72, 178.49f, "hafnium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(73, new ELEMENTINFO(73, 180.95f, "tantalum")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(74, new ELEMENTINFO(74, 183.85f, "tungsten")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(75, new ELEMENTINFO(75, 186.21f, "rhenium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(76, new ELEMENTINFO(76, 190.2f, "osmium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(77, new ELEMENTINFO(77, 192.22f, "iridium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(78, new ELEMENTINFO(78, 195.08f, "platinum")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(79, new ELEMENTINFO(79, 196.97f, "gold")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(80, new ELEMENTINFO(80, 200.59f, "mercury")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(81, new ELEMENTINFO(81, 204.38f, "thallium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(82, new ELEMENTINFO(82, 207.2f, "lead")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(83, new ELEMENTINFO(83, 208.98f, "bismuth")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(84, new ELEMENTINFO(84, 209.0f, "polonium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(85, new ELEMENTINFO(85, 210.0f, "astatine")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(86, new ELEMENTINFO(86, 222.0f, "radon")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(87, new ELEMENTINFO(87, 223.0f, "francium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(88, new ELEMENTINFO(88, 226.03f, "radium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(89, new ELEMENTINFO(89, 227.0f, "actinium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(90, new ELEMENTINFO(90, 232.04f, "thorium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(91, new ELEMENTINFO(91, 231.04f, "protactinium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(92, new ELEMENTINFO(92, 238.03f, "uranium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(93, new ELEMENTINFO(93, 237.05f, "neptunium")));
	Elements.insert(map<int,ELEMENTINFO*>::value_type(94, new ELEMENTINFO(94, 244.0f, "plutonium")));

	return;
}

void ClearElements(void)
{
	for (map<int,ELEMENTINFO*>::iterator it = Elements.begin(); it != Elements.end(); it++)
		delete it->second;

	Elements.clear();

	return;
}
