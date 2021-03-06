#ifndef VIEWPOOL_H
#define VIEWPOOL_H

class ConstantBuffer;
class DescriptorHeap;
class ShadowInfo;
class Light;

#include "Core.h"

class ViewPool
{
public:
	ViewPool(
		ID3D12Device5* device,
		DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
		DescriptorHeap* descriptorHeap_RTV,
		DescriptorHeap* descriptorHeap_DSV);
	virtual ~ViewPool();

	ConstantBuffer* GetFreeCB(unsigned int size, std::wstring resourceName = L"CBV_DEFAULTNAME");
	ShadowInfo* GetFreeShadowInfo(LIGHT_TYPE type, SHADOW_RESOLUTION shadowResolution);

	void ClearAll();
	void ClearSpecificLight(LIGHT_TYPE type, ConstantBuffer* cbv, ShadowInfo* si);
	void ClearSpecificCB(unsigned int size, ConstantBuffer* cbv);

private:
	ID3D12Device5* m_pDevice = nullptr;

	DescriptorHeap* m_pDescriptorHeap_CBV_UAV_SRV = nullptr;
	DescriptorHeap* m_pDescriptorHeap_RTV = nullptr;
	DescriptorHeap* m_pDescriptorHeap_DSV = nullptr;

	std::map<unsigned int, std::vector<std::pair<bool, ConstantBuffer*>>> m_CbPool;
	ConstantBuffer* createConstantBuffer(unsigned int size, std::wstring resourceName);

	std::map<LIGHT_TYPE, std::vector<std::tuple<bool, SHADOW_RESOLUTION, ShadowInfo*>>> m_ShadowPools;
	ShadowInfo* createShadowInfo(LIGHT_TYPE lightType, SHADOW_RESOLUTION shadowResolution);
	unsigned int m_ShadowInfoIdCounter = 0;
};

#endif
