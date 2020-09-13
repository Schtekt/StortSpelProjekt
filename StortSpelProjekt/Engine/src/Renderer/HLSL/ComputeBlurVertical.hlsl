#include "../../Headers/structs.h"

Texture2D<float4> textures[]   : register (t0);
RWTexture2D<float4> textureToBlur[] : register(u0);

ConstantBuffer<DescriptorHeapIndices> dhIndices : register(b2, space4);

static const int g_BlurRadius = 4;
static const int g_NumThreads = 256;
groupshared float4 g_SharedMem[g_NumThreads + 2 * g_BlurRadius];

[numthreads(1, g_NumThreads, 1)]
void CS_main(uint3 dispatchThreadID : SV_DispatchThreadID, int3 groupThreadID : SV_GroupThreadID)
{
	unsigned int readIndex = dhIndices.index2;
	unsigned int writeIndex = dhIndices.index3;

	float weights[5] = { 0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f };
	/* -------------------- Clamp out of bound samples -------------------- */
	// left side
	if (groupThreadID.y < g_BlurRadius)
	{
		int y = max(dispatchThreadID.y - g_BlurRadius, 0);
		g_SharedMem[groupThreadID.y] = textures[readIndex][int2(dispatchThreadID.x, y)];
	}

	// right side
	if (groupThreadID.y >= g_NumThreads - g_BlurRadius)
	{
		int y = min(dispatchThreadID.y + g_BlurRadius, textures[readIndex].Length.y - 1);
		g_SharedMem[groupThreadID.y + 2 * g_BlurRadius] = textures[readIndex][int2(dispatchThreadID.x, y)];
	}
	/* -------------------- Clamp out of bound samples -------------------- */

	// Fill the middle parts of the sharedMemory
	g_SharedMem[groupThreadID.y + g_BlurRadius] = textures[readIndex][min(dispatchThreadID.xy, textures[readIndex].Length.xy - 1)];

	// Wait for shared memory to be populated before reading from it
	GroupMemoryBarrierWithGroupSync();

	// Blur
	// Current fragments contribution
	float4 blurColor = weights[0] * g_SharedMem[groupThreadID.y + g_BlurRadius];

	// Adjacent fragment contributions
	for (int i = 1; i <= g_BlurRadius; i++)
	{
		int left = groupThreadID.y + g_BlurRadius - i;
		int right = groupThreadID.y + g_BlurRadius + i;
		blurColor += weights[i] * g_SharedMem[left];
		blurColor += weights[i] * g_SharedMem[right];
	}

	textureToBlur[writeIndex][dispatchThreadID.xy] = blurColor;
}