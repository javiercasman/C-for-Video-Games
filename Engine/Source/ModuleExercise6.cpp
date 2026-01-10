#include "Globals.h"
#include "ModuleExercise6.h"
#include "ModuleResources.h"
#include "ModuleD3D12.h"
#include "ModuleCamera.h"
#include "Application.h"
#include "ReadData.h"
#include "SimpleMath.h"
#include "DebugDrawPass.h"
#include "ModuleShaderDescriptors.h"
#include "ModuleSampler.h"
#include "Model.h"
#include "ModuleRingBuffer.h"

ModuleExercise6::ModuleExercise6(ModuleD3D12* d3D12, ModuleCamera* Camera, ModuleRingBuffer* RingBuffer) : d3d12(d3D12), camera(Camera), ringBuffer(RingBuffer)
{
}

bool ModuleExercise6::init()
{
	commandList = d3d12->getCommandList();
	device = d3d12->getDevice();

	//bool ret = createVertexBuffer();
	bool ret = createRootSignature();
	ret = ret && createPSO();
	ret = ret && loadModel();
	if (ret)
	{
		resources = app->getResources();

		shaderDescriptors = app->getShaderDescriptors();
		samplers = app->getSampler();

		commandQueue = d3d12->getCommandQueue();
		debugDraw = new DebugDrawPass(device, commandQueue);
	}
	return ret;
}


//TODO: en moduleeditor vamos a hacer diferentes funciones segun el ejercicio que queramos hacer. para hacerlo mas sencillo en un futuro, mas modular. hay que hacer que en este ejercicio haga lo mismo del 5 ademas de mdofiicar y actualizar en tiempo real los parametros
// de phong

void ModuleExercise6::render()
{
	commandAllocator = d3d12->getCommandAllocator();
	//reset commandallocator
	commandList->Reset(commandAllocator, pso.Get());

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(d3d12->getCurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(1, &barrier);

	//necesito el rtv del frame actual. hacemos funcion de moduled3d12 que nos lo de.
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3d12->getCurrentRtv();
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = d3d12->getDsv();
	commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	Matrix modelMatrix = model->getModelMatrix();
	/*
	Matrix view = Matrix::CreateLookAt(Vector3(0.0f, 10.0f, 10.0f), Vector3::Zero, Vector3::Up);
	float aspect = float(windowWidth) / float(windowHeight);
	float fov = XM_PIDIV4; // PI/4
	Matrix proj = Matrix::CreatePerspectiveFieldOfView(fov, aspect, 0.1f, 1000.0f);
	*/
	Matrix viewMatrix = camera->getViewMatrix();
	Matrix projMatrix = camera->getProjectionMatrix();

	PerFrame perFrame;
	perFrame.L = light.L;
	perFrame.L.Normalize();
	perFrame.Lc = light.Lc;
	perFrame.Ac = light.Ac;
	perFrame.viewPos = camera->getPosition();

	//Set current Render Target by calling OMSetRenderTargets and clear it with ClearRenderTargetView
	float clearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

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

	ID3D12DescriptorHeap* descriptorHeaps[] = { shaderDescriptors->getHeap(), samplers->getHeap() };
	commandList->SetDescriptorHeaps(2, descriptorHeaps);

	commandList->SetGraphicsRootDescriptorTable(4, samplers->getGPUHandle(samplerType)); //sampler de momento 0, haremos q con imgui pueda cambiar el sampler

	Matrix mvp = (modelMatrix * viewMatrix * projMatrix).Transpose();
	commandList->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / sizeof(UINT32), &mvp, 0);

	commandList->SetGraphicsRootConstantBufferView(1, ringBuffer->AllocBuffer(&perFrame, alignUp(sizeof(perFrame), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT)));

	model->draw(commandList);

	if (showGrid) dd::xzSquareGrid(-10.0f, 10.0f, 0.0f, 1.0f, dd::colors::LightGray); // Grid plane
	if (showAxis) dd::axisTriad(ddConvert(Matrix::Identity), 0.1f, 1.0f); // XYZ axis
	debugDraw->record(commandList, windowWidth, windowHeight, viewMatrix, projMatrix);

	/*barrier = CD3DX12_RESOURCE_BARRIER::Transition(d3d12->getCurrentBackBuffer().Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	commandList->ResourceBarrier(1, &barrier);
	if (SUCCEEDED(commandList->Close()))
	{
		ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
		commandQueue->ExecuteCommandLists(std::size(ppCommandLists), ppCommandLists);
	}*/
}

void ModuleExercise6::postRender()
{

}

void ModuleExercise6::setSampler(ModuleSampler::Type type)
{
	samplerType = type;
}

bool ModuleExercise6::createVertexBuffer()
{
	struct Vertex
	{
		Vector3 position;
		Vector2 uv;
	};
	static Vertex vertices[6] =
	{
		{ Vector3(-1.0f, -1.0f, 0.0f), Vector2(-0.2f, 1.2f) },
		{ Vector3(-1.0f, 1.0f, 0.0f), Vector2(-0.2f, -0.2f) },
		{ Vector3(1.0f, 1.0f, 0.0f), Vector2(1.2f, -0.2f) },
		{ Vector3(-1.0f, -1.0f, 0.0f), Vector2(-0.2f, 1.2f) },
		{ Vector3(1.0f, 1.0f, 0.0f), Vector2(1.2f, -0.2f) },
		{ Vector3(1.0f, -1.0f, 0.0f), Vector2(1.2f, 1.2f) }
	};
	vertexBuffer = resources->createDefaultBuffer(vertices, sizeof(vertices), "Exercise6");
	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeof(vertices);
	vertexBufferView.StrideInBytes = sizeof(Vertex);
	return true;
}

bool ModuleExercise6::createRootSignature()
{
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;

	CD3DX12_ROOT_PARAMETER rootParameters[5] = {};
	CD3DX12_DESCRIPTOR_RANGE tableRanges;
	CD3DX12_DESCRIPTOR_RANGE sampRange;

	tableRanges.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	sampRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 4, 0);

	rootParameters[0].InitAsConstants((sizeof(Matrix) / sizeof(UINT32)), 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
	rootParameters[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[2].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[3].InitAsDescriptorTable(1, &tableRanges, D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters[4].InitAsDescriptorTable(1, &sampRange, D3D12_SHADER_VISIBILITY_PIXEL);
	rootSignatureDesc.Init(5, rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> blob;
	if (FAILED(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, nullptr))) return false;
	if (FAILED(device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&rootSignature)))) return false;

	return true;
}

bool ModuleExercise6::createPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	auto dataVS = DX::ReadData(L"Exercise6VS.cso");
	auto dataPS = DX::ReadData(L"Exercise6PS.cso");
	psoDesc.VS = { dataVS.data(), dataVS.size() };
	psoDesc.PS = { dataPS.data(), dataPS.size() };

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
	psoDesc.InputLayout = { inputLayout, sizeof(inputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC) };

	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.NumRenderTargets = 1;
	psoDesc.SampleDesc = { 1, 0 };
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.FrontCounterClockwise = TRUE;
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	if (FAILED(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso)))) return false;

	return true;
}

bool ModuleExercise6::loadModel()
{
	model = new Model();
	model->load("../Game/Assets/Models/Duck/Duck.gltf", "../Game/Assets/Models/Duck/", Material::Phong);

	//materialBuffer = resources->createDefaultBuffer(materialData, alignUp(sizeof(MaterialData), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));

	return true;
}
