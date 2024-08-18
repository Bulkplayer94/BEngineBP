#include "cb_water.hlsli"

float4 main(VertexOutputWater input) : SV_TARGET
{
    float4 foamColor = special_1.Sample(textureSampler, input.uv);
    
    float4 textureColor = volume.Sample(textureSampler, input.uv);
    
    float foamIntensity = saturate(input.height);
    if (foamIntensity < 0.5F)
        foamIntensity = 0.0F;
    
    float4 foamResult = foamColor * foamIntensity;
    
    float4 finalColor = lerp(textureColor, foamResult, foamIntensity);
    
    return finalColor;
}

