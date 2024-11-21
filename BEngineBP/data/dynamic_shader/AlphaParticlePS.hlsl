struct PSInput
{
    float4 position : SV_Position;
    float2 uvCoords : TEXCOORD0;
    float4 color : COLOR0;
};

Texture2D gradientTexture : register(t0);
SamplerState gradientSampler : register(s0);

float4 main(PSInput input) : SV_Target
{
    return float4(gradientTexture.Sample(gradientSampler, input.uvCoords).rgb * input.color.rgb, 1.0F);
}