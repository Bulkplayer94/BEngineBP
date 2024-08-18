#include "cb_common.hlsli"

float4 main(VertexOutput input) : SV_TARGET
{
    float4 textureColor = volume.Sample(textureSampler, input.uv);
    
    float4 diffuseColor = directionalLight.diffuseColor;
    float3 lightDir = -directionalLight.lightDirection;
    
    float lightIntensity = max(0.005F, saturate(dot(input.norm, lightDir)));
    
    float4 color = diffuseColor * lightIntensity;
    color = color * textureColor;
    
    return color;   
};