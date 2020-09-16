#ifndef DEPTHRENDERTASK_H
#define DEPTHRENDERTASK_H

#include "RenderTask.h"

class DepthRenderTask : public RenderTask
{
public:
	DepthRenderTask(ID3D12Device5* device,
		RootSignature* rootSignature,
		LPCWSTR VSName, LPCWSTR PSName,
		std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
		LPCTSTR psoName);
	~DepthRenderTask();

	void Execute();

private:
	void drawRenderComponent(
		component::ModelComponent* mc,
		component::TransformComponent* tc,
		const DirectX::XMMATRIX* viewProjTransposed,
		ID3D12GraphicsCommandList5* cl);
};

#endif