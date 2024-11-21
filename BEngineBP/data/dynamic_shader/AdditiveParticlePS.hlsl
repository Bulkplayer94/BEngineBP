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
    //return gradientTexture.Sample(gradientSampler, input.uvCoords).rgba * input.color.rgba;
    return float4(1.0F, 1.0F, 1.0F, 1.0F);
}