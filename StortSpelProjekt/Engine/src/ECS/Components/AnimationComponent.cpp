#include "AnimationComponent.h"

#include "../Renderer/Renderer.h"
#include "../Renderer/AnimatedModel.h"
#include "../Events/EventBus.h"
#include "../ECS/Entity.h"

// DX12
#include "../Renderer/GPUMemory/ShaderResourceView.h"
#include "../Renderer/GPUMemory/UnorderedAccessView.h"
#include "../Renderer/GPUMemory/Resource.h"
#include "../Renderer/DescriptorHeap.h"

component::AnimationComponent::AnimationComponent(Entity* parent)
	:Component(parent)
{
}

component::AnimationComponent::~AnimationComponent()
{
	if (m_pSkeleton)
	{
		delete m_pSkeleton;
	}

	deleteCBMatrices();

	for (unsigned int i = 0; i < m_DefaultResourceVertices.size(); i++)
	{
		delete m_DefaultResourceVertices[i];
		delete m_SRVs[i];
		delete m_pUAVs[i];
	}
}

void component::AnimationComponent::RenderUpdate(double dt)
{
	if (m_pAnimatedModel)
	{
		if (m_pActiveAnimation.first)
		{
			m_pActiveAnimation.second.animationTime += dt;
			if (m_pQueuedAnimation.first)
			{
				// Play "QueuedAnimation"
				// If there is a animation waiting in queue it should be played.
				m_pPendingAnimation.first = m_pQueuedAnimation.first;
				m_pPendingAnimation.second = m_pQueuedAnimation.second;
				m_pEndingAnimation.first = m_pActiveAnimation.first;
				m_pEndingAnimation.second = m_pActiveAnimation.second;
				m_pActiveAnimation.first = nullptr;
				m_pActiveAnimation.second.Reset();
				m_pQueuedAnimation.first = nullptr;
				m_pQueuedAnimation.second.Reset();
				return;
			}
			else if (m_pActiveAnimation.first->isFinished(m_pActiveAnimation.second.animationTime) && !m_pActiveAnimation.second.loop)
			{
				// Play "ReactivateAnimation"
				// If the active animation has finished looping, play the last looping animation.
				m_pPendingAnimation.first = m_pReactivateAnimation.first;
				m_pPendingAnimation.second = m_pReactivateAnimation.second;
				m_pEndingAnimation.first = m_pActiveAnimation.first;
				m_pEndingAnimation.second = m_pActiveAnimation.second;
				m_pActiveAnimation.first = nullptr;
				m_pActiveAnimation.second.Reset();
				m_pReactivateAnimation.first = nullptr;
				m_pReactivateAnimation.second.Reset();
				return;
			}
			m_pActiveAnimation.first->Update(m_pActiveAnimation.second.animationTime, m_AnimationState);
		}
		else if (m_pPendingAnimation.first && m_pEndingAnimation.first)
		{
			float factor;
			m_BlendTimeElapsed += dt;
			if (m_BlendTimeElapsed >= ANIMATION_TRANSITION_TIME)
			{
				m_BlendTimeElapsed = 0.0f;
				// The blend phase is finished, so the pending animation will be active now.
				m_pActiveAnimation.first = m_pPendingAnimation.first;
				m_pActiveAnimation.second = m_pPendingAnimation.second;
				m_pPendingAnimation.second.Reset();
				m_pPendingAnimation.first = nullptr;
				m_pEndingAnimation.second.Reset();
				m_pEndingAnimation.first = nullptr;
				return;
			}
			else
			{
				factor = m_BlendTimeElapsed / ANIMATION_TRANSITION_TIME;
			}
			assert(factor >= 0.0f && factor <= 1.0f);

			m_pPendingAnimation.second.animationTime += dt;
			m_pEndingAnimation.second.animationTime += dt;
			std::map<std::string, TransformKey> startState;
			m_pPendingAnimation.first->Update(m_pPendingAnimation.second.animationTime, startState);
			std::map<std::string, TransformKey> endState;
			m_pEndingAnimation.first->Update(m_pEndingAnimation.second.animationTime, endState);

			for (auto& key : startState)
			{
				m_AnimationState[key.first].position = InterpolateTranslation(&endState[key.first].position, &key.second.position, factor);
				m_AnimationState[key.first].rotation = InterpolateRotation(&endState[key.first].rotation, &key.second.rotation, factor);
				m_AnimationState[key.first].scaling	= InterpolateScaling(&endState[key.first].scaling, &key.second.scaling, factor);
			}
		}

		// Update Data
		updateSkeleton(m_pSkeleton, DirectX::XMMatrixIdentity());

		// Set Data
		const void* data = static_cast<const void*>(m_UploadMatrices.data());
		m_pCB->GetUploadResource()->SetData(data);
	}
}

