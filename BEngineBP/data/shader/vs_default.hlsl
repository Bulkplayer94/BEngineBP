#include "cb_common.hlsli"

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    float4 worldPos = mul(float4(input.pos, 1.0f), worldMatrix);
    float4 viewPos = mul(worldPos, viewMatrix);
    output.pos = mul(viewPos, perspectiveMatrix);
    
    output.uv = input.uv;
    
    output.norm = mul(input.norm, (float3x3) worldMatrix);
    output.norm = normalize(output.norm);
    
    return output;
}