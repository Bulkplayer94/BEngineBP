#include "cb_common.hlsli"

float4 main(VertexOutput input) : SV_TARGET
{
    float4 textureColor = volume.Sample(textureSampler, input.uv);

    float4 colorSum = float4(0.0F, 0.0F, 0.0F, 1.0F);
    for (uint i = 0; i < LIGHTS_COUNT; i++)
    {
        float lightIntensity = saturate(dot(input.norm, input.lightPos[i]));
        colorSum.rgb += pointLights[i].diffuseColor.rgb * lightIntensity;
    }
    
    float4 color = saturate(colorSum) * textureColor;
    return color; 
}
