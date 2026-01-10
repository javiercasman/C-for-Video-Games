#include "Globals.h"
#include "ModuleExercise3.h"
#include "ModuleResources.h"
#include "ModuleD3D12.h"
#include "ModuleCamera.h"
#include "Application.h"
#include "ReadData.h"
#include "SimpleMath.h"
#include "DebugDrawPass.h"

ModuleExercise3::ModuleExercise3(ModuleD3D12* d3D12, ModuleCamera* Camera) : d3d12(d3D12), camera(Camera)
{
}

bool ModuleExercise3::init()
{
	commandList = d3d12->getCommandList();
	device = d3d12->getDevice();

	bool ret = createVertexBuffer();
	ret = ret && createRootSignature();
	ret = ret && createPSO();
	if (ret) 
	{
		commandQueue = d3d12->getCommandQueue();
		debugDraw = new DebugDrawPass(device, commandQueue);
	}
	return ret;
}

void ModuleExercise3::render()
{
	if (isCurrentExercise == false) return;
	commandAllocator = d3d12->getCommandAllocator(); //a lo mejor esto no es necesario? si solo usamos el get, asi no hacemos un nuevo comptr y tal
	commandList->Reset(commandAllocator, pso.Get());

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(d3d12->getCurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(1, &barrier);

	//necesito el rtv del frame actual. hacemos funcion de moduled3d12 que nos lo de.
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3d12->getCurrentRtv();
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = d3d12->getDsv();
	commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	Matrix model = Matrix::Identity;
	/*
	Matrix view = Matrix::CreateLookAt(Vector3(0.0f, 10.0f, 10.0f), Vector3::Zero, Vector3::Up);
	float aspect = float(windowWidth) / float(windowHeight);
	float fov = XM_PIDIV4; // PI/4
	Matrix proj = Matrix::CreatePerspectiveFieldOfView(fov, aspect, 0.1f, 1000.0f);
	*/
	Matrix view = camera->getViewMatrix();
	Matrix proj = camera->getProjectionMatrix();

	//Set current Render Target by calling OMSetRenderTargets and clear it with ClearRenderTargetView
	float clearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr); //aaaaaamigo

	//Set the Root Signature to use for rendering by calling SetGraphicsRootSignature
	commandList->SetGraphicsRootSignature(rootSignature.Get());

	//Set the viewport and the scissor using RSSetViewports and RSSetScissorRects commands and passing D3D12_VIEWPORT and D3D12_RECT as arguments respectively
	d3d12->getWindowSize(windowWidth, windowHeight);
	D3D12_VIEWPORT viewport{ 0.0, 0.0, float(windowWidth), float(windowHeight) , 0.0, 1.0 };
	D3D12_RECT scissor{ 0, 0, windowWidth, windowHeight };

	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissor);

	//Set the Primitive Topology by using the function IASetPrimitiveTopology to TRIANGLE_LIST
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Set the Vertex Buffer to use for rendering by calling IASetVertexBuffers
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

	Matrix mvp = mvp = (model * view * proj).Transpose();
	commandList->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / sizeof(UINT32), &mvp, 0);

	//Add the command DrawInstanced with 3 vertices, 1 instance.
	commandList->DrawInstanced(3, 1, 0, 0);
	
	dd::xzSquareGrid(-10.0f, 10.0f, 0.0f, 1.0f, dd::colors::LightGray); // Grid plane
	dd::axisTriad(ddConvert(Matrix::Identity), 0.1f, 1.0f); // XYZ axis
	debugDraw->record(commandList, windowWidth, windowHeight, view, proj);

	/*barrier = CD3DX12_RESOURCE_BARRIER::Transition(d3d12->getCurrentBackBuffer().Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	commandList->ResourceBarrier(1, &barrier);
	if (SUCCEEDED(commandList->Close()))
	{
		ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
		commandQueue->ExecuteCommandLists(std::size(ppCommandLists), ppCommandLists);
	}*/
}

void ModuleExercise3::postRender()
{

}

bool ModuleExercise3::createVertexBuffer()
{
	struct Vertex
	{
		float x, y, z;
	};
	Vertex vertices[3] =
	{
		{-1.0f, -1.0f, 0.0f }, // 0
		{ 0.0f, 1.0f, 0.0f },  // 1
		{ 1.0f, -1.0f, 0.0f }  // 2
	};
	vertexBuffer = app->getResources()->createDefaultBuffer(vertices, sizeof(vertices), "Exercise2");
	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeof(vertices);
	vertexBufferView.StrideInBytes = sizeof(Vertex);
	return true;
}

bool ModuleExercise3::createRootSignature()
{
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	CD3DX12_ROOT_PARAMETER rootParameters;
	rootParameters.InitAsConstants(sizeof(Matrix) / sizeof(UINT32), 0); // number of 32 bit elements in a matrix
	rootSignatureDesc.Init(1, &rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	ComPtr<ID3DBlob> blob;
	if (FAILED(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, nullptr)))
		return false;
	if (FAILED(device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&rootSignature)))) return false;
	return true;
}

bool ModuleExercise3::createPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	auto dataVS = DX::ReadData(L"Exercise3VS.cso");
	auto dataPS = DX::ReadData(L"Exercise3PS.cso");
	psoDesc.VS = { dataVS.data(), dataVS.size() };
	psoDesc.PS = { dataPS.data(), dataPS.size() };

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = { {"MY_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0} };
	psoDesc.InputLayout = { inputLayout, sizeof(inputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC) };

	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.NumRenderTargets = 1;
	psoDesc.SampleDesc = { 1, 0 };
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	if (FAILED(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso)))) return false;
	return true;
}
