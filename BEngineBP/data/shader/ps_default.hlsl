#include "cb_common.hlsli"

float4 main(VertexOutput input) : SV_TARGET
{
    float4 textureColor = volume.Sample(textureSampler, input.uv);

    float4 colorSum = float4(0.0F, 0.0F, 0.0F, 1.0F);
    
    for (uint i = 0; i < LIGHTS_COUNT; i++)
    {
        float3 lightDir = input.lightPos[i];
        
        float distanceSq = dot(lightDir, lightDir);
        
        float attenuation = 1.0f / (1.0f + 0.2f * distanceSq + 0.02f * distanceSq * distanceSq);
        
        lightDir = normalize(lightDir);
        
        float diffuseIntensity = saturate(dot(input.norm, lightDir));
        
        float lightIntensity = (diffuseIntensity * attenuation) * saturate(pointLights[i].brightness);
        
        colorSum.rgb += pointLights[i].diffuseColor.rgb * lightIntensity;
    }
    
    float4 color = saturate(colorSum) * textureColor;
    return color;
}