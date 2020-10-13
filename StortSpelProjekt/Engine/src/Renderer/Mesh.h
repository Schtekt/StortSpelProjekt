#ifndef MESH_H
#define MESH_H

#include "EngineMath.h"
#include "Core.h"

#include "../ECS/Components/BoundingBoxComponent.h"
#include "../ECS/Components/GUI2DComponent.h"

class Texture;
class Resource;
class ShaderResourceView;
class DescriptorHeap;
class Component;
class GUI2DComponent;
struct SlotInfo;

// DX12 Forward Declarations
struct ID3D12Device5;
struct D3D12_INDEX_BUFFER_VIEW;

struct Vertex
{
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT2 uv;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT3 tangent;
};

class Mesh
{
public:
    Mesh(   ID3D12Device5* device,
            std::vector<Vertex>* vertices,
            std::vector<unsigned int>* indices,
            DescriptorHeap* descriptorHeap_SRV,
            const std::wstring& path = L"NOPATH");
    virtual ~Mesh();

    // Vertices
    Resource* GetDefaultResourceVertices() const;
    const std::vector<Vertex>* GetVertices() const;
    const size_t GetSizeOfVertices() const;
    const size_t GetNumVertices() const;

    // Indices
    Resource* GetDefaultResourceIndices() const;
    const std::vector<unsigned int>* GetIndices() const;
    const size_t GetSizeOfIndices() const;
    const size_t GetNumIndices() const;
    const D3D12_INDEX_BUFFER_VIEW* GetIndexBufferView() const;

    const std::wstring& GetPath() const;

private:
    friend class MergeRenderTask;
    friend class DownSampleRenderTask;
    friend class SkyboxRenderTask;
    friend class Renderer;
    friend class SceneManager;
    friend class Model;
    friend class component::BoundingBoxComponent;
	friend class component::GUI2DComponent;

    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    std::wstring m_Path = L"NOPATH";

    Resource* m_pUploadResourceVertices = nullptr;
    Resource* m_pUploadResourceIndices = nullptr;
    Resource* m_pDefaultResourceVertices = nullptr;
    Resource* m_pDefaultResourceIndices = nullptr;

    ShaderResourceView* m_pSRV = nullptr;
    D3D12_INDEX_BUFFER_VIEW* m_pIndexBufferView = nullptr;
};

#endif
