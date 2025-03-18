float4x4 RotationMatrixX(float r)
{
    float c = cos(r);
    float s = sin(r);
    
    return float4x4(
        1, 0, 0, 0,
        0, c, s, 0,
        0, -s, c, 0,
        0, 0, 0, 1
    );
}

float4x4 RotationMatrixY(float r)
{
    float c = cos(r);
    float s = sin(r);
    
    return float4x4(
        c, 0, -s, 0,
        0, 1, 0, 0,
        s, 0, c, 0,
        0, 0, 0, 1
    );
}

float4x4 RotationMatrixZ(float r)
{
    float c = cos(r);
    float s = sin(r);
    
    return float4x4(
        c, s, 0, 0,
        -s, c, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
}

float4x4 TranslateMatrix(float3 p)
{
    float x = p.x;
    float y = p.y;
    float z = p.z;
    
    return float4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        x, y, z, 1
    );
}

float4x4 ScalingMatrix(float3 f)
{
    float x = f.x;
    float y = f.y;
    float z = f.z;
    
    return float4x4(
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1
    );
}