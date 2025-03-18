#include "MatrixLib.hlsli"

cbuffer AnimationBuffer : register(b0)
{
    float4 camPos;
    float deltaTime;
    float particleCount;
    float2 padding;
};

struct Particle
{
    float3 position;
    float3 velocity;
    float3 gravity;
    float drag;
    float lifetime;
    unsigned int textureID;
    bool isDeleted;
};
RWStructuredBuffer<Particle> particleBuffer : register(u0);

struct ParticleInstance
{
    float4x4 worldMatrix;
};
RWStructuredBuffer<ParticleInstance> particleInstanceBuffer : register(u1);

[numthreads(1024, 1, 1)]
void main(int3 dispatchID : SV_DispatchThreadID)
{
    int id = ((dispatchID.y - 1) * 512) + dispatchID.x;
    if (id >= particleCount)
        return;

    Particle currParticle = particleBuffer[id];
    
    float decay = pow(currParticle.drag, deltaTime);
    //currParticle.position += currParticle.velocity * deltaTime;
    currParticle.velocity += currParticle.gravity * deltaTime;
    currParticle.velocity *= decay;
    currParticle.lifetime -= deltaTime;

    if (currParticle.lifetime < 0.0F)
    {
        currParticle.isDeleted = true;
    }
    
    particleBuffer[id] = currParticle;

    float3 camPosition = camPos.xyz;
    float3 lookDir = normalize(currParticle.position - camPosition);
    float pitch = asin(lookDir.y);
    float yaw = atan2(lookDir.x, lookDir.z);

    float4x4 rotationX = RotationMatrixX(-pitch);
    float4x4 rotationY = RotationMatrixY(yaw);
    float4x4 rotationMat = mul(rotationX, rotationY);

    float4x4 scalingMat = ScalingMatrix(float3(3, 3, 3));
    float4x4 transMat = TranslateMatrix(currParticle.position);

    float4x4 worldMatrix = mul(scalingMat, rotationMat);
    worldMatrix = mul(worldMatrix, transMat);
    
    ParticleInstance instance;
    instance.worldMatrix = worldMatrix;
    particleInstanceBuffer[id] = instance;
}
