#include "stdafx.h"
#include "QuadManager.h"

#include "../Renderer/Texture/Texture.h"
#include "../Renderer/DescriptorHeap.h"
#include "../Renderer/GPUMemory/Resource.h"
#include "../Renderer/GPUMemory/ShaderResourceView.h"
#include "../Renderer/Mesh.h"
#include "../Renderer/Renderer.h"

#include "DX12Tasks/CopyOnDemandTask.h"

#include "../Misc/Window.h"
#include "../Misc/AssetLoader.h"

#include "../Events/EventBus.h"

QuadManager::QuadManager()
{
	m_Id = s_Id;
	s_Id++;
}

QuadManager::~QuadManager()
{
	EventBus::GetInstance().Unsubscribe(this, &QuadManager::pressed);

	if (m_pQuad != nullptr)
	{
		delete m_pQuad;
	}

	if (m_pSlotInfo != nullptr)
	{
		delete m_pSlotInfo;
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
	E_DEPTH_LEVEL depthLevel,
	float4 blend,
	Texture* texture,
	float3 color)
{
	// Examine if we are going to overwrite the current quad
	if (m_pQuad != nullptr)
	{
		delete m_pQuad;
		m_pQuad = nullptr;
		m_Clickable = false;

		if (m_pQuadTexture != nullptr)
		{
			delete m_pQuadTexture;
			m_pQuadTexture = nullptr;
		}
		if (m_pQuadTextureMarked != nullptr)
		{
			delete m_pQuadTextureMarked;
			m_pQuadTextureMarked = nullptr;
		}
	}

	if (m_pQuadTexture == nullptr && texture != nullptr)
	{
		m_pQuadTexture = texture;
	}

	m_Name = name;
	m_Clickable = clickable;
	m_Markable = markable;
	m_DepthLevel = depthLevel;
	m_AmountOfBlend = blend;

	float x = (pos.x * 2.0f) - 1.0f;
	float y = ((1.0f - pos.y) * 2.0f) - 1.0f;
	size.x = ((pos.x + size.x) * 2.0f) - 1.0f;
	size.y = ((1.0f - (pos.y + size.y)) * 2.0f) - 1.0f;

	std::vector<Vertex> m_Vertices = {};

	DirectX::XMFLOAT3 normal = DirectX::XMFLOAT3{ 1.0, 1.0, 0.0 };

	Vertex vertex = {};
	vertex.pos = DirectX::XMFLOAT3{ x, y, 0.0f };
	m_Positions["upper_left"] = float2{ vertex.pos.x, vertex.pos.y };
	vertex.uv = DirectX::XMFLOAT2{ 0.0, 0.0 };
	vertex.normal = normal;

	// Using the tangent as the color
	vertex.tangent = DirectX::XMFLOAT3(color.x, color.y, color.z);
	m_Vertices.push_back(vertex);

	vertex.pos = DirectX::XMFLOAT3{ x, size.y, 0.0f };
	m_Positions["lower_left"] = float2{ vertex.pos.x, vertex.pos.y };
	vertex.uv = DirectX::XMFLOAT2{ 0.0, 1.0 };
	m_Vertices.push_back(vertex);

	vertex.pos = DirectX::XMFLOAT3{ size.x, y, 0.0f };
	m_Positions["upper_right"] = float2{ vertex.pos.x, vertex.pos.y };
	vertex.uv = DirectX::XMFLOAT2{ 1.0, 0.0 };
	m_Vertices.push_back(vertex);

	vertex.pos = DirectX::XMFLOAT3{ size.x, size.y, 0.0f };
	m_Positions["lower_right"] = float2{ vertex.pos.x, vertex.pos.y };
	vertex.uv = DirectX::XMFLOAT2{ 1.0, 1.0 };
	m_Vertices.push_back(vertex);

	std::vector<unsigned int> indices = { 0, 2, 1, 2, 1, 3 };

	Renderer* renderer = &Renderer::GetInstance();

	m_pQuad = new Mesh(
		&m_Vertices, &indices,
		to_wstring(std::to_string(m_Id)));

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

		// tmp
		EventBus::GetInstance().Subscribe(this, &QuadManager::test);
	}

	if (m_Markable)
	{
		// The quad should have a "marked" texture if it is clickable and has a texture
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
}

void QuadManager::UploadAndExecuteQuadData()
{
	Renderer* renderer = &Renderer::GetInstance();

	uploadQuadData(renderer);
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

	if ((x >= m_Positions.at("upper_left").x && y <= m_Positions.at("upper_left").y)
		&& (x >= m_Positions.at("lower_left").x && y >= m_Positions.at("lower_left").y)
		&& (x <= m_Positions.at("upper_right").x && y <= m_Positions.at("upper_right").y)
		&& (x <= m_Positions.at("lower_right").x && y >= m_Positions.at("lower_right").y))
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

const E_DEPTH_LEVEL* QuadManager::GetDepthLevel() const
{
	return &m_DepthLevel;
}

const float4 QuadManager::GetAmountOfBlend() const
{
	return m_AmountOfBlend;
}

const int QuadManager::GetId() const
{
	return m_Id;
}

const bool QuadManager::GetActiveTexture() const
{
	return m_ActiveTexture;
}

void QuadManager::SetActiveTexture(const bool texture)
{
	m_ActiveTexture = texture;
}

void QuadManager::pressed(MouseClick* evnt)
{
	if (evnt->button == MOUSE_BUTTON::LEFT_DOWN && evnt->pressed && IsMarked())
	{
		EventBus::GetInstance().Publish(&ButtonPressed(m_Name));
	}
}

void QuadManager::uploadQuadData(Renderer* renderer)
{
	// Submit to GPU
	renderer->submitMeshToCodt(m_pQuad);

	if (m_pQuadTexture != nullptr)
	{
		renderer->submitTextureToCodt(m_pQuadTexture);
	}

	if (m_pQuadTextureMarked != nullptr)
	{
		renderer->submitTextureToCodt(m_pQuadTextureMarked);
	}
}

void QuadManager::test(ButtonPressed* evnt)
{
	if (evnt->name == "head")
	{
		Log::Print("Pressed\n");
	}
}
