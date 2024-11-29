cbuffer AnimationBuffer : register(b0)
{
    float deltaTime;
    float particleCount;
    float padding[2];
};

struct Particle
{
    float3 position; // Position des Partikels
    float3 velocity; // Geschwindigkeit des Partikels
    float3 gravity;
    float drag; // Gewicht des Partikels
    float lifetime; // Größe des Partikels
    unsigned int textureID;
    bool isDeleted;
};

RWStructuredBuffer<Particle> particleBuffer : register(u0); // Ausgabepuffer (UAV)

[numthreads(512, 1, 1)]
void main(uint3 dispatchID : SV_DispatchThreadID)
{
    uint id = dispatchID.x;
    if (id > particleBuffer.Lenght)
        return;
    
    Particle currParticle = particleBuffer[id];
    if (currParticle.isDeleted == true)
        return;
    
    currParticle.position += currParticle.velocity * deltaTime;
    currParticle.velocity += currParticle.gravity * deltaTime;
    currParticle.velocity *= pow(currParticle.drag, deltaTime);
    currParticle.lifetime -= deltaTime;
    
    if (currParticle.lifetime < 0.0F)
    {
        currParticle.isDeleted = true;
    }
    
    particleBuffer[id] = currParticle;
}
