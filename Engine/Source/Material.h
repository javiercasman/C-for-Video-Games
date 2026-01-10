#pragma once

namespace tinygltf { struct Material; class Model; }

struct BasicMaterialData
{
	XMFLOAT4 baseColour;
	BOOL	hasColourTexture; // use BOOL (4 bytes) instead of c++ bool (1 byte) as HLSL bool is 4 bytes long
};

struct PhongMaterialData
{
	XMFLOAT4 diffuseColour;
	float	kd;
	float	ks;
	float	shininess;
	BOOL	hasDiffuseTexture;
};

class Material
{
public:
	enum MaterialType
	{
		Basic,
		Phong
	};

public:
	Material(MaterialType Type);
	~Material();

	void load(const tinygltf::Model& model, const tinygltf::Material& material, const char* basePath, std::vector<ComPtr<ID3D12Resource>> &materialBuffers);
	const PhongMaterialData& getPhongMaterial() const { if (type == Phong) return materialData.phong; }

	ModuleShaderDescriptors* getDescriptors() const { return descriptors; }

private:
	union
	{
		BasicMaterialData basic;
		PhongMaterialData phong;
	} materialData;
	MaterialType type;
	Vector4 colour;
	ComPtr<ID3D12Resource> colourTex;

	ModuleShaderDescriptors* descriptors;
};