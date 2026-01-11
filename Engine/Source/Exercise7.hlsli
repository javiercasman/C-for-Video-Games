static const float PI = 3.14159265;

cbuffer PerFrame : register(b1)
{
    float3 L;
    float3 Lc;
    float3 Ac;
    float3 viewPos;
};

cbuffer PerInstance : register(b2)
{
    float4x4 modelMat;
    float4x4 normalMat;
    
    float3 diffuseColour;
    bool hasDiffuseTex;
    float3 specularColour;
    float shininess;
};