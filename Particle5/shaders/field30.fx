float4x4 g_mView;
float4x4 g_mProjection;
float4x4 g_mWVP;
bool g_bRecalcMx;
float g_ViewWidth;
float g_ViewHeight;
float4 g_Xaxis;
float4 g_Yaxis;

texture g_FieldTex;
float4 g_PosiColor;
float4 g_NegaColor;
float g_FieldScale;
float4 g_FieldHColor;
float4 g_FieldVColor;
bool g_bSubFieldLines;
int g_SFL_Multiplier;
int g_SFL_Modulus;

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
		g_Xaxis.x = 1.0f;
		g_Xaxis.y = 0.0f;
		g_Xaxis.z = 0.0f;
		g_Xaxis.w = 0.0f;
		g_Yaxis.x = 0.0f;
		g_Yaxis.y = 1.0f;
		g_Yaxis.z = 0.0f;
		g_Yaxis.w = 0.0f;
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
	float f;
	float4 c;
	int i;
	
	pos.x = (In.TextureUV.x - 0.5f) * g_ViewWidth;
	pos.y = (In.TextureUV.y - 0.5f) * g_ViewHeight;
	pos.z = 0.0f;
	pos.w = 0.0f;
	
	c = tex2D(FieldTextureSampler, In.TextureUV);
	
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
		g_Xaxis.x = 1.0f;
		g_Xaxis.y = 0.0f;
		g_Xaxis.z = 0.0f;
		g_Xaxis.w = 1.0f;
		g_Yaxis.x = 0.0f;
		g_Yaxis.y = 1.0f;
		g_Yaxis.z = 0.0f;
		g_Yaxis.w = 1.0f;
	}
	
	Out.Position = mul(Pos, g_mWVP);
	Out.TextureUV = Tex;

	return Out;
}

PS_FIELD_OUTPUT RenderFieldPS(VS_FIELD_OUTPUT In)
{
	PS_FIELD_OUTPUT Out;
	float4 c;
	int f;
	
	c = tex2D(FieldTextureSampler, In.TextureUV);
	
	if (!g_bSubFieldLines)
	{
		if (c.r - c.g > 0) Out.Color = g_PosiColor * (c.r - c.g) * g_FieldScale;
		else if (c.g - c.r > 0) Out.Color = g_NegaColor * (c.g - c.r) * g_FieldScale;
	}
	else
	{
		if (c.r - c.g >= 0)
        	{
			f = (int) ((c.r - c.g) * g_SFL_Multiplier);
			if (f % g_SFL_Modulus == 0) Out.Color = g_PosiColor * (c.r - c.g) * g_FieldScale;
			else Out.Color = 0.0f;
		}
		else if (c.g - c.r > 0)
		{
			f = (int) ((c.g - c.r) * g_SFL_Multiplier);
			if (f % g_SFL_Modulus == 0) Out.Color = g_NegaColor * (c.g - c.r) * g_FieldScale;
			else Out.Color = 0.0f;
		}
	}
	
	Out.Color.a = 1.0f;
	
	return Out;
}

PS_FIELD_OUTPUT BuildBetterFieldPS(VS_FIELD_OUTPUT In)
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
	c.a = 1.0f;
	
	for (i = 0; i < 4; i++)
	{
		if (g_SourceOn[i])
		{
			offset = g_SourceLocation[i] - pos;
			f = g_SourceCharge[i] / length(offset) / 500.0f;
			offset = normalize(offset);
			c.r += f * offset.x;
			c.g += f * offset.y;
		}
	}
	
	Out.Color = c;
	
	return Out;
}

PS_FIELD_OUTPUT RenderBetterFieldPS(VS_FIELD_OUTPUT In)
{
	PS_FIELD_OUTPUT Out;
	float4 c;
	
	c = tex2D(FieldTextureSampler, In.TextureUV);

	//c.r = abs(c.r) * g_FieldScale;
	//c.g = abs(c.g) * g_FieldScale;
	//c.a = 1.0f;
	//Out.Color = c;
	Out.Color = (abs(c.r) * g_FieldHColor + abs(c.g) * g_FieldVColor) * g_FieldScale;
	Out.Color.a = 1.0f;
	
	return Out;
}

PS_FIELD_OUTPUT RenderFieldLinesPS(VS_FIELD_OUTPUT In)
{
	PS_FIELD_OUTPUT Out;
	float4 c, nc;
	float4 x;
	float r, g;

	c = tex2D(FieldTextureSampler, In.TextureUV);
	c.a = 0.0f;
	if (any(c))
	{
		nc = normalize(c);
		r = sin(nc.g * 3.141529f * 4.0f) * 0.5f + 0.5f;
		g = sin(nc.r * 3.141529f * 4.0f) * 0.5f + 0.5f;
		Out.Color = abs((c.r * g_FieldHColor * r + c.g * g_FieldVColor * g)) * g_FieldScale;
		Out.Color.a = 1.0f;
	}
	else
	{
		Out.Color = c;
		Out.Color.a = 1.0f;
	}

	return Out;
}

technique T0
{
	pass P0
	{
		VertexShader = compile vs_3_0 BuildFieldVS();
		PixelShader = compile ps_3_0 BuildFieldPS();
	}
	pass P1
	{
		VertexShader = compile vs_3_0 RenderFieldVS();
		PixelShader = compile ps_3_0 RenderFieldPS();
	}
}

technique T1
{
	pass P0
	{
		VertexShader = compile vs_3_0 BuildFieldVS();
		PixelShader = compile ps_3_0 BuildBetterFieldPS();
	}
	pass P1
	{
		VertexShader = compile vs_3_0 RenderFieldVS();
		PixelShader = compile ps_3_0 RenderBetterFieldPS();
	}
}

technique T2
{
	pass P0
	{
		VertexShader = compile vs_3_0 BuildFieldVS();
		PixelShader = compile ps_3_0 BuildBetterFieldPS();
	}
	pass P1
	{
		VertexShader = compile vs_3_0 RenderFieldVS();
		PixelShader = compile ps_3_0 RenderFieldLinesPS();
	}
}