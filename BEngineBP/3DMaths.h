#pragma once

#define _USE_MATH_DEFINES 
#include <math.h>
#include <PxPhysicsAPI.h>
#pragma warning(push)
#pragma warning(disable:4201) // anonymous struct warning

struct float2 {
    float x, y;
};

struct float3
{
    float x, y, z;
};

union float4
{
    struct {
        float x, y, z, w;
    };
    struct {
        float3 xyz;
    };
};
#pragma warning(pop)

struct float3x3
{
    // We internally do 3x4 for HLSL-friendly alignment
    float m[3][4];
};

union float4x4
{
    float m[4][4];
    float4 cols[4];
    float colsT[16];

    inline float4 row(int i) { // Returns i-th row of matrix
        return { m[0][i], m[1][i], m[2][i], m[3][i] };
    }

};

struct float4x3
{
    float m[4][3];
};

inline float degreesToRadians(float degs) {
    return degs * ((float)M_PI / 180.0f);
}

inline float length(float3 v) {
    return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

inline float length(float4 v) {
    return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z +v.w*v.w);
}

inline float dot(float4 a, float4 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

inline float distance(float3 v1, float3 v2) {
    return (v1.x - v2.x) + (v1.y - v2.y) + (v1.z - v2.z);
}

inline float euclideanDistance(float3 v1, float3 v2) {
    return sqrtf(abs(distance(v1, v2)));
}

inline float3 operator* (float3 v, float f) {
    return {v.x*f, v.y*f, v.z*f};
}

inline float4 operator* (float4 v, float f) {
    return {v.x*f, v.y*f, v.z*f, v.w*f};
}

inline float3 normalise(float3 v) {
    return v * (1.f / length(v));
}

inline float4 normalise(float4 v) {
    return v * (1.f / length(v));
}

inline float3 cross(float3 a, float3 b) {
    return {
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    };
}

inline float3 operator+= (float3 &lhs, float3 rhs) {
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    return lhs;
}

inline float3 operator-= (float3 &lhs, float3 rhs) {
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
    return lhs;
}

inline float3 operator- (float3 v) {
    return {-v.x, -v.y, -v.z};
}

inline float4x4 scaleMat(float s) {
    return {
        s, 0, 0, 0,
        0, s, 0, 0,
        0, 0, s, 0,
        0, 0, 0, 1
    };
}

inline float4x4 rotateXMat(float rad) {
    float sinTheta = sinf(rad);
    float cosTheta = cosf(rad);
    return {
        1, 0, 0, 0,
        0, cosTheta, -sinTheta, 0,
        0, sinTheta, cosTheta, 0,
        0, 0, 0, 1
    };
}

inline float4x4 rotateYMat(float rad) {
    float sinTheta = sinf(rad);
    float cosTheta = cosf(rad);
    return {
        cosTheta, 0, sinTheta, 0,
        0, 1, 0, 0,
        -sinTheta, 0, cosTheta, 0,
        0, 0, 0, 1
    };
}

inline float4x4 rotateZMat(float rad) {
    float sinTheta = sinf(rad);
    float cosTheta = cosf(rad);
    return {
        cosTheta, -sinTheta, 0, 0,
        sinTheta, cosTheta, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
}

inline float4x4 rotateXYZMat(float radX, float radY, float radZ) {
    float sinX = sinf(radX);
    float cosX = cosf(radX);
    float sinY = sinf(radY);
    float cosY = cosf(radY);
    float sinZ = sinf(radZ);
    float cosZ = cosf(radZ);

    float cosYcosX = cosY * cosX;
    float cosYsinX = cosY * sinX;
    float sinYcosX = sinY * cosX;
    float sinYsinX = sinY * sinX;

    return {
        cosYcosX * cosZ - sinYsinX * sinZ, -cosYcosX * sinZ - sinYsinX * cosZ, sinYcosX, 0,
        cosYsinX * cosZ + sinYcosX * sinZ, -cosYsinX * sinZ + sinYcosX * cosZ, -sinYsinX, 0,
        -sinY * cosZ, sinY * sinZ, cosY, 0,
        0, 0, 0, 1
    };
}

inline float4x4 translationMat(float3 trans) {
    return {
        1, 0, 0, trans.x,
        0, 1, 0, trans.y,
        0, 0, 1, trans.z,
        0, 0, 0, 1
    };
}

inline float4x4 makePerspectiveMat(float aspectRatio, float fovYRadians, float zNear, float zFar)
{
    // float yScale = 1 / tanf(0.5f * fovYRadians); 
    // NOTE: 1/tan(X) = tan(90degs - X), so we can avoid a divide
    // float yScale = tanf((0.5f * M_PI) - (0.5f * fovYRadians));
    float yScale = tanf(0.5f * ((float)M_PI - fovYRadians));
    float xScale = yScale / aspectRatio;
    float zRangeInverse = 1.f / (zNear - zFar);
    float zScale = zFar * zRangeInverse;
    float zTranslation = zFar * zNear * zRangeInverse;

    float4x4 result = {
        xScale, 0, 0, 0,
        0, yScale, 0, 0,
        0, 0, zScale, zTranslation,
        0, 0, -1, 0 
    };
    return result;
}

inline float4x4 createIdentityMatrix() {
    float4x4 result = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    return result;
}

inline float4x4 operator* (float4x4 a, float4x4 b) {
    return {
        dot(a.row(0), b.cols[0]),
        dot(a.row(1), b.cols[0]),
        dot(a.row(2), b.cols[0]),
        dot(a.row(3), b.cols[0]),
        dot(a.row(0), b.cols[1]),
        dot(a.row(1), b.cols[1]),
        dot(a.row(2), b.cols[1]),
        dot(a.row(3), b.cols[1]),
        dot(a.row(0), b.cols[2]),
        dot(a.row(1), b.cols[2]),
        dot(a.row(2), b.cols[2]),
        dot(a.row(3), b.cols[2]),
        dot(a.row(0), b.cols[3]),
        dot(a.row(1), b.cols[3]),
        dot(a.row(2), b.cols[3]),
        dot(a.row(3), b.cols[3])
    };
}

inline float4 operator* (float4 v, float4x4 m) {
    return {
        dot(v, m.cols[0]),
        dot(v, m.cols[1]),
        dot(v, m.cols[2]),
        dot(v, m.cols[3])
    };
}

inline float4x4 transpose(float4x4 m) {
    return float4x4 {
        m.m[0][0], m.m[1][0], m.m[2][0], m.m[3][0], 
        m.m[0][1], m.m[1][1], m.m[2][1], m.m[3][1], 
        m.m[0][2], m.m[1][2], m.m[2][2], m.m[3][2], 
        m.m[0][3], m.m[1][3], m.m[2][3], m.m[3][3]
    };
}

inline float3x3 float4x4ToFloat3x3(float4x4 m) {
    float3x3 result = {
        m.m[0][0], m.m[0][1], m.m[0][2], 0.0, 
        m.m[1][0], m.m[1][1], m.m[1][2], 0.0,
        m.m[2][0], m.m[2][1], m.m[2][2], 0.0
    };
    return result;
}

inline float4x4 operator* (float4x4 b, float3x3 a) {
    float4x3 result;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j];
        }
    }

    // Pad the result with an identity row (0, 0, 0, 1)
    float4x4 finalResult = {
        result.m[0][0], result.m[0][1], result.m[0][2], 0.0f,
        result.m[1][0], result.m[1][1], result.m[1][2], 0.0f,
        result.m[2][0], result.m[2][1], result.m[2][2], 0.0f,
        0.0f,           0.0f,           0.0f,           1.0f
    };

    return finalResult;
}