void component::AnimationComponent::OnInitScene()
{
	Renderer::GetInstance().InitAnimationComponent(this); 
}

void component::AnimationComponent::OnUnInitScene()
{
	Renderer::GetInstance().UnInitAnimationComponent(this);
	Reset();

	// Maybe wrong place
	for (unsigned int i = 0; i < m_DefaultResourceVertices.size(); i++)
	{
		delete m_DefaultResourceVertices[i];
		delete m_SRVs[i];
		delete m_pUAVs[i];

		m_DefaultResourceVertices[i] = nullptr;
		m_SRVs[i] = nullptr;
		m_pUAVs[i] = nullptr;
	}
}

void component::AnimationComponent::Reset()
{
	// Clear queues 
	m_pPendingAnimation.first = nullptr;
	m_pActiveAnimation.first = nullptr;
	m_pEndingAnimation.first = nullptr;
	m_pReactivateAnimation.first = nullptr;
	m_pQueuedAnimation.first = nullptr;

	m_pPendingAnimation.second.Reset();
	m_pActiveAnimation.second.Reset();
	m_pEndingAnimation.second.Reset();
	m_pReactivateAnimation.second.Reset();
	m_pQueuedAnimation.second.Reset();

	// Initialize the upload matrices
	DirectX::XMFLOAT4X4 matIdentity;
	DirectX::XMStoreFloat4x4(&matIdentity, DirectX::XMMatrixIdentity());
	m_UploadMatrices.clear();
	for (unsigned int i = 0; i < MAX_ANIMATION_MATRICES; i++)
	{
		m_UploadMatrices.push_back(matIdentity);
	}

	// Run the default animation.
	for (auto& animation : m_Animations)
	{
		if (animation->name == "Idle")
		{
			m_pActiveAnimation.first = animation;
			m_pActiveAnimation.first->Update(m_pActiveAnimation.second.animationTime, m_AnimationState);
			break;
		}
	}
}

bool component::AnimationComponent::PlayAnimation(std::string animationName, bool loop)
{
	for (auto& animation : m_Animations)
	{
		if (animation->name == animationName)
		{
			// If there is no pending animation we add the new animation.
			if(!m_pPendingAnimation.first && m_pActiveAnimation.first)
			{
				if (m_pActiveAnimation.first->name != animation->name)
				{
					// First, put the new animation as pending, so it can be blended with the currently active animation.
					m_pPendingAnimation.first = animation;
					m_pPendingAnimation.second.loop = loop;
					if (m_pActiveAnimation.second.loop) // If it's not looping however, we have to reactivate the currently active animation later.
					{
						m_pReactivateAnimation.first = m_pActiveAnimation.first;
						m_pReactivateAnimation.second = m_pActiveAnimation.second;
					}
					else if (loop) // If this animation will be looping we don't need to reactivate the old one.
					{
						m_pReactivateAnimation.first = nullptr;
						m_pReactivateAnimation.second.Reset();
					}
					// Next, we tell the currently active animation to end, so we can blend to the new animation.
					m_pEndingAnimation.first = m_pActiveAnimation.first;
					m_pEndingAnimation.second = m_pActiveAnimation.second;
					m_pActiveAnimation.first = nullptr;
					m_pActiveAnimation.second.Reset();

					return true;
				}
			}
			else if (m_pActiveAnimation.first) // If there is already a pending animation we put the new animation in a queue
			{
				m_pQueuedAnimation.first = animation;
				m_pQueuedAnimation.second.loop = loop;
				return true;
			}
		}
	}

	return false;
}

