
//入力情報
RWTexture2D<float4> BackBuffer : register(u0);

//入力情報
RWTexture2D<float4> Raytracing : register(u1);

//出力先UAV  
RWTexture2D<float4> OutputImg : register(u2);

//色収差用CBV
struct IroShuusaData
{
    float4 m_facter;
};
ConstantBuffer<IroShuusaData> IroShuusa : register(b0);

float EaseInExp(float arg_rate)
{
    return arg_rate == 0.0f ? 0.0f : pow(2.0f, 10.0f * arg_rate - 10.0f);
}
float EaseInQuint(float arg_rate)
{
    return arg_rate * arg_rate * arg_rate * arg_rate * arg_rate;
}

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    //背景色だったらレイトレの結果を描画
    bool isBackGroundColor = BackBuffer[DTid.xy].a <= 0.0f;
    if (isBackGroundColor)
    {
        
        //色収差もかけちゃう。
        float2 center = float2(1280.0f / 2.0f, 720.0f / 2.0f);
        float2 position = float2(DTid.xy);
        float2 dir = normalize(center - position);
        float posLength = length(center - position);
        float lengthRate = posLength / center;
        
        OutputImg[DTid.xy].x = Raytracing[DTid.xy + dir * (EaseInExp(lengthRate) * IroShuusa.m_facter.x)].x;
        OutputImg[DTid.xy].y = Raytracing[DTid.xy + dir * (EaseInExp(lengthRate) * IroShuusa.m_facter.y)].y;
        OutputImg[DTid.xy].z = Raytracing[DTid.xy + dir * (EaseInExp(lengthRate) * IroShuusa.m_facter.z)].z;
        
    }
    else
    {
        float alpha = BackBuffer[DTid.xy].w;
        OutputImg[DTid.xy] = BackBuffer[DTid.xy] + Raytracing[DTid.xy] * (1.0f - alpha);
    
    }
    
}