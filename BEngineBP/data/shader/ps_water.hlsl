#include "cb_common.hlsli"

float4 main(VertexOutput input) : SV_TARGET
{
    float4 textureColor = volume.Sample(textureSampler, input.uv);
    
    return textureColor;
}