#include "stdafx.h"
#include "PreDepthRenderTask.h"

#include "../Mesh.h"
#include "../Transform.h"
#include "../RootSignature.h"
#include "../CommandInterface.h"
#include "../SwapChain.h"
#include "../DescriptorHeap.h"
#include "../RenderTarget.h"
#include "../Resource.h"
#include "../RenderView.h"
#include "../PipelineState.h"
#include "../BaseCamera.h"

PreDepthRenderTask::PreDepthRenderTask(ID3D12Device5* device, 
	RootSignature* rootSignature, 
	LPCWSTR VSName, LPCWSTR PSName, 
	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds, 
	LPCTSTR psoName)
	: RenderTask(device, rootSignature, VSName, PSName, gpsds, psoName)
{
}

PreDepthRenderTask::~PreDepthRenderTask()
{
}

void PreDepthRenderTask::Execute()
{
	ID3D12CommandAllocator* commandAllocator = m_pCommandInterface->GetCommandAllocator(m_CommandInterfaceIndex);
	ID3D12GraphicsCommandList5* commandList = m_pCommandInterface->GetCommandList(m_CommandInterfaceIndex);
	m_pCommandInterface->Reset(m_CommandInterfaceIndex);

	commandList->SetGraphicsRootSignature(m_pRootSig);

	DescriptorHeap* descriptorHeap_CBV_UAV_SRV = m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV];
	ID3D12DescriptorHeap* d3d12DescriptorHeap = descriptorHeap_CBV_UAV_SRV->GetID3D12DescriptorHeap();
	commandList->SetDescriptorHeaps(1, &d3d12DescriptorHeap);

	commandList->SetGraphicsRootDescriptorTable(RS::dtSRV, descriptorHeap_CBV_UAV_SRV->GetGPUHeapAt(0));

	DescriptorHeap* depthBufferHeap = m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::DSV];

	commandList->SetPipelineState(m_PipelineStates[0]->GetPSO());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Same viewport as depth
	const D3D12_VIEWPORT* viewPort = m_pSwapChain->GetRenderTarget(m_BackBufferIndex)->GetRenderView()->GetViewPort();
	const D3D12_RECT* rect = m_pSwapChain->GetRenderTarget(m_BackBufferIndex)->GetRenderView()->GetScissorRect();
	commandList->RSSetViewports(1, viewPort);
	commandList->RSSetScissorRects(1, rect);

	const DirectX::XMMATRIX* viewProjMatTrans = m_pCamera->GetViewProjectionTranposed();

	/*
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		depthBufferHeap->(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_DEPTH_WRITE));
		*/

	// Clear and set depthstencil
	D3D12_CPU_DESCRIPTOR_HANDLE dsh = depthBufferHeap->GetCPUHeapAt(0);
	commandList->ClearDepthStencilView(dsh, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	commandList->OMSetRenderTargets(0, nullptr, false, &dsh);

	// Draw for every Rendercomponent
	for (int i = 0; i < m_RenderComponents.size(); i++)
	{
		component::ModelComponent* mc = m_RenderComponents.at(i).first;
		component::TransformComponent* tc = m_RenderComponents.at(i).second;

		// Draws all entities with ModelComponent + TransformComponent
		drawRenderComponent(mc, tc, viewProjMatTrans, commandList);
	}

	/*
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		pair.second->GetResource()->GetID3D12Resource1(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
		*/
	
	commandList->Close();
}

void PreDepthRenderTask::drawRenderComponent(component::ModelComponent* mc, component::TransformComponent* tc, const DirectX::XMMATRIX* viewProjTransposed, ID3D12GraphicsCommandList5* cl)
{
	// Draw for every m_pMesh the meshComponent has
	for (unsigned int i = 0; i < mc->GetNrOfMeshes(); i++)
	{
		Mesh* m = mc->GetMeshAt(i);
		size_t num_Indices = m->GetNumIndices();
		const SlotInfo* info = mc->GetSlotInfoAt(i);

		Transform* transform = tc->GetTransform();
		DirectX::XMMATRIX* WTransposed = transform->GetWorldMatrixTransposed();
		DirectX::XMMATRIX WVPTransposed = (*viewProjTransposed) * (*WTransposed);

		// Create a CB_PER_OBJECT struct
		CB_PER_OBJECT_STRUCT perObject = { *WTransposed, WVPTransposed, *info };

		cl->SetGraphicsRoot32BitConstants(RS::CB_PER_OBJECT_CONSTANTS, sizeof(CB_PER_OBJECT_STRUCT) / sizeof(UINT), &perObject, 0);

		cl->IASetIndexBuffer(m->GetIndexBufferView());
		cl->DrawIndexedInstanced(num_Indices, 1, 0, 0, 0);
	}
}
