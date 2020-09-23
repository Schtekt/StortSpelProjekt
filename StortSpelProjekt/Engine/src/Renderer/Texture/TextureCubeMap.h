#ifndef TEXTURECUBEMAP_H
#define TEXTURECUBEMAP_H

#include "Core.h"
#include "Texture.h"
class Resource;
class CommandInterface;
class ShaderResourceView;
class DescriptorHeap;

class TextureCubeMap : public Texture
{
public:
	TextureCubeMap();
	virtual ~TextureCubeMap();

	bool Init(std::wstring filePath, ID3D12Device5* device, DescriptorHeap* descriptorHeap);

private:
	friend class CopyOnDemandTask;

	// TODO: FILIP kolla s� att texture hiraki �r bra
	std::vector<D3D12_SUBRESOURCE_DATA> m_SubResourceData;
	// TODO: FILIP temp not save
	std::unique_ptr<uint8_t[]> m_DdsData;

};

#endif
