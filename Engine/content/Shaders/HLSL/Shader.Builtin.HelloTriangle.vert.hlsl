struct VSOut
{
    float4 pos : SV_Position;
    float4 color : Color;
};

cbuffer CBuf : register (b0)
{
    matrix transform;
};

VSOut main( float3 pos : Position, float4 color : Color )
{
    VSOut vso;
    vso.pos = mul(float4(pos, 1.0f), transform);
    vso.color = color;
    return vso;
}