void component::AnimationComponent::initialize(ID3D12Device5* device5, DescriptorHeap* dh_CBV_UAV_SRV)
{
	component::ModelComponent* mc = m_pParent->GetComponent<component::ModelComponent>();
	if (mc != nullptr)
	{
		m_pAnimatedModel = dynamic_cast<AnimatedModel*>(mc->GetModel());
		if (m_pAnimatedModel)
		{
			m_pSkeleton = m_pAnimatedModel->CloneSkeleton();
			m_Animations = m_pAnimatedModel->GetAnimations();

			// Store the globalInverse transform.
			DirectX::XMMATRIX globalTransform = DirectX::XMLoadFloat4x4(&m_pSkeleton->defaultTransform);
			DirectX::XMStoreFloat4x4(&m_GlobalInverseTransform, DirectX::XMMatrixInverse(nullptr, globalTransform));

			m_UploadMatrices.reserve(MAX_ANIMATION_MATRICES);

			Reset();
			bindAnimation(m_pSkeleton);

			// Create the hackMeshes
			for (unsigned int i = 0; i < mc->GetNrOfMeshes(); i++)
			{
				std::string meshName = m_pParent->GetName();

				// create vertices resource
				m_DefaultResourceVertices.push_back(new Resource(device5, mc->GetMeshAt(i)->GetSizeOfVertices(), RESOURCE_TYPE::DEFAULT, L"VERTEX_DEFAULT_RESOURCE_" + to_wstring(meshName), D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS));

				// Create SRV
				D3D12_SHADER_RESOURCE_VIEW_DESC dsrv = {};
				dsrv.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
				dsrv.Buffer.FirstElement = 0;
				dsrv.Format = DXGI_FORMAT_UNKNOWN;
				dsrv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				dsrv.Buffer.NumElements = mc->GetMeshAt(i)->GetNumVertices();
				dsrv.Buffer.StructureByteStride = sizeof(Vertex);

				// Set view to mesh
				m_SRVs.push_back(new ShaderResourceView(
					device5,
					dh_CBV_UAV_SRV,
					&dsrv,
					m_DefaultResourceVertices[i]));


				// Create uavs

				D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
				uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
				uavDesc.Buffer.FirstElement = 0;
				uavDesc.Buffer.NumElements = mc->GetMeshAt(i)->GetNumVertices();
				uavDesc.Buffer.StructureByteStride = sizeof(Vertex);

				m_pUAVs.push_back(new UnorderedAccessView(
					device5,
					dh_CBV_UAV_SRV,
					&uavDesc,
					m_DefaultResourceVertices[i]));
			}
		}
		else
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "AnimationComponent initialized when there's no animated model! Make sure the model used actually has animations.\n");
		}
	}
	else
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "AnimationComponent initialized when there's no ModelComponent! Make sure the AnimationComponent is initialized AFTER the modelComponent.\n");
	}
}

void component::AnimationComponent::updateSkeleton(SkeletonNode* node, DirectX::XMMATRIX parentTransform)
{
	DirectX::XMMATRIX localTransform = DirectX::XMMatrixIdentity();

	if (node->currentStateTransform) // node animated?
	{
		DirectX::XMVECTOR position, rotationQ, scale, rotationOrigin;

		if (node->currentStateTransform->pScale)
		{
			scale = DirectX::XMLoadFloat3(node->currentStateTransform->pScale);
			localTransform *= DirectX::XMMatrixScalingFromVector(scale);
		}

		if (node->currentStateTransform->pRotation)
		{
			rotationQ = DirectX::XMLoadFloat4(node->currentStateTransform->pRotation);
			localTransform *= DirectX::XMMatrixRotationQuaternion(rotationQ);
		}

		if (node->currentStateTransform->pPosition)
		{
			position = DirectX::XMLoadFloat3(node->currentStateTransform->pPosition);
			localTransform *= DirectX::XMMatrixTranslationFromVector(position);
		}
	}
	else // use default transformation otherwise
	{
		localTransform = DirectX::XMLoadFloat4x4(&node->defaultTransform);
	}

	DirectX::XMMATRIX finalTransform = localTransform * parentTransform;

	if (node->boneID != -1)
	{
		DirectX::XMMATRIX globalInverse = DirectX::XMLoadFloat4x4(&m_GlobalInverseTransform);
		DirectX::XMMATRIX inverseBindPose = DirectX::XMLoadFloat4x4(&node->inverseBindPose);

		DirectX::XMStoreFloat4x4(&m_UploadMatrices[node->boneID], DirectX::XMMatrixTranspose(inverseBindPose * finalTransform * globalInverse));
	}

	for (unsigned int i = 0; i < node->children.size(); i++)
	{
		updateSkeleton(node->children[i], finalTransform);
	}
}

void component::AnimationComponent::bindAnimation(SkeletonNode* node)
{
	if (m_AnimationState.find(node->name) != m_AnimationState.end() && node->name!="BoneRoot")
	{
		node->currentStateTransform = &m_AnimationState[node->name];
	}
	else
	{
		node->currentStateTransform = nullptr;
	}

	// Loop through all nodes in the tree
	for (auto& child : node->children)
	{
		bindAnimation(child);
	}
}

void component::AnimationComponent::createCBMatrices(ID3D12Device5* device, DescriptorHeap* dh_CBV_UAV_SRV)
{
	// Create the ConstantBuffer
	std::string temp = m_pParent->GetName();
	temp = temp.substr(temp.find_last_of("/\\") + 1);
	std::wstring resourceName = L"_ANIMATED_CB_Matrices_" + to_wstring(temp);

	m_pCB = new ConstantBuffer(device, sizeof(ANIMATION_MATRICES_STRUCT), resourceName, dh_CBV_UAV_SRV);
}

void component::AnimationComponent::deleteCBMatrices()
{
	if (m_pCB != nullptr)
	{
		delete m_pCB;
		m_pCB = nullptr;
	}
}