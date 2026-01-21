#include "Globals.h"
#include "ModuleExercise7.h"
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
#include "RenderTexture.h"
#include "ModuleRTDescriptors.h"
#include "ModuleDSDescriptors.h"
#include "ModuleEditor.h"

ModuleExercise7::ModuleExercise7(ModuleD3D12* d3D12, ModuleCamera* Camera, ModuleRingBuffer* RingBuffer) : d3d12(d3D12), camera(Camera), ringBuffer(RingBuffer)
{
}

bool ModuleExercise7::init()
{
	commandList = d3d12->getCommandList();
	device = d3d12->getDevice();

	//bool ret = createVertexBuffer();
	bool ret = createRootSignature();
	ret = ret && createPSO();
	ret = ret && loadModel();
	depthFormat = DXGI_FORMAT_D32_FLOAT;
	if (ret)
	{
		resources = app->getResources();

		shaderDescriptors = app->getShaderDescriptors();
		samplers = app->getSampler();

		commandQueue = d3d12->getCommandQueue();
		debugDraw = new DebugDrawPass(device, commandQueue);

		rtDescriptors = app->getRTDescriptors();
		dsDescriptors = app->getDSDescriptors();

		renderTexture = new RenderTexture("Exercise7", DXGI_FORMAT_R8G8B8A8_UNORM, Vector4(0.188f, 0.208f, 0.259f, 1.0f), rtDescriptors, shaderDescriptors, dsDescriptors, depthFormat, 1.0f);
	}
	return ret;
}

void ModuleExercise7::preRender()
{
	if (canvasSize.x > 0.0f && canvasSize.y > 0.0f)
		renderTexture->resize(int(canvasSize.x), int(canvasSize.y));
}

void ModuleExercise7::renderToTexture()
{
	commandList->SetPipelineState(pso.Get());

	Matrix modelMatrix = model->getModelMatrix();
	Matrix viewMatrix = camera->getViewMatrix();

	unsigned width = unsigned(canvasSize.x);
	unsigned height = unsigned(canvasSize.y);

	camera->setAspectRatio(float(width), float(height));
	Matrix projMatrix = camera->getProjectionMatrix();

	PerFrame perFrame;
	perFrame.L = light.L;
	perFrame.L.Normalize();
	perFrame.Lc = light.Lc;
	perFrame.Ac = light.Ac;
	perFrame.viewPos = camera->getPosition();

	/*
	//Set the viewport and the scissor using RSSetViewports and RSSetScissorRects commands and passing D3D12_VIEWPORT and D3D12_RECT as arguments respectively
	d3d12->getWindowSize(windowWidth, windowHeight);
	D3D12_VIEWPORT viewport{ 0.0, 0.0, float(windowWidth), float(windowHeight) , 0.0, 1.0 };
	D3D12_RECT scissor{ 0, 0, windowWidth, windowHeight };

	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissor);
	*/

	renderTexture->renderToTexture(commandList);

	//Set the Primitive Topology by using the function IASetPrimitiveTopology to TRIANGLE_LIST
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->SetGraphicsRootDescriptorTable(4, samplers->getGPUHandle(samplerType)); //sampler de momento 0, haremos q con imgui pueda cambiar el sampler

	Matrix mvp = (modelMatrix * viewMatrix * projMatrix).Transpose();
	commandList->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / sizeof(UINT32), &mvp, 0);

	commandList->SetGraphicsRootConstantBufferView(1, ringBuffer->AllocBuffer(&perFrame, alignUp(sizeof(perFrame), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT)));

	model->draw(commandList);

	if (showGrid) dd::xzSquareGrid(-10.0f, 10.0f, 0.0f, 1.0f, dd::colors::LightGray); // Grid plane
	if (showAxis) dd::axisTriad(ddConvert(Matrix::Identity), 0.1f, 1.0f); // XYZ axis
	debugDraw->record(commandList, width, height, viewMatrix, projMatrix);

	renderTexture->renderToPixelShader(commandList);
}

void ModuleExercise7::render()
{
	if (isCurrentExercise == false) return;
	app->getEditor()->exercise7GUI();
	commandAllocator = d3d12->getCommandAllocator();
	//reset commandallocator
	commandList->Reset(commandAllocator, pso.Get());

	//Set the Root Signature to use for rendering by calling SetGraphicsRootSignature
	commandList->SetGraphicsRootSignature(rootSignature.Get());

	ID3D12DescriptorHeap* descriptorHeaps[] = { shaderDescriptors->getHeap(), samplers->getHeap() };
	commandList->SetDescriptorHeaps(2, descriptorHeaps);

	if (renderTexture->isValid() && canvasSize.x > 0.0f && canvasSize.y > 0.0f) renderToTexture();

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(d3d12->getCurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(1, &barrier);

	//necesito el rtv del frame actual. hacemos funcion de moduled3d12 que nos lo de.
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3d12->getCurrentRtv();
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = d3d12->getDsv();
	commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	//Set current Render Target by calling OMSetRenderTargets and clear it with ClearRenderTargetView
	float clearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

}

void ModuleExercise7::postRender()
{

}

void ModuleExercise7::setSampler(ModuleSampler::Type type)
{
	samplerType = type;
}

bool ModuleExercise7::createRootSignature()
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

bool ModuleExercise7::createPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	auto dataVS = DX::ReadData(L"Exercise7VS.cso");
	auto dataPS = DX::ReadData(L"Exercise7PS.cso");
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

bool ModuleExercise7::loadModel()
{
	model = new Model();
	std::string assetsPath = getAssetsPath("Assets/Models/Duck/");
	std::string gltfPath = assetsPath + "Duck.gltf";
	model->load(gltfPath.c_str(), assetsPath.c_str(), Material::PBRPhong);

	//materialBuffer = resources->createDefaultBuffer(materialData, alignUp(sizeof(MaterialData), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));

	return true;
}
