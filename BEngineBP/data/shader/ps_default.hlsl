#include "cb_common.hlsli"

float4 main(VertexOutput input) : SV_TARGET
{
    return volume.Sample(textureSampler, input.uv);
}