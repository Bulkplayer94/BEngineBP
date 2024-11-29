/*
struct ParticleComputeStruct {
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 velocity;
			float weight;
			float size;
			unsigned int textureID;
		};
*/

struct Particle
{
    float3 position; // Position des Partikels
    float3 velocity; // Geschwindigkeit des Partikels
    float weight; // Gewicht des Partikels
    float size; // Größe des Partikels
    uint textureID; // ID der Textur
};

StructuredBuffer<Particle> particleBufferRead : register(t0); // Eingabepuffer (SRV)
RWStructuredBuffer<Particle> particleBufferWrite : register(u0); // Ausgabepuffer (UAV)

cbuffer SimulationParameters : register(b0)
{
    float3 gravityBase; // Basis-Gravitationsvektor (z. B. float3(0, 1, 0))
    float deltaTime; // Zeitdifferenz zwischen den Frames
    uint particleBufferSize;
    float3 padding;
}

[numthreads(256, 1, 1)]
void main(uint3 dispatchID : SV_DispatchThreadID)
{
    uint id = dispatchID.x;

    //if (id >= particleBufferSize)
    //    return;

    Particle particle = particleBufferRead[id];

    float3 gravityEffect = particle.weight * gravityBase;

    particle.velocity += gravityEffect * deltaTime;

    particle.position += particle.velocity * deltaTime;

    particle.size *= 0.99f;

    particleBufferWrite[id] = particle;
}
