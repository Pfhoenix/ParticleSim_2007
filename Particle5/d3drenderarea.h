class D3DRenderArea
{
public:

	LPDIRECT3D9 g_pD3D;
	LPDIRECT3DDEVICE9 MainSurface;
	LPDIRECT3DVERTEXBUFFER9 LineVB;
	LPDIRECT3DVERTEXBUFFER9 FieldVB, SceneVB;
	LPDIRECT3DTEXTURE9 FieldTex, SceneTex;
	LPDIRECT3DSURFACE9 FieldSurface, SceneSurface, BackBuffer;
	D3DVIEWPORT9 Viewport;
	LPD3DXFONT Font;
	HWND hWindow;
	int WinWidth, WinHeight;
	vector<Texture*> TextureList;
	LPD3DXEFFECT RenderFx;
	LPD3DXEFFECT FieldFx;

	D3DRenderArea(HWND, int = 0, int = 0);

	HRESULT InitD3D();
	LPDIRECT3DVERTEXBUFFER9 CreateVertexBuffer(CUSTOMVERTEX*, int);
	LPDIRECT3DINDEXBUFFER9 CreateIndexBuffer(unsigned int*, int);
	void SetD3DXMatrix(Vector3D<float>, Vector3D<float>, Vector3D<float>, D3DXQUATERNION);
	//void RenderTexture(Texture*, float, float, float, float, DWORD);
	void RenderLines(CUSTOMVERTEX*, int);
	void RenderLines(LPDIRECT3DVERTEXBUFFER9, int);
	void RenderLine(const Vector3D<float>&, const Vector3D<float>&);
	void RenderPoint(float, float, DWORD);
	void RenderPoints(CUSTOMVERTEX*, int);
//	void RenderMesh(Mesh*);
	void GetTextSize(string, RECT*);
	void RenderText(string, RECT*, DWORD);
	bool LoadTexture(string, Texture**);
	virtual void StartRenderScene(DWORD);
	virtual void EndRenderScene(void);
//	Sprite* FindSprite(string);
//	void ClearSprites(void);
	D3DCOLORVALUE BuildColorValue(float, float, float, float);
	void ScreenToWorld(int, int, NxVec3*, NxVec3*);

	~D3DRenderArea();
};
