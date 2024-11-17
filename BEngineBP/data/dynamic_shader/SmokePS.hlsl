struct PSInput
{
    float4 position : SV_Position;
    float2 texCoords : TEXCOORD0;
    float4 color : COLOR0;
};

SamplerState gradientSampler : register(s0);
Texture2D gradientTexture : register(t0);

float4 main(PSInput input) : SV_Target
{
    return gradientTexture.Sample(gradientSampler, input.texCoords);
}