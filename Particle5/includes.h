#ifndef INCLUDES_H
#define INCLUDES_H


// class prototypes
class Texture;
class MT19937A;
class Vector2D;
class D3DRenderArea;
class PhysX;
class GenericObject;
class ChargedObject;
class Particle;
class Electron;
class Positron;
class Nucleus;
class Magnet;
class Emitter;
class Camera;


// defines
#define Log(x) LogFile << "> " << x << endl
#define szAppName "particle5"
#define PI 3.1415926538f
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ  | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define WM_MOUSEWHEEL 0x020A
#define CAMERA_ROT_LIMIT 1.570796
#define POSITRON_TEX 0
#define ELECTRON_TEX 1
#define PROTON_TEX 2
#define SQUARE_TEX 3
#define EMITTER_TEX 4
#define RADIATION_TEX 5
#define MAX_SHADER_PARTICLES 4

//#define D3D_DEBUG_INFO


// enumerations
enum RB_FIXED_TYPE { RBFT_NONE, RBFT_FROZEN, RBFT_FIXEDSPINNER, RBFT_FREESPINNER, RBFT_MOVER };


// non-class includes
#undef UNICODE
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <commctrl.h>
#include <time.h>
#include <d3dx9.h>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
using namespace std;
#include "vector3d.h"
#include "NxPhysics.h"

typedef struct _customvertex
{
	float x, y, z;
	DWORD color;
	float u, v;
} CUSTOMVERTEX;

typedef struct _shaderparticle
{
	D3DXVECTOR4 Location;
	BOOL bPositive;
	float Charge;
	BOOL bOn;
} SHADERPARTICLE;

typedef struct _elementinfo
{
	int AtomicNumber;
	float AtomicWeight;
	string Name;

	_elementinfo(int an, float aw, string n)
	{
		AtomicNumber = an;
		AtomicWeight = aw;
		Name = n;

		return;
	}
} ELEMENTINFO;

// class includes
#include "texture.h"
#include "mt19937a.h"
#include "vector2d.h"
#include "d3drenderarea.h"
#include "physx.h"
#include "genericobject.h"
#include "chargedobject.h"
#include "particle.h"
#include "electron.h"
#include "positron.h"
#include "nucleus.h"
#include "magnet.h"
#include "emitter.h"
#include "camera.h"
#include "resource.h"
#include "particle5globals.h"


#endif
