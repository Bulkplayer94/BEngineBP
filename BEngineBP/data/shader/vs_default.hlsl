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
    
    for (int i = 0; i < LIGHTS_COUNT; i++)
    {
        // Determine the light positions based on the position of the lights and the position of the vertex in the world.
        output.lightPos[i] = pointLights[i].position.xyz - worldPos.xyz;

        // Normalize the light position vectors.
        output.lightPos[i] = normalize(output.lightPos[i]);
    }
    
    output.worldPos = worldPos;
    
    return output;
}