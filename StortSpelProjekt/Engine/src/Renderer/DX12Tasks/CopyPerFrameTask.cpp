#include "stdafx.h"
#include "CopyPerFrameTask.h"

#include "../CommandInterface.h"

CopyPerFrameTask::CopyPerFrameTask(ID3D12Device5* device, COMMAND_INTERFACE_TYPE interfaceType, unsigned int FLAG_THREAD)
	:CopyTask(device, interfaceType, FLAG_THREAD)
{

}

CopyPerFrameTask::~CopyPerFrameTask()
{

}

void CopyPerFrameTask::ClearSpecific(const Resource* uploadResource)
{
	unsigned int i = 0;
	// Loop through all copyPerFrame tasks
	for (auto& tuple : m_UploadDefaultData)
	{
		if (std::get<0>(tuple) == uploadResource)
		{
			// Remove
			m_UploadDefaultData.erase(m_UploadDefaultData.begin() + i);
		}
		i++;
	}
}

void CopyPerFrameTask::Clear()
{
	m_UploadDefaultData.clear();
}

void CopyPerFrameTask::Execute()
{
	ID3D12CommandAllocator* commandAllocator = m_pCommandInterface->GetCommandAllocator(m_CommandInterfaceIndex);
	ID3D12GraphicsCommandList5* commandList = m_pCommandInterface->GetCommandList(m_CommandInterfaceIndex);

	m_pCommandInterface->Reset(m_CommandInterfaceIndex);

	volatile int counter = 0;
	for (auto& tuple : m_UploadDefaultData)
	{
		if (std::get<2>(tuple) != nullptr)
		{
			copyResource(
				commandList,
				std::get<0>(tuple),		// UploadHeap
				std::get<1>(tuple),		// DefaultHeap
				std::get<2>(tuple));	// Data
		}
		else
		{
			Log::Print("Tried to copy nullptr\n");
		}
		counter++;
	}

	commandList->Close();
}
