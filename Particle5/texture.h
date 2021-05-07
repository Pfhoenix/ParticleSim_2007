class Texture
{
public:

	LPDIRECT3DTEXTURE9 d3dbits;
	int Width, Height;
	CUSTOMVERTEX Vertices[4];
	LPDIRECT3DVERTEXBUFFER9 d3dVB;
        
	Texture(void);

	void Clear(void);

	~Texture(void);
};
