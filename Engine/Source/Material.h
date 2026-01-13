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

struct PBRPhongMaterialData
{
	XMFLOAT3 diffuseColour;
	BOOL     hasDiffuseTex;

	XMFLOAT3 specularColour;
	float    shininess;
};

class Material
{
public:
	enum MaterialType
	{
		Basic,
		Phong,
		PBRPhong
	};

public:
	Material(MaterialType Type);
	~Material();

	void load(const tinygltf::Model& model, const tinygltf::Material& material, const char* basePath, std::vector<ComPtr<ID3D12Resource>> &materialBuffers);
	const PhongMaterialData& getPhongMaterial() const { if (type == Phong) return materialData.phong; }
	const PBRPhongMaterialData& getPBRMaterial() const { if (type == PBRPhong) return materialData.pbrPhong; }

	ModuleShaderDescriptors* getDescriptors() const { return descriptors; }
	CD3DX12_GPU_DESCRIPTOR_HANDLE getGPUHandle() const;
	MaterialType getMaterialType() const { return type; }
	const std::string& getName() const { return name; }

	void setPhongMaterial(const PhongMaterialData& phong) { if (type == Phong) materialData.phong = phong; }
	void setPBRPhongMaterial(const PBRPhongMaterialData& pbr) { if (type == PBRPhong) materialData.pbrPhong = pbr; }

private:
	union
	{
		BasicMaterialData basic;
		PhongMaterialData phong;
		PBRPhongMaterialData pbrPhong;
	} materialData;
	std::string name;
	MaterialType type;
	Vector4 colour;
	ComPtr<ID3D12Resource> colourTex;

	ModuleShaderDescriptors* descriptors;
	UINT descriptorIdx;
};