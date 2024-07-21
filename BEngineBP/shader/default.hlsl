cbuffer constants : register(b0)
{
    float4x4 modelViewProj;
    float4x4 worldMat;
    float4x4 perspMat;
    float4x4 viewMat;
};

cbuffer instancedData : register(b1)
{
    float4x4 modelMat;
};

struct VS_Input
{
    float3 pos : POS;
    float2 uv : TEX;
    float3 norm : NORM;
    unsigned int instanceID : SV_InstanceID;
};

struct VS_Output
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 norm : NORMAL;
};

Texture2D mytexture : register(t0);
SamplerState mysampler : register(s0);
//{
//    Filter = Anisotropic;
//    MaxAnisotropy = 16;
//    AddressU = Wrap;
//    AddressV = Wrap;
//};

VS_Output vs_main(VS_Input input)
{
    VS_Output output;
    float4 worldPos = mul(float4(input.pos, 1.0f), worldMat);
    float4 viewPos = mul(worldPos, viewMat);
    output.pos = mul(viewPos, perspMat);
    output.uv = input.uv;
    output.norm = mul(input.norm, (float3x3)worldMat);
    output.norm = normalize(output.norm);
    return output;
}



float4 ps_main(VS_Output input) : SV_TARGET
{
    float3 lightDirection = {1.0f, 0.0f, 0.0f};
    const float4 diffuseColor = {1.0f, 1.0f, 1.0f, 1.0f};

    float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color;


	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = mytexture.Sample(mysampler, input.uv);

	// Invert the light direction for calculations.
    lightDir = -lightDirection;

    // Calculate the amount of light on this pixel.
	lightIntensity = saturate(dot(input.norm, lightDir));

    // Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
    color = diffuseColor * lightIntensity;

    // Multiply the texture pixel and the final diffuse color to get the final pixel color result.
	color = color * textureColor;

	return color;

	//return mytexture.Sample(mysampler, input.uv);
}