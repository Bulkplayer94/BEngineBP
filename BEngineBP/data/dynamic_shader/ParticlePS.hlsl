struct PixelInput
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

float4 main(PixelInput input) : SV_Target
{
    return float4(2.5F, 0.0F, 0.0F, 1.0F);
}