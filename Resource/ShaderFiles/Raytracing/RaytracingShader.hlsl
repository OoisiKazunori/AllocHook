#include "RaytracingShaderHeader.hlsli"

//RayGenerationシェーダー
[shader("raygeneration")]
void mainRayGen()
{

    //現在のレイのインデックス。左上基準のスクリーン座標として使える。
    uint2 launchIndex = DispatchRaysIndex().xy;
    
    //カメラから見たレイの方向を計算。
    float2 dims = float2(DispatchRaysDimensions().xy);
    float2 d = (launchIndex.xy + 0.5f) / dims.xy * 2.0f - 1.0f;
    float aspect = dims.x / dims.y;
    float4 target = mul(cameraEyePos.m_projMat, float4(d.x, -d.y, 1, 1));
    float3 dir = mul(cameraEyePos.m_viewMat, float4(target.xyz, 0)).xyz;
    
    //GBufferから値を抜き取る。
    float4 albedoColor = albedoMap[launchIndex];
    float4 normalColor = normalMap[launchIndex];
    float4 materialInfo = materialMap[launchIndex];
    float4 worldColor = worldMap[launchIndex];
    float4 emissiveColor = emissiveMap[launchIndex];
    
    //法線が-1,-1,-1だったらパーティクルなので処理を飛ばす。
    if (normalColor.r <= -0.9f && normalColor.g <= -0.9f && normalColor.b <= -0.9f)
    {

        finalColor[launchIndex.xy] = albedoColor;
        emissiveTexture[launchIndex.xy] = emissiveColor;
        lensFlareTexture[launchIndex.xy] = float4(0, 0, 0, 0);
        return;
        
    }
    
    //遠さを見る。 ある程度離れている位置では反射の計算を切る
    const float REFLECTION_DEADLINE = 100.0f;
    bool isFar = REFLECTION_DEADLINE < length(cameraEyePos.m_eye - worldColor.xyz);
    
    //ライティングパスを行う。
    float bright = 0.0f;
    float pointlightBright = 0.0f;
    //発光しているオブジェクトは発光具合を明るさにする。
    if (0 < length(emissiveColor.xyz))
    {
        bright += length(emissiveColor.xyz);
    }
    //反射屈折するオブジェクトのライティングは切る。
    else if (MATERIAL_REFLECT == materialInfo.y || MATERIAL_REFRACT == materialInfo.y)
    {
        bright = 1.0f;
    }
    //ライティングを行う。
    else
    {
        LightingPass(bright, pointlightBright, worldColor, normalColor, lightData, launchIndex, gRtScene, isFar);
        
        
    }
    
    //通常ライティング
    albedoColor.xyz *= clamp(bright, 0.3f, 1.0f);
    
    
    //マテリアルのIDをもとに、反射屈折のレイを飛ばす。
    float4 final = float4(0, 0, 0, 1);
    //SecondaryPass(dir, emissiveColor, worldColor, materialInfo, normalColor, albedoColor, gRtScene, cameraEyePos, final);
    final = albedoColor;
    
    //float4 reflectColor = float4(0, 0, 0, 0);
    //if (materialInfo.a == 1)
    //{
        
    //    //レイの方向を決める。
    //    float3 cameraDir = normalize(worldColor.xyz - cameraEyePos.m_eye);
    //    float3 reflectDir = reflect(cameraDir, normalColor.xyz);
        
    //    //レイを打つ。
    //    Payload reflectPayload;
    //    reflectPayload.m_color = float3(0, 0, 0);
    //    reflectPayload.m_rayID = 0;
    //    CastRay(reflectPayload, worldColor.xyz, reflectDir, 1000, MISS_CHECKHIT, RAY_FLAG_NONE, gRtScene, 0xFF);
        
    //    if (reflectPayload.m_color.x != -1.0f)
    //    {
            
    //        final.xyz = reflectPayload.m_color;
            
    //    }
        
    //}
    
    //合成の結果を入れる。
    finalColor[launchIndex.xy] = albedoColor;
    emissiveTexture[launchIndex.xy] = emissiveColor;
    lensFlareTexture[launchIndex.xy] = emissiveColor / 8.0f;
  
}

//missシェーダー レイがヒットしなかった時に呼ばれるシェーダー
[shader("miss")]
void mainMS(inout Payload PayloadData)
{
 
    float3 mieColor = float3(0, 0, 0);
    PayloadData.m_color = AtmosphericScattering(WorldRayDirection() * 15000.0f, mieColor);

}

//シャドウ用missシェーダー
[shader("miss")]
void shadowMS(inout Payload payload)
{
    
    //このシェーダーに到達していたら影用のレイがオブジェクトに当たっていないということなので、payload.m_color.x(影情報)に白を入れる。
    payload.m_color = float3(1, 1, 1);

}

//当たり判定チェック用missシェーダー 
[shader("miss")]
void checkHitRayMS(inout Payload payload)
{
    
    payload.m_color = float3(-1, -1, -1);

}


//closesthitシェーダー レイがヒットした時に呼ばれるシェーダー
[shader("closesthit")]

    void mainCHS
    (inout
    Payload payload, MyAttribute
    attrib)
{
    
    //ここにレイが当たった地点の頂点データとかが入っている。
    Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer);
    
    //当たった位置のピクセルをサンプリングして、色をPayloadに入れる。
    float4 mainTexColor = objectTexture.SampleLevel(smp, vtx.uv, 0);
    payload.m_color = mainTexColor.xyz;
    
    uint instanceID = InstanceID();
    
    //rayidが1なら影用のライト
    if (payload.m_rayID == 1)
    {
        
        payload.m_color = float3(0.0f, 0.0f, 0.0f);
        
        //if (instanceID == 10)
        //{
            
        //    payload.m_color = float3(1.0f, 1.0f, 1.0f);
            
        //}
        
    }
    //rayidが1ならプレイヤーの影用のライト
    else if (payload.m_rayID == 3)
    {
        
        if (instanceID != 10)
        {
            
            payload.m_color = float3(1.0f, 1.0f, 1.0f);
            
        }
        
        
    }
    //rayidが2なら敵の反射屈折
    else if (payload.m_rayID == 2)
    {
        
        //反射先のライティングを行う。
        float bright = 0;
        float pointlightBright = 0;
        const float REFLECTION_DEADLINE = 10000.0f;
        bool isFar = REFLECTION_DEADLINE < length(cameraEyePos.m_eye - vtx.pos.xyz);
        LightingPass(bright, pointlightBright, float4(WorldRayOrigin(), 1.0f), float4(vtx.normal, 1.0f), lightData, DispatchRaysIndex(), gRtScene, isFar);
        payload.m_color *= clamp(bright, 0.3f, 1.0f);
    
        //当たったオブジェクトのInstanceIDが1だったら(GPUパーティクルだったら)輝度を保存する。
        if (instanceID == 1)
        {
            payload.m_emissive = payload.m_color;
        }
        else
        {
            payload.m_emissive = payload.m_color / 3.0f;

        }
        
    }
    //それ以外は通常の反射
    else
    {
    
        //当たったオブジェクトのInstanceIDが1だったら(GPUパーティクルだったら)輝度を保存する。
        uint instanceID = InstanceID();
        if (instanceID == 1)
        {
            payload.m_emissive = payload.m_color;
        }
        else
        {
            //反射先のライティングを行う。
            float bright = 0;
            float pointlightBright = 0;
            LightingPass(bright, pointlightBright, float4(vtx.pos, 1.0f), float4(vtx.normal, 1.0f), lightData, DispatchRaysIndex(), gRtScene, false);
            
            //夜のときのディレクショナルライトのY 絶対値
            const float NIGHT_DIRLIGHT_Y = 0.4472f;
            //昼のときのディレクショナルライトのY 絶対値
            const float DAY_DIRLIGHT_Y = 0.894f;
            const float DIRLIGHT_Y_CHANGE_AMOUNT = abs(DAY_DIRLIGHT_Y - NIGHT_DIRLIGHT_Y);
            
            //夜を基準とした時の現在の昼の割合
            float dayRate = (abs(lightData.m_dirLight.m_dir.y) - NIGHT_DIRLIGHT_Y) / DIRLIGHT_Y_CHANGE_AMOUNT;
    
            //ポイントライトの色
            float3 pointLightColor = float3(0.93f, 0.67f, 0.64f) * (pointlightBright);
    
            //ライトの明るさが0だったら影の色をアルベドに設定。
            float3 nonLightAlbedo = payload.m_color.xyz;
            const float3 NIGHT_LIGHT_COLOR = float3(0.18f, 0.30f, 0.42f);
            const float3 DAY_GROUND_COLOR = float3(0.45f, 0.60f, 0.44f);
            //カスの色
            if (abs(nonLightAlbedo.x - 0.24f) <= 0.2f && vtx.pos.y < 1.0f)
            {
        
                const float3 NIGHT_SHADOW_COLOR = float3(0.10f, 0.12f, 0.18f);
                const float3 DAY_SHADOW_COLOR = float3(0.24f, 0.15f, 0.17f);
                payload.m_color.xyz = DAY_SHADOW_COLOR * dayRate + NIGHT_SHADOW_COLOR * (1.0f - dayRate);
                nonLightAlbedo = payload.m_color.xyz;
        
            }
            else if (bright <= 0.0f)
            {
        
        
                const float3 NIGHT_SHADOW_COLOR = float3(0.10f, 0.12f, 0.18f);
                const float3 DAY_SHADOW_COLOR = float3(0.24f, 0.15f, 0.17f);
                payload.m_color.xyz = DAY_SHADOW_COLOR * dayRate + NIGHT_SHADOW_COLOR * (1.0f - dayRate);
        
        
            }
            else
            {
        
              //アルベドにライトの色をかける。
              float3 lightColor = float3(1, 1, 1) * dayRate + NIGHT_LIGHT_COLOR * (1.0f - dayRate);
             payload.m_color.xyz *= lightColor * clamp(bright, 0.0f, 1.0f);
            
            }
    
            //ポイントライトの明るさを足す。
            if (0 < pointlightBright)
            {
        
                payload.m_color.xyz += nonLightAlbedo * pointLightColor;
        
            }

        }
    }
           
}

//AnyHitShader
[shader("anyhit")]

    void mainAnyHit
    (inout
    Payload payload, MyAttribute
    attrib)
{
        
    Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer);
    
    if (payload.m_rayID == 1 && InstanceID() == 10)
    {
        IgnoreHit();

    }
    
}