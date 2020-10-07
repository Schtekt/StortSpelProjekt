#include "stdafx.h"
#include "Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "GPUMemory/ShaderResourceView.h"
#include "structs.h"
#include "Animation.h"

Model::Model(const std::wstring path, SkeletonNode* rootNode, std::map<unsigned int, VertexWeight>* perVertexBoneData, std::vector<Mesh*>* meshes, std::vector<Animation*>* animations, std::vector<std::map<TEXTURE_TYPE, Texture*>>* textures)
{
	m_Path = path;
	m_pSkeleton = rootNode;
	m_PerVertexBoneData = *perVertexBoneData;
	m_Size = (*meshes).size();

	m_Meshes = (*meshes);
	m_Animations = (*animations);
	m_Textures = (*textures);

	// Store the globalInverse transform.
	DirectX::XMMATRIX globalInverse = DirectX::XMLoadFloat4x4(&rootNode->defaultTransform);
	globalInverse = DirectX::XMMatrixInverse(nullptr, globalInverse);
	DirectX::XMStoreFloat4x4(&m_GlobalInverseTransform, globalInverse);

	// Fill SlotInfo with mesh+material info
	for (unsigned int i = 0; i < (*meshes).size(); i++)
	{
		m_SlotInfos.push_back(
			{
			(*meshes)[i]->m_pSRV->GetDescriptorHeapIndex(),
			(*textures)[i][TEXTURE_TYPE::AMBIENT]->GetDescriptorHeapIndex(),
			(*textures)[i][TEXTURE_TYPE::DIFFUSE]->GetDescriptorHeapIndex(),
			(*textures)[i][TEXTURE_TYPE::SPECULAR]->GetDescriptorHeapIndex(),
			(*textures)[i][TEXTURE_TYPE::NORMAL]->GetDescriptorHeapIndex(),
			(*textures)[i][TEXTURE_TYPE::EMISSIVE]->GetDescriptorHeapIndex(),
			});
	}
}

Model::~Model()
{
	delete m_pSkeleton;
}

void Model::Update()
{
	if (m_pActiveAnimation != nullptr)
	{
		float animationTime = fmod(m_pActiveAnimation->durationInTicks, m_pActiveAnimation->ticksPerSecond);
		m_pActiveAnimation->Update(animationTime);
		updateSkeleton(animationTime, m_pSkeleton, DirectX::XMMatrixIdentity());
	}
}

std::wstring Model::GetPath() const
{
	return m_Path;
}

unsigned int Model::GetSize() const
{
	return m_Size;
}

Mesh* Model::GetMeshAt(unsigned int index)
{
	return m_Meshes[index];
}

std::map<TEXTURE_TYPE, Texture*>* Model::GetTexturesAt(unsigned int index)
{
	return &m_Textures[index];
}

SlotInfo* Model::GetSlotInfoAt(unsigned int index)
{
	return &m_SlotInfos[index];
}

void Model::updateSkeleton(float animationTime, SkeletonNode* node, DirectX::XMMATRIX parentTransform)
{
	m_pActiveAnimation->currentState[node->name].transform;
	DirectX::XMMATRIX transform;
	
	transform = DirectX::XMLoadFloat4x4(&node->defaultTransform);

	if (node->currentStateTransform)
	{
		DirectX::XMVECTOR position, rotationQ, scale, rotationOrigin;
		DirectX::XMLoadFloat3(&node->currentStateTransform->position);
		DirectX::XMLoadFloat4(&node->currentStateTransform->rotationQuaternion);
		DirectX::XMLoadFloat3(&node->currentStateTransform->scaling);
		rotationOrigin = { 0.0f,0.0f,0.0f };
		transform = DirectX::XMMatrixAffineTransformation(scale, rotationOrigin, rotationQ, position);
	}

	transform = parentTransform * transform;

	for (unsigned int i = 0; i < node->children.size(); i++)
	{
		updateSkeleton(animationTime, node->children[i], transform);
	}

	DirectX::XMMATRIX globalInverse = DirectX::XMLoadFloat4x4(&m_GlobalInverseTransform);
	DirectX::XMMATRIX inverseBindPose = DirectX::XMLoadFloat4x4(&node->inverseBindPose);
	transform = globalInverse * transform * inverseBindPose;

	DirectX::XMStoreFloat4x4(&node->modelSpaceTransform, transform);
}
