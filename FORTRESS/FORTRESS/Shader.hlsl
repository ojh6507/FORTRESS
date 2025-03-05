// Shader.hlsl
cbuffer cbCameraInfo : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    float3 gvCameraPosition : packoffset(c8);
};

cbuffer cbGameObjectInfo : register(b2)
{
    matrix gmtxGameObject;
}

struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float4 color : COLOR;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;

    float4 worldPos = mul(float4(input.position, 1.0f), gmtxGameObject);
    output.positionW = worldPos.xyz;
    output.position = mul(worldPos, gmtxView);
    output.position = mul(output.position, gmtxProjection);

    output.color = input.color;

    return output;
}

float4 PS(PS_INPUT input) : SV_TARGET
{
    return input.color;
}
