#include "stdafx.h"
#include "QuadManager.h"

#include "../Renderer/Texture/Texture.h"
#include "../Renderer/DescriptorHeap.h"
#include "../Renderer/GPUMemory/Resource.h"
#include "../Renderer/GPUMemory/ShaderResourceView.h"
#include "../Renderer/GPUMemory/ConstantBuffer.h"
#include "../Renderer/DescriptorHeap.h"
#include "../Renderer/Mesh.h"
#include "../Renderer/Renderer.h"

#include "DX12Tasks/CopyOnDemandTask.h"

#include "../Misc/Window.h"
#include "../Misc/AssetLoader.h"

#include "../Events/EventBus.h"

void SendButtonEvent(const std::string& name)
{
	EventBus::GetInstance().Publish(&ButtonPressed(name));
}

QuadManager::QuadManager()
{
	m_Id = s_Id;
	s_Id++;
	m_pOnClicked = &SendButtonEvent;
	m_CBData.blendFactor = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_CBData.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_CBData.pos = { 0.0f, 0.0f };
	m_CBData.scale = { 0.0f, 0.0f };
	m_CBData.textureInfo = { 0.0f, 0.0f };

	Renderer* renderer = &Renderer::GetInstance();
	createCB(renderer->m_pDevice5, renderer->m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]);
}

QuadManager::~QuadManager()
{
	if (m_Clickable == true)
	{
		EventBus::GetInstance().Unsubscribe(this, &QuadManager::pressed);
	}

	if (m_pQuad != nullptr)
	{
		delete m_pQuad;
	}

	if (m_pSlotInfo != nullptr)
	{
		delete m_pSlotInfo;
	}

	if (m_pCB != nullptr)
	{
		// In case of removal in the middle of a frame
		CopyTask* task = Renderer::GetInstance().m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND];
		CopyOnDemandTask* codt = static_cast<CopyOnDemandTask*>(task);
		codt->UnSubmitCB(m_pCB);

		delete m_pCB;
	}
}

bool QuadManager::operator==(const QuadManager& other) const
{
	return (m_Id == other.m_Id);
}

void QuadManager::CreateQuad(
	std::string name,
	float2 pos, float2 size,
	bool clickable, bool markable,
	int depthLevel,
	float4 blend,
	Texture* texture,
	float3 color)
{
	// We can't create a quad if the quad is already created!
	if (m_pQuad != nullptr)
	{
		Log::PrintSeverity(Log::Severity::WARNING, "This quad is already created... Could not create a new quad with the name %s!\n", name.c_str());
		return;
	}

	if (m_pQuadTexture == nullptr && texture != nullptr)
	{
		m_pQuadTexture = texture;
	}

	m_Name = name;
	m_Clickable = clickable;
	m_Markable = markable;
	m_Depth = depthLevel;
	m_BaseScale = size;
	m_BasePos = pos;

	m_CBData.blendFactor = blend;
	m_CBData.color = float4{ color.x, color.y, color.z, 1.0f };
	m_CBData.pos.x = (m_BasePos.x * 2.0f) - 1.0f;
	m_CBData.pos.y = ((1.0f - m_BasePos.y) * 2.0f) - 1.0f;
	m_CBData.scale.x = ((m_BasePos.x + m_BaseScale.x) * 2.0f) - 1.0f;
	m_CBData.scale.y = ((1.0f - (m_BasePos.y + m_BaseScale.y)) * 2.0f) - 1.0f;
	m_CBData.textureInfo.x = HasTexture();

	std::vector<Vertex> m_Vertices = {};

	// Default Quadmesh data
	Vertex vertex = {};
	vertex.pos = DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f };
	vertex.uv = DirectX::XMFLOAT2{ 0.0f, 0.0f };
	vertex.normal = DirectX::XMFLOAT3{ 1.0f, 1.0f, 0.0f };
	vertex.tangent = DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f };	
	m_Vertices.push_back(vertex);

	vertex.pos = DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f };
	vertex.uv = DirectX::XMFLOAT2{ 0.0, 1.0 };
	m_Vertices.push_back(vertex);

	vertex.pos = DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f };
	vertex.uv = DirectX::XMFLOAT2{ 1.0, 0.0 };
	m_Vertices.push_back(vertex);

	vertex.pos = DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f };
	vertex.uv = DirectX::XMFLOAT2{ 1.0, 1.0 };
	m_Vertices.push_back(vertex);

	std::vector<unsigned int> indices = { 0, 2, 1, 2, 1, 3 };

	Renderer* renderer = &Renderer::GetInstance();

	m_pQuad = new Mesh(
		&m_Vertices, &indices,
		to_wstring(m_Name));

	m_pQuad->Init(renderer->m_pDevice5, renderer->m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]);

	m_pSlotInfo = new SlotInfo();
	m_pSlotInfo->vertexDataIndex = m_pQuad->GetSRV()->GetDescriptorHeapIndex();

	if (m_pQuadTexture != nullptr)
	{
		m_pSlotInfo->textureAlbedo = m_pQuadTexture->GetDescriptorHeapIndex();
	}

	if (m_Clickable)
	{
		EventBus::GetInstance().Subscribe(this, &QuadManager::pressed);
	}

	if (m_Markable)
	{
		// The quad should have a "marked" texture if it is markable and has a texture
		if (m_pQuadTexture != nullptr)
		{
			AssetLoader* al = AssetLoader::Get();
			std::wstring markedTexture = m_pQuadTexture->GetPath();

			std::wstring ending = L".png";
			for (int i = 0; i < ending.size(); i++)
			{
				markedTexture.pop_back();
			}

			markedTexture += L"_m" + ending;

			m_pQuadTextureMarked = al->LoadTexture2D(markedTexture);

			m_pSlotInfo->textureEmissive = m_pQuadTextureMarked->GetDescriptorHeapIndex();
		}
	}

	uploadQuadData();
	submitCBQuadDataToCodt();
}

void QuadManager::SetPos(float2 pos)
{
	m_BasePos = pos;

	m_CBData.pos.x = (m_BasePos.x * 2.0f) - 1.0f;
	m_CBData.pos.y = ((1.0f - m_BasePos.y) * 2.0f) - 1.0f;
	m_CBData.scale.x = ((m_BasePos.x + m_BaseScale.x) * 2.0f) - 1.0f;
	m_CBData.scale.y = ((1.0f - (m_BasePos.y + m_BaseScale.y)) * 2.0f) - 1.0f;

	submitCBQuadDataToCodt();
}

void QuadManager::SetSize(float2 size)
{
	m_BaseScale = size;

	m_CBData.pos.x = (m_BasePos.x * 2.0f) - 1.0f;
	m_CBData.pos.y = ((1.0f - m_BasePos.y) * 2.0f) - 1.0f;
	m_CBData.scale.x = ((m_BasePos.x + m_BaseScale.x) * 2.0f) - 1.0f;
	m_CBData.scale.y = ((1.0f - (m_BasePos.y + m_BaseScale.y)) * 2.0f) - 1.0f;

	submitCBQuadDataToCodt();
}

void QuadManager::SetClickable(bool clickable)
{
	// If we no longer want the quad to be clickable, unsubscribe it from the eventbus
	if (m_Clickable == true && clickable == false)
	{
		EventBus::GetInstance().Unsubscribe(this, &QuadManager::pressed);
	}
	// If it was not clickable, subscribe
	else if (m_Clickable == false && clickable == true)
	{
		EventBus::GetInstance().Subscribe(this, &QuadManager::pressed);
	}

	m_Clickable = clickable;
}

void QuadManager::SetMarkable(bool markable)
{
	if (m_Markable == false && markable == true)
	{
		// The quad should have a "marked" texture if it is markable and has a texture
		if (m_pQuadTexture != nullptr)
		{
			AssetLoader* al = AssetLoader::Get();
			std::wstring markedTexture = m_pQuadTexture->GetPath();

			std::wstring ending = L".png";
			for (int i = 0; i < ending.size(); i++)
			{
				markedTexture.pop_back();
			}

			markedTexture += L"_m" + ending;

			if (m_pQuadTextureMarked != nullptr)
			{
				delete m_pQuadTextureMarked;
				m_pQuadTextureMarked = nullptr;
			}
			m_pQuadTextureMarked = al->LoadTexture2D(markedTexture);

			m_pSlotInfo->textureEmissive = m_pQuadTextureMarked->GetDescriptorHeapIndex();
		}
		else
		{
			Log::PrintSeverity(Log::Severity::WARNING, "The quad '%s' does not have a texture... Could not make it markable.\n", m_Name.c_str());
		}
	}
	else if (m_Markable == true && markable == false)
	{
		delete m_pQuadTexture;
		m_pQuadTexture = nullptr;

		delete m_pQuadTextureMarked;
		m_pQuadTextureMarked = nullptr;
	}

	m_Markable = markable;
}

void QuadManager::SetBlend(float4 blend)
{
	m_CBData.blendFactor = blend;

	submitCBQuadDataToCodt();
}

void QuadManager::SetColor(float3 color)
{
	m_CBData.color = float4{ color.x, color.y, color.z, 1.0f };

	submitCBQuadDataToCodt();
}

void QuadManager::SetDepthLevel(int depthLevel)
{
	m_Depth = depthLevel;
}

const bool QuadManager::HasTexture() const
{
	bool exists = false;
	if (m_pQuadTexture != nullptr)
	{
		exists = true;
	}

	return exists;
}

const bool QuadManager::IsMarked() const
{
	bool marked = false;

	float x = 0, y = 0;
	Renderer* renderer = &Renderer::GetInstance();
	renderer->GetWindow()->MouseInClipspace(&x, &y);

	float2 upper_left = { m_CBData.pos.x, m_CBData.pos.y };
	float2 upper_right = { m_CBData.scale.x, m_CBData.pos.y };
	float2 lower_left = { m_CBData.pos.x, m_CBData.scale.y };
	float2 lower_right = { m_CBData.scale.x, m_CBData.scale.y };

	if ((x >= upper_left.x && y <= upper_left.y)
		&& (x >= lower_left.x && y >= lower_left.y)
		&& (x <= upper_right.x && y <= upper_right.y)
		&& (x <= lower_right.x && y >= lower_right.y))
	{
		marked = true;
	}

	return marked;
}

const bool QuadManager::IsClickable() const
{
	return m_Clickable;
}

const bool QuadManager::IsMarkable() const
{
	return m_Markable;
}

const bool QuadManager::IsQuadHidden() const
{
	return m_QuadIsHidden;
}

Mesh* const QuadManager::GetQuad() const
{
	return m_pQuad;
}

Texture* const QuadManager::GetTexture(bool texture) const
{
	if (texture == 0)
	{
		return m_pQuadTexture;
	}
	else
	{
		return m_pQuadTextureMarked;
	}
}

SlotInfo* const QuadManager::GetSlotInfo() const
{
	return m_pSlotInfo;
}

const float4 QuadManager::GetAmountOfBlend() const
{
	return m_CBData.blendFactor;
}

const int QuadManager::GetId() const
{
	return m_Id;
}

int QuadManager::GetDepth() const
{
	return m_Depth;
}

float2 QuadManager::GetScale() const
{
	return m_BaseScale;
}

float2 QuadManager::GetPos() const
{
	return m_BasePos;
}

void QuadManager::SetActiveTexture(const bool texture)
{
	m_CBData.textureInfo.y = texture;
	submitCBQuadDataToCodt();
}

void QuadManager::HideQuad(bool hide)
{
	m_QuadIsHidden = hide;
}

void QuadManager::SetOnClicked(void(*clickFunc)(const std::string&))
{
	m_pOnClicked = clickFunc;
}

void QuadManager::pressed(MouseClick* evnt)
{
	if (evnt->button == MOUSE_BUTTON::LEFT_DOWN && evnt->pressed && IsMarked() && !m_QuadIsHidden)
	{
		m_pOnClicked(m_Name);
	}
}

void QuadManager::uploadQuadData()
{
	Renderer* renderer = &Renderer::GetInstance();
	AssetLoader* al = AssetLoader::Get();

	// Submit to GPU
	renderer->submitMeshToCodt(m_pQuad);

	if (m_pQuadTexture != nullptr && !al->IsTextureLoadedOnGpu(m_pQuadTexture))
	{
		renderer->submitTextureToCodt(m_pQuadTexture);
	}

	if (m_pQuadTextureMarked != nullptr && !al->IsTextureLoadedOnGpu(m_pQuadTextureMarked))
	{
		renderer->submitTextureToCodt(m_pQuadTextureMarked);
	}
}

void QuadManager::createCB(ID3D12Device5* device, DescriptorHeap* descriptorHeap_CBV)
{
	// Create ConstantBuffer
	int sizeOfGUIData = sizeof(CB_PER_GUI_STRUCT);

	if (m_pCB != nullptr)
	{
		delete m_pCB;
		m_pCB = nullptr;
	}

	m_pCB = new ConstantBuffer(device, sizeOfGUIData, L"CB_QUADDATA_RESOURCE", descriptorHeap_CBV);
}

void QuadManager::submitCBQuadDataToCodt()
{
	// Submit to GPU
	const void* data = static_cast<const void*>(&m_CBData);

	Resource* uploadR = m_pCB->GetUploadResource();
	Resource* defaultR = m_pCB->GetDefaultResource();
	Renderer::GetInstance().submitToCodt(&std::make_tuple(uploadR, defaultR, data));
}

void QuadManager::unsubmitQuad()
{
	CopyTask* task = Renderer::GetInstance().m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND];
	CopyOnDemandTask* codt = static_cast<CopyOnDemandTask*>(task);
	codt->UnSubmitMesh(m_pQuad);
}
