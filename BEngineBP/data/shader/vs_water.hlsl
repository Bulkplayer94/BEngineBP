#include "cb_common.hlsli"

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.pos = mul(float4(input.pos, 1.0F), worldMatrix);
    output.pos = mul(output.pos, viewMatrix);
    output.pos = mul(output.pos, perspectiveMatrix);
    
    output.uv = input.uv;
    
    output.norm = mul(input.norm, (float3x3) worldMatrix);
    output.norm = normalize(output.norm);
    
    return output;
}