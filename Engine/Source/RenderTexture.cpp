#include "Globals.h"
#include "RenderTexture.h"
#include "ModuleResources.h"
#include "ModuleShaderDescriptors.h"
#include "ModuleRTDescriptors.h"
#include "ModuleDSDescriptors.h"

RenderTexture::RenderTexture(const char* name, DXGI_FORMAT format, const Vector4 clearColour, ModuleRTDescriptors* rtDescriptors, ModuleShaderDescriptors* srvDescriptors, ModuleDSDescriptors* dsvDescriptors, DXGI_FORMAT depthFormat, float clearDepth) : name(name), format(format), clearColour(clearColour), rtDescriptors(rtDescriptors), srvDescriptors(srvDescriptors), dsvDescriptors(dsvDescriptors), depthFormat(depthFormat), clearDepth(clearDepth)
{
}

void RenderTexture::resize(unsigned int newWidth, unsigned int newHeight)
{
	if (width == newWidth && height == newHeight)
		return;
	width = newWidth;
	height = newHeight;

	ModuleResources* resources = app->getResources();

	resources->deferRelease(texture);
	texture = resources->createRenderTargetTexture(size_t(width), size_t(height), format, clearColour, name);

	rtvIndex = rtDescriptors->allocDescriptor();
	rtDescriptors->createRTV(texture.Get(), rtvIndex);

	srvIndex = srvDescriptors->allocDescriptor();
	srvDescriptors->createSRV(texture.Get(), srvIndex);

	if (depthFormat != DXGI_FORMAT_UNKNOWN)
	{
		resources->deferRelease(texture);
		depthTexture = resources->createDepthStencil(size_t(width), size_t(height), depthFormat, clearDepth, 0, name);

		dsvIndex = dsvDescriptors->allocDescriptor();
		dsvDescriptors->createDSV(depthTexture.Get(), dsvIndex);
	}
}

void RenderTexture::renderToTexture(ID3D12GraphicsCommandList* commandList)
{
	CD3DX12_RESOURCE_BARRIER toRT = CD3DX12_RESOURCE_BARRIER::Transition(texture.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(1, &toRT);

	D3D12_CPU_DESCRIPTOR_HANDLE rtv = rtDescriptors->getCPUHandle(rtvIndex);

	if (depthFormat == DXGI_FORMAT_UNKNOWN)
	{
		commandList->OMSetRenderTargets(1, &rtv, FALSE, nullptr);
		commandList->ClearRenderTargetView(rtv, reinterpret_cast<float*>(&clearColour), 0, nullptr);
	}
	else
	{
		D3D12_CPU_DESCRIPTOR_HANDLE dsv = dsvDescriptors->getCPUHandle(dsvIndex);

		commandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

		commandList->ClearRenderTargetView(rtv, reinterpret_cast<float*>(&clearColour), 0, nullptr);
		commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	}

	D3D12_VIEWPORT viewport{ 0.0f, 0.0f, float(width), float(height), 0.0f, 1.0f };
	D3D12_RECT scissor = { 0, 0, LONG(width), LONG(height) };
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissor);
}

void RenderTexture::renderToPixelShader(ID3D12GraphicsCommandList* commandList)
{
	CD3DX12_RESOURCE_BARRIER toSRV = CD3DX12_RESOURCE_BARRIER::Transition(texture.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	commandList->ResourceBarrier(1, &toSRV);
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderTexture::getRTVHandle() const
{
	return rtDescriptors->getCPUHandle(rtvIndex);
}

D3D12_GPU_DESCRIPTOR_HANDLE RenderTexture::getSRVHandle() const
{
	return srvDescriptors->getGPUHandle(srvIndex);
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderTexture::getDSVHandle() const
{
	return dsvDescriptors->getCPUHandle(dsvIndex);
}

//D3D12_CPU_DESCRIPTOR_HANDLE rtv = rtDescriptors->getCPUHandle(rtvIndex);