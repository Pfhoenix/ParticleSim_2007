// global variables
extern ofstream LogFile;
extern HWND hMainWnd, hToolbox, hSFLToolbox;
extern HINSTANCE hMainInst;
extern HMENU hMainMenu;
extern int Width, Height;
extern D3DRenderArea *RenderArea;
extern PhysX *Physics;
extern Camera CamObj;
extern MT19937A mt;
extern D3DXMATRIX ViewMatrix;
extern D3DXMATRIX ProjMatrix;
extern D3DXMATRIX WorldMatrix;
extern float DeltaTime, LastDeltaTime;
extern bool bUpdateCamera;
extern bool bFreezeEngine, bForceSmallStep;
extern vector<Texture*> Textures;
extern vector<GenericObject*> Objects;
extern vector<ChargedObject*> ChargedObjects;
//extern CSP *Space;
extern bool bEnableCollisions;
extern float MaxLinearSpeed;
extern GenericObject *UnderMouse;
extern GenericObject *HeldObject;
extern POINT LastMouse;
extern NxVec3 Mouse3D, MouseNormal;
extern vector<SHADERPARTICLE> ShaderParticles;
extern bool bCanRenderField;
extern bool bRenderField;
extern bool bRealField;
extern bool bSubFieldLines;
extern int SubFieldLinesMulti;
extern int SubFieldLinesMod;
extern float FieldScale;
extern int FieldType;
extern int PlaceType;
extern int NucleusType;
extern float PosiColor[4];
extern float NegaColor[4];
extern float ForceHColor[4];
extern float ForceVColor[4];
extern map<int,ELEMENTINFO*> Elements;
extern bool bGravity;

// function prototypes
void InitEngine(HWND);
void CleanupEngine(void);
void RenderScreen(void);
void RenderField(void);		// renders a field of electric potential according to the sum of each particle's electrostatic field
void RenderRealField(void);	// renders a total electric field according to the interaction of each source on each other, VERY EXPENSIVE
void TickAll(void);
//void ProcessPhysics(void);
void SaveSettings(void);
void LoadSettings(void);
void Interpolate(Vector3D<float>*, float);
void GetAxes(const D3DXQUATERNION&, Vector3D<float>*, Vector3D<float>*, Vector3D<float>*);
void DeleteObject(GenericObject*);
void BufferShaderParticle(D3DXVECTOR4, float);
int CALLBACK ToolboxWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK ElementsDlgProc(HWND, UINT, WPARAM, LPARAM);
int CALLBACK SFLToolboxWndProc(HWND, UINT, WPARAM, LPARAM);
void ToggleToolbox(void);
void InitElements(void);
void ClearElements(void);
