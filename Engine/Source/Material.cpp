#include "Globals.h"
#include "Material.h"
#include "ModuleResources.h"
#include "ModuleShaderDescriptors.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"

Material::Material(MaterialType Type) : type(Type)
{
}

Material::~Material()
{
}

void Material::load(const tinygltf::Model& model, const tinygltf::Material& material, const char* basePath, std::vector<ComPtr<ID3D12Resource>>& materialBuffers)
{
	name = material.name;
	colour = Vector4(float(material.pbrMetallicRoughness.baseColorFactor[0]),
		float(material.pbrMetallicRoughness.baseColorFactor[1]),
		float(material.pbrMetallicRoughness.baseColorFactor[2]),
		float(material.pbrMetallicRoughness.baseColorFactor[3]));
	
	BOOL hasColourTex = FALSE;

	if (material.pbrMetallicRoughness.baseColorTexture.index >= 0)
	{
		const tinygltf::Texture& texture = model.textures[material.pbrMetallicRoughness.baseColorTexture.index];
		const tinygltf::Image& image = model.images[texture.source];
		if (!image.uri.empty())
		{
			colourTex = app->getResources()->createTextureFromFile(std::string(basePath) + image.uri);
			hasColourTex = TRUE;
		}
		else hasColourTex = FALSE;
	}

	else hasColourTex = FALSE;

	if (type == Basic)
	{
		materialData.basic.baseColour = colour;
		materialData.basic.hasColourTexture = hasColourTex;
	}
	
	else if (type == Phong)
	{
		materialData.phong.diffuseColour = colour;
		materialData.phong.kd = 0.85f;
		materialData.phong.ks = 0.35f;
		materialData.phong.shininess = 32.0f;
		materialData.phong.hasDiffuseTexture = hasColourTex;
	}

	else if (type == PBRPhong)
	{
		materialData.pbrPhong.diffuseColour = XMFLOAT3(colour.x, colour.y, colour.z);
		materialData.pbrPhong.hasDiffuseTex = hasColourTex;
		materialData.pbrPhong.shininess = 64.0f;
		materialData.pbrPhong.specularColour = XMFLOAT3(0.015f, 0.015f, 0.015f);
	}

	/* 
	esto solo sirve para el ejercicio 5. como vamos por el 6, lo comento. habrá que hacer cambios para meter esto en el ej5 y que funcione bien. me gustaria tener un codigo mas sencillo, que solo seleccionando el ejercicio funcione todo.0
	materialBuffers.push_back(app->getResources()->createDefaultBuffer(&materialData, alignUp(sizeof(MaterialData), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT), "Material Buffer")); //cambiar por ring buffer, ahora entiendo por que no lo hace aqui sino en el module exercise
	*/
	descriptors = app->getShaderDescriptors();

	if (materialData.basic.hasColourTexture)
		descriptors->createSRV(colourTex.Get(), 0);

	else
		descriptors->createNullTexture2DSRV();
}
