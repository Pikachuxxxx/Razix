#ifndef __STDCBUFFER_H__
#define __STDCBUFFER_H__

#include "../toolkit/shader_common/shader_base.h"

unistruct Constants
{
	Matrix4Unaligned m_modelView;
	Matrix4Unaligned m_modelViewProjection;
	Vector4Unaligned m_lightPosition;
	Vector4Unaligned m_lightColor; // 1.3*float3(0.5,0.58,0.8);
	Vector4Unaligned m_ambientColor; // 0.3*float3(0.18,0.2,0.3);
	Vector4Unaligned m_lightAttenuation; // float4(1,0,0,0);
};

#endif
