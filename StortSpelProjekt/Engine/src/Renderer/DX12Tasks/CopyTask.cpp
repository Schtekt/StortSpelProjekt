#include "stdafx.h"
#include "CopyTask.h"

CopyTask::CopyTask(ID3D12Device5* device)
	:DX12Task(device, COMMAND_INTERFACE_TYPE::COPY_TYPE)
{

}

CopyTask::~CopyTask()
{

}