struct PixelInput
{
    float4 pos : SV_Position;
    float3 uv : TEXCOORD0;
};

SamplerState defaultSampler : register(s0);
Texture2DArray texturesArray : register(t5);

float4 main(PixelInput input) : SV_Target
{
    return texturesArray.Sample(defaultSampler, input.uv);

}