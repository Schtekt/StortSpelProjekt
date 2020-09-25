#include "stdafx.h"
#include "Model.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include "GPUMemory/ShaderResourceView.h"
#include "structs.h"
#include "Animation.h"

Model::Model(const std::wstring* path, std::vector<Mesh*>* meshes, std::vector<Animation*>* animations, std::vector<Material*>* materials)
{
	m_Path = *path;
	m_Size = (*meshes).size();

	m_Meshes = (*meshes);
	m_Animations = (*animations);
	m_Materials = (*materials);

	// Fill SlotInfo with mesh+material info
	for (unsigned int i = 0; i < (*meshes).size(); i++)
	{
		m_SlotInfos.push_back(
			{
			m_Meshes[i]->m_pSRV->GetDescriptorHeapIndex(),
			m_Materials[i]->GetTexture(TEXTURE_TYPE::ALBEDO)->GetDescriptorHeapIndex(),
			m_Materials[i]->GetTexture(TEXTURE_TYPE::ROUGHNESS)->GetDescriptorHeapIndex(),
			m_Materials[i]->GetTexture(TEXTURE_TYPE::METALLIC)->GetDescriptorHeapIndex(),
			m_Materials[i]->GetTexture(TEXTURE_TYPE::NORMAL)->GetDescriptorHeapIndex(),
			m_Materials[i]->GetTexture(TEXTURE_TYPE::EMISSIVE)->GetDescriptorHeapIndex()
			});
	}
}

Model::~Model()
{
}

const std::wstring* Model::GetPath() const
{
	return &m_Path;
}

unsigned int Model::GetSize() const
{
	return m_Size;
}

Mesh* Model::GetMeshAt(unsigned int index) const
{
	return m_Meshes[index];
}

Material* Model::GetMaterialAt(unsigned int index) const
{
	return m_Materials[index];;
}

const SlotInfo* Model::GetSlotInfoAt(unsigned int index) const
{
	return &m_SlotInfos[index];
}
