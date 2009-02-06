/*-----------------------------------------------------------------------------

effects.fx
----------

Diese Datei enthält alle effekte, die mit Hilfe Der DxGraphics-Engine mgölich
sind. Diese Dateui wird bei bedarf erweitert um neue Effekte zu ermöglichen.

Autor: Peter Friedland

-----------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
//Uniforms
//-----------------------------------------------------------------------------

#ifdef EFFECT_2D_STD

float4x4 g_mat2DScaleRotTrans; //Transormation für ein 2D-Quad
float4               g_vColor; //Manipulierbare Farbe
float4               g_vPivot; //Pivot

#endif

#ifdef EFFECT_2D_ANIM

float4      g_vTexTranslation; //texturtransformation

#endif

#ifdef EFFECT_3D_STD

float4x4   g_matWorldViewProj; //Matrix Für Transformation + Projektion

#endif

#ifdef EFFECT_3D_DIFFUSE

float4x4           g_matWorld; //Worldmatrix für Normalen
float4            g_vLightDir=float4(1.0f,-1.0f,1.0f,1.0f); //Lichtrichtung

#endif

//Textur stufe 0 immer
Texture          g_texDiffuse; //textur Layer 0

//-----------------------------------------------------------------------------
//Vertex Shader Struct - In/Out
//-----------------------------------------------------------------------------

struct SVertexShaderIn
{
	float4 vPosition : POSITION0; //Position
	float2 vTexCoord : TEXCOORD0; //Texturkoordinaten
	#ifdef EFFECT_3D_DIFFUSE
	float3 vNormal   : NORMAL0;   //Normale Für licht
	#endif
};

struct SVertexShaderOut
{
	float4 vPosition : POSITION0; //Position
	float2 vTexCoord : TEXCOORD0; //Texturkoordinaten
	#ifdef EFFECT_3D_DIFFUSE
	float3 vNormal   : TEXCOORD1; //Normale Für licht
	#endif
};

//-----------------------------------------------------------------------------
//Pixel Shader Struct - In
//-----------------------------------------------------------------------------

struct SPixelShaderIn
{
	float4 vPosition : POSITION0; //Position
	float2 vTexCoord : TEXCOORD0; //Texturkoordinaten
	#ifdef EFFECT_3D_DIFFUSE
	float3 vNormal   : TEXCOORD1;   //Normale Für licht
	#endif
};

//-----------------------------------------------------------------------------
//Vertex Shader Function
//-----------------------------------------------------------------------------

SVertexShaderOut VertexShaderMain(SVertexShaderIn stInput)
{
	SVertexShaderOut stOutput;

	#ifdef EFFECT_2D_STD
	stOutput.vPosition = mul(stInput.vPosition-g_vPivot,g_mat2DScaleRotTrans);
	#endif
	
	#ifdef EFFECT_2D_STD
	stOutput.vTexCoord = stInput.vTexCoord;
	#endif
	
	#ifdef EFFECT_2D_ANIM
	stOutput.vTexCoord = stOutput.vTexCoord + g_vTexTranslation.xy;
	#endif

	#ifdef EFFECT_3D_STD
	stOutput.vPosition = mul(stInput.vPosition,g_matWorldViewProj);
	stOutput.vTexCoord = stInput.vTexCoord;
	#endif

	#ifdef EFFECT_3D_DIFFUSE
	stOutput.vNormal = mul(stInput.vNormal,g_matWorld);
	#endif

	return stOutput;
}

//-----------------------------------------------------------------------------
//Sampler
//-----------------------------------------------------------------------------

sampler g_sampDiffuse = sampler_state
{
	texture = <g_texDiffuse>;
	MAGFILTER = LINEAR;
	MINFILTER = LINEAR;
	MIPFILTER = LINEAR;
};

//-----------------------------------------------------------------------------
//Pixel Shader Function
//-----------------------------------------------------------------------------

float4 PixelShaderMain(SPixelShaderIn stInput) : COLOR0
{
	#ifdef EFFECT_2D_STD
	return tex2D(g_sampDiffuse,stInput.vTexCoord) * g_vColor;
	#endif
	
	#ifdef EFFECT_3D_DIFFUSE
	float fDiffuse = saturate(dot(normalize(stInput.vNormal),-g_vLightDir));
	#endif
	
	#ifdef EFFECT_3D_STD
		
	#ifdef EFFECT_3D_DIFFUSE
		return tex2D(g_sampDiffuse,stInput.vTexCoord) * fDiffuse;
	#else
		return tex2D(g_sampDiffuse,stInput.vTexCoord);
	#endif

	#endif
}

technique MainTechnique
{
	pass p0
	{
		CullMode = None;
		VertexShader = compile vs_2_0 VertexShaderMain();
		PixelShader  = compile ps_2_0 PixelShaderMain();
	}
}