#include "cb_water.hlsli"

VertexOutputWater main(VertexInput input, uint instancing : SV_InstanceID)
{
    VertexOutputWater output;
    output.pos = mul(float4(input.pos, 1.0F), InstancedViewBuffers[instancing].modelMatrix);
    output.pos = mul(output.pos, viewMatrix);
    output.pos = mul(output.pos, perspectiveMatrix);
    
    output.uv = input.uv;
    
    output.norm = mul(input.norm, (float3x3) InstancedViewBuffers[instancing].modelMatrix);
    output.norm = normalize(output.norm);
    
    output.height = clamp(input.pos.z / 10.0F, 0.0F, 1.0F);
    
    return output;
}