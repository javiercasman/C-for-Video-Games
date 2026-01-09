#pragma once

namespace tinygltf { struct Material; class Model; }

struct BasicMaterialData
{
	Vector4 baseColour;
	BOOL	hasColourTexture; // use BOOL (4 bytes) instead of c++ bool (1 byte) as HLSL bool is 4 bytes long
};

struct PhongMaterialData
{
	Vector4	diffuseColour;
	float	kd;
	float	ks;
	float	shininess;
	BOOL	hasDiffuseTexture;
};

class Material
{
public:

	struct MaterialData
	{
		enum MaterialType
		{
			Basic,
			Phong
		} type;
		union
		{
			BasicMaterialData basic;
			PhongMaterialData phong;
		};
	};
	
	Material();
	~Material();
	void load(const tinygltf::Model& model, const tinygltf::Material& material, const char* basePath, std::vector<ComPtr<ID3D12Resource>> &materialBuffers);

	ModuleShaderDescriptors* getDescriptors() const { return descriptors; }
private:
	ModuleShaderDescriptors* descriptors;

	MaterialData materialData;
	Vector4 colour;
	ComPtr<ID3D12Resource> colourTex;
};