bool IsEnableToUseMaterialTex(float4 texColor)
{
    return texColor.a <= 0.0f;
};


struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};

cbuffer MatBuffer : register(b0)
{
    matrix Mat;
}

cbuffer LightNum : register(b1)
{
    int lightArrayNum;
}

cbuffer LightStatus : register(b2)
{
    float lightRadius;
}

RWStructuredBuffer<float3>LightBuffer:register(u0);

VSOutput VSmain(float4 pos : POSITION, float2 uv : TEXCOORD)
{
	VSOutput op;
	op.svpos = mul(Mat, pos);
	op.uv = uv;
	return op;
}

Texture2D<float4> AlbedoTex : register(t0);
Texture2D<float4> NormalTex : register(t1);
Texture2D<float4> WorldTex : register(t2);
SamplerState smp : register(s0);

float4 PSmain(VSOutput input) : SV_TARGET
{
    float4 albedoColor = AlbedoTex.Sample(smp, input.uv);
    float4 worldNormalVec = NormalTex.Sample(smp, input.uv);
    float4 worldPos = WorldTex.Sample(smp, input.uv);

    //法線マップを使わないならAlbedoを出力する
    if(IsEnableToUseMaterialTex(worldNormalVec))
    {
        return albedoColor;
    }
    
    float3 ambient = float3(0.3f,0.3f,0.3f);
    float3 lightOutput = ambient;
    for(int i = 0; i < lightArrayNum; ++i)
    {
        float3 lightV = LightBuffer[i] - worldPos.xyz;
        float len = length(lightV);
        //距離がlightRadius以上なら計算しない。
        if(lightRadius <= len)
        {
            continue;
        }

        lightOutput += 1.0f - (len / lightRadius);
        lightOutput = clamp(lightOutput,ambient,float3(1.0f,1.0f,1.0f));
    }
    float4 outputColor = float4(albedoColor.xyz * lightOutput,1.0f);
    return outputColor;
}