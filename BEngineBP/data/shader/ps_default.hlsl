#include "cb_common.hlsli"

float4 main(VertexOutput input) : SV_TARGET
{
    //float3 lightDirection = { 0.0f, -1.0f, 0.0f };
    //const float4 diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    float4 textureColor = volume.Sample(textureSampler, input.uv);
    
    float4 diffuseColor = directionalLight.diffuseColor;
    float3 lightDir = -directionalLight.lightDirection;
    
    float lightIntensity = saturate(dot(input.norm, lightDir));
    
    float4 color = diffuseColor * lightIntensity;
    color = color * textureColor;
    
    return color;   
};