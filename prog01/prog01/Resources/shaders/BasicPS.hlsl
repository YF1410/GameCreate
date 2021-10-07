#include "Basic.hlsli"

//cbuffer cbuff0 : register(b0)
//{
//	float4 color;
//};

//float4 main() : SV_TARGET
//{
//	//return color;
//	return float4 (1.0f, 0.0f, 0.0f, 1.0f);
//}

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
	//return float4 (input.uv, 1.0f, 1.0f);

	//return float4 (tex.Sample(smp, input.uv)) * color;

	//return float4(input.normal, 1);  //RGB�����ꂼ��̖@����XYZ�AA��1�ŏo��

	float3 light = normalize(float3(1, -1, 1));  //�E���� �����̃��C�g
	float diffuse = saturate(dot(-light, input.normal));  //diffuse��[0, 1]�͈̔͂�Clamp����
	float brightness = diffuse + 0.3f;  //�A���r�G���g����0.3�Ƃ��Čv�Z

	float4 texcolor = float4(tex.Sample(smp, input.uv));

	//return float4(brightness, brightness, brightness, 1); //�P�x��RGB�ɑ�����ďo��
	return float4(texcolor.rgb * brightness, texcolor.a) * color;
}