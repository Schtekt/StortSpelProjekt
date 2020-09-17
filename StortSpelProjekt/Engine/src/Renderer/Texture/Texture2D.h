#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include "Core.h"
#include "Texture.h"
class Resource;
class CommandInterface;
class ShaderResourceView;
class DescriptorHeap;

class Texture2D : public Texture
{
public:
	Texture2D();
	virtual ~Texture2D();

	bool Init(std::wstring filePath, ID3D12Device5* device, DescriptorHeap* descriptorHeap);

private:
};

#endif
