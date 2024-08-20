#include "cb_common.hlsli"

VertexOutput main(VertexInput input, uint instancedID : SV_InstanceID)
{
    VertexOutput output;
    float4 worldPos = mul(float4(input.pos, 1.0f), InstancedViewBuffers[instancedID].modelMatrix);
    float4 viewPos = mul(worldPos, viewMatrix);
    output.pos = mul(viewPos, perspectiveMatrix);
    
    output.uv = input.uv;
    
    output.norm = mul(input.norm, (float3x3)InstancedViewBuffers[instancedID].modelMatrix);
    output.norm = normalize(output.norm);
    
    return output;
}