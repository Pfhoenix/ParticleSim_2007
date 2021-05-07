float4x4 g_mWorld;
float4x4 g_mView;
float4x4 g_mProjection;
float4x4 g_mWVP;
bool g_bRecalcMx;

texture g_SpriteTex;
texture g_SceneTex;

sampler SpriteTextureSampler = sampler_state
{
    texture = <g_SpriteTex>;
    mipfilter = POINT;
    minfilter = LINEAR;
    magfilter = LINEAR;
};

sampler SceneTextureSampler = sampler_state
{
	texture = <g_SceneTex>;
	mipfilter = POINT;
	minfilter = LINEAR;
	magfilter = LINEAR;
};

struct VS_PARTICLE_OUTPUT
{
	float4 Position : POSITION;
	float2 TextureUV : TEXCOORD0;
};

struct PS_OUTPUT
{
	float4 Color : COLOR;
};

float4 g_SourceLocation[4];
bool g_SourcePositive[4];
bool g_SourceOn[4];


VS_PARTICLE_OUTPUT RenderSpriteVS(float4 Pos : POSITION, float2 Tex : TEXCOORD0)
{
	VS_PARTICLE_OUTPUT Output;

	if (g_bRecalcMx)
	{
		g_mWVP = mul(mul(g_mWorld, g_mView), g_mProjection);
		g_bRecalcMx = false;
	}

	// get the actual position
	Output.Position = mul(Pos, g_mWVP);
	// set texture coordinates
	Output.TextureUV = Tex;

	return Output;
}

PS_OUTPUT RenderSpritePS(VS_PARTICLE_OUTPUT In)
{
	PS_OUTPUT Out;
	
	Out.Color = tex2D(SpriteTextureSampler, In.TextureUV);
	
	return Out;
}

PS_OUTPUT RenderSpriteHighlightPS(VS_PARTICLE_OUTPUT In)
{
	PS_OUTPUT Out;
	float a;
	
	Out.Color = tex2D(SpriteTextureSampler, In.TextureUV);
	Out.Color.r += (1.0 - Out.Color.r) * 0.5;
	Out.Color.g += (1.0 - Out.Color.g) * 0.5;
	Out.Color.b += (1.0 - Out.Color.b) * 0.5;
	
	return Out;
}


struct VS_FINAL_OUTPUT
{
	float4 Position : POSITION;
	float2 TextureUV : TEXCOORD0;
};

VS_FINAL_OUTPUT RenderFinalVS(float4 Pos : POSITION, float2 Tex : TEXCOORD0)
{
	VS_FINAL_OUTPUT Out;
	
	if (g_bRecalcMx)
	{
		g_mWVP = mul(g_mView, g_mProjection);
		g_bRecalcMx = false;
	}
	
	Out.Position = mul(Pos, g_mWVP);
	Out.TextureUV = Tex;

	return Out;
}

PS_OUTPUT RenderFinalPS(VS_FINAL_OUTPUT In)
{
	PS_OUTPUT Out;
	
	Out.Color = tex2D(SceneTextureSampler, In.TextureUV);
	
	return Out;
}

technique T0
{
	pass P0
	{
		VertexShader = compile vs_2_0 RenderSpriteVS();
		PixelShader = compile ps_2_0 RenderSpritePS();
	}
	pass P1
	{
		VertexShader = compile vs_2_0 RenderFinalVS();
		PixelShader = compile ps_2_0 RenderFinalPS();
	}
	pass P2
	{
		VertexShader = compile vs_2_0 RenderSpriteVS();
		PixelShader = compile ps_2_0 RenderSpriteHighlightPS();
	}
}
