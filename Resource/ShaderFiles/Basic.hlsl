struct OutputData
{
    matrix mat;
    float4 color;
};

cbuffer MatrixBuffer : register(b0)
{
    matrix mat;
}
cbuffer colorBuffer : register(b1)
{
    float4 color;
}

struct VSOutput
{
    float4 svpos : SV_POSITION;
};

VSOutput VSmain(float4 pos : POSITION)
{
	VSOutput op;
	op.svpos = mul(mat, pos);
	return op;
}

float4 PSmain(VSOutput input) : SV_TARGET
{
    return color;
}




struct VSOutputWorld
{
    float4 svpos : SV_POSITION;
    float3 worldPos : POSITION;
};

VSOutputWorld GBufferVSmain(float4 pos : POSITION)
{
	VSOutputWorld op;
	op.svpos = mul(mat, pos);
    op.worldPos = float3(0,0,0);
	return op;
}

struct GBufferOutput
{
    float4 albedo : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 metalnessRoughness : SV_TARGET2;
    float4 world : SV_TARGET3;
    float4 emissive : SV_TARGET4;
};
GBufferOutput GBufferPSmain(VSOutputWorld input) : SV_TARGET
{
    GBufferOutput output;
    output.albedo = color;
    output.normal = float4(-1,-1,-1,1);
    output.metalnessRoughness = float4(0,0,0,0);
    output.world = float4(-1,-1,-1,1);
    output.emissive = float4(0,0,0,1);
    return output;
}