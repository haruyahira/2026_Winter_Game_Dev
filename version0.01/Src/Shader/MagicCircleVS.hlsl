cbuffer cbMatrix : register(b2)
{
    row_major matrix matView;
    row_major matrix matProj;
};

struct VS_INPUT
{
    float3 pos : POSITION0;
    float4 spos : POSITION1;
    float3 norm : NORMAL0;
    float3 tangent : TANGENT0;
    float3 binorm : BINORMAL0;
    float4 dif : COLOR0;
    float4 spc : COLOR1;
    float2 uv : TEXCOORD0;
    float2 su : TEXCOORD1;
    uint vid : SV_VertexID;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 dif : COLOR0;
    float4 spc : COLOR1;
    float2 uv : TEXCOORD0;
    float2 su : TEXCOORD1;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;

    float4 localPos = float4(input.pos, 1.0f);
    output.pos = mul(localPos, matView);
    output.pos = mul(output.pos, matProj);

    output.dif = input.dif;
    output.spc = input.spc;

    float2 finalUV = input.uv;
    if (finalUV.x == 0.0f && finalUV.y == 0.0f && input.vid != 0)
    {
        if (input.vid == 1 || input.vid == 3)
            finalUV = float2(1.0f, 0.0f);
        else if (input.vid == 2 || input.vid == 5)
            finalUV = float2(0.0f, 1.0f);
        else if (input.vid == 4)
            finalUV = float2(1.0f, 1.0f);
    }

    output.uv = finalUV;
    output.su = finalUV;

    return output;
}