float4x4 g_mView;
float4x4 g_mProjection;
float4x4 g_mWVP;
bool g_bRecalcMx;
float g_ViewWidth;
float g_ViewHeight;

texture g_FieldTex;
float g_FieldScale;
float4 g_PosiColor;
float4 g_NegaColor;

float4 g_SourceLocation[4];
bool g_SourcePositive[4];
float g_SourceCharge[4];
bool g_SourceOn[4];

sampler FieldTextureSampler = sampler_state
{
	texture = <g_FieldTex>;
	mipfilter = POINT;
	minfilter = LINEAR;
	magfilter = LINEAR;
};

struct VS_FIELD_OUTPUT
{
	float4 Position : POSITION;
	float2 TextureUV : TEXCOORD0;
	//float4 Color : COLOR0;
};

struct PS_FIELD_OUTPUT
{
	float4 Color : COLOR0;
};

VS_FIELD_OUTPUT BuildFieldVS(float4 Pos : POSITION, float2 Tex : TEXCOORD0)
{
	VS_FIELD_OUTPUT Out;
	
	if (g_bRecalcMx)
	{
		g_mWVP = mul(g_mView, g_mProjection);
		g_bRecalcMx = false;
	}

	Out.Position = mul(Pos, g_mWVP);
	//if (g_bPositive) Out.Color = g_PosiColor;
	//else Out.Color = g_NegaColor;
	Out.TextureUV = Tex;
	
	return Out;
}

PS_FIELD_OUTPUT BuildFieldPS(VS_FIELD_OUTPUT In)
{
	PS_FIELD_OUTPUT Out;
	float4 pos;
	float4 offset;
	float f;
	float4 c;
	int i;
	
	pos.x = (In.TextureUV.x - 0.5f) * g_ViewWidth;
	pos.y = (In.TextureUV.y - 0.5f) * g_ViewHeight;
	pos.z = 0.0f;
	pos.w = 0.0f;
	
	c = tex2D(FieldTextureSampler, In.TextureUV);
	c.b = 0.0f;
	c.a = 0.0f;
	
	for (i = 0; i < 4; i++)
	{
		if (g_SourceOn[i])
		{
			f = 8.0f / distance(g_SourceLocation[i], pos);
			//if (f > 1.0f) f = 1.0f;
			if (g_SourcePositive[i]) c.r += f * g_SourceCharge[i] / 5500.0;
			else c.g += f * g_SourceCharge[i] / -5500.0;;
		}
	}
	
	Out.Color = c;
	
	return Out;
}

VS_FIELD_OUTPUT RenderFieldVS(float4 Pos : POSITION, float2 Tex : TEXCOORD0)
{
	VS_FIELD_OUTPUT Out;
	
	if (g_bRecalcMx)
	{
		g_mWVP = mul(g_mView, g_mProjection);
		g_bRecalcMx = false;
	}
	
	Out.Position = mul(Pos, g_mWVP);
	Out.TextureUV = Tex;

	return Out;
}

PS_FIELD_OUTPUT RenderFieldPS(VS_FIELD_OUTPUT In)
{
	PS_FIELD_OUTPUT Out;
	float4 c;
	
	c = tex2D(FieldTextureSampler, In.TextureUV);
	
	Out.Color = (g_PosiColor * c.r + g_NegaColor * c.g) * g_FieldScale;
	Out.Color.a = 1.0f;
	
	return Out;
}

technique T0
{
	pass P0
	{
		VertexShader = compile vs_2_0 BuildFieldVS();
		PixelShader = compile ps_2_0 BuildFieldPS();
	}
	pass P1
	{
		VertexShader = compile vs_2_0 RenderFieldVS();
		PixelShader = compile ps_2_0 RenderFieldPS();
	}
}
