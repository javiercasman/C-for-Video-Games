#pragma once

#include <dxgi1_6.h>


class ModuleRTDescriptors;
class ModuleShaderDescriptors;
class ModuleDSDescriptors;

class RenderTexture
{
public:
	RenderTexture(const char* name, DXGI_FORMAT format, const Vector4 clearColour, ModuleRTDescriptors* rtDescriptors, ModuleShaderDescriptors* srvDescriptors, ModuleDSDescriptors* dsvDescriptors, DXGI_FORMAT depthFormat = DXGI_FORMAT_UNKNOWN, float clearDepth = 1.0f);
	~RenderTexture();

	void resize(unsigned int newWidth, unsigned int newHeight);

	void renderToTexture(ID3D12GraphicsCommandList* commandList);
	void renderToPixelShader(ID3D12GraphicsCommandList* commandList);

	ID3D12Resource* getTexture() const { return texture.Get(); }
	
	D3D12_CPU_DESCRIPTOR_HANDLE getRTVHandle() const;
	D3D12_GPU_DESCRIPTOR_HANDLE getSRVHandle() const;
	D3D12_CPU_DESCRIPTOR_HANDLE getDSVHandle() const;

	UINT getSRVIndex() const { return srvIndex; }

	bool isValid() const { return width > 0 || height > 0; }
private:
	const char* name;
	DXGI_FORMAT format;
	Vector4 clearColour;
	DXGI_FORMAT depthFormat;
	FLOAT clearDepth;

	ComPtr<ID3D12Resource> texture;
	ComPtr<ID3D12Resource> depthTexture;

	unsigned int width = 0;
	unsigned int height = 0;

	ModuleRTDescriptors* rtDescriptors = nullptr;
	ModuleShaderDescriptors* srvDescriptors = nullptr;
	ModuleDSDescriptors* dsvDescriptors = nullptr;

	UINT rtvIndex = 0;
	UINT srvIndex = 0;
	UINT dsvIndex = 0;
	/*
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
	*/
};