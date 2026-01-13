#include "Globals.h"
#include "Model.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_IMPLEMENTATION /* Only in one of the includes */
#include "tiny_gltf.h"

#include "Mesh.h"
#include "Material.h"
#include "ModuleShaderDescriptors.h"
#include "ModuleRingBuffer.h"

#define SCALE_FACTOR 0.01f

struct PerInstancePhong
{
	Matrix modelMat;
	Matrix normalMat;
	PhongMaterialData material;
};

struct PerInstancePBR
{
	Matrix modelMat;
	Matrix normalMat;
	PBRPhongMaterialData material;
};

Model::Model()
{
	modelMatrix = Matrix::Identity * Matrix::CreateScale(SCALE_FACTOR);

	normalMatrix = modelMatrix;
	normalMatrix.Translation(Vector3::Zero);
	normalMatrix.Invert();
	normalMatrix.Transpose();
}

Model::~Model()
{
	for (Mesh* mesh : meshes) delete mesh;
	for (Material* material : materials) delete material;
}

bool Model::load(const char* assetFileName, const char* basePath, Material::MaterialType type)
{
	tinygltf::TinyGLTF gltfContext;
	tinygltf::Model model;
	std::string error, warning;

	bool loadOk = gltfContext.LoadASCIIFromFile(&model, &error, &warning, assetFileName);

	if (loadOk)
	{
		for(const auto& modelMesh : model.meshes)
		{
			for (const auto& modelPrimitive : modelMesh.primitives) {
				// Process each primitive and create Mesh objects
				Mesh* mesh = new Mesh();
				mesh->load(model, modelMesh, modelPrimitive);
				meshes.push_back(mesh);
			}
		}
		for(const auto& modelMaterial : model.materials)
		{
			// Process each material and create Material objects
			Material* material = new Material(type);
			material->load(model, modelMaterial, basePath, materialBuffers); //assetfilename tiene q cambiar pq es otro file
			materials.push_back(material);
		}
		// Load model implementation
		LOG("Model %s loaded successfully", assetFileName);
	}
	else LOG("Error loading %s: %s", assetFileName, error.c_str());
	return loadOk;
}

void Model::draw(ID3D12GraphicsCommandList* commandList) const
{
	for(const Mesh* mesh : meshes)
	{
		if(UINT(mesh->getMaterialIndex()) < materials.size())
		{
			const Material* material = materials[mesh->getMaterialIndex()];

			if (app->getCurrentExerciseIndex() >= 6)
			{
				if (app->getCurrentExerciseIndex() == 6)
				{
					PerInstancePhong perInstance = {};
					perInstance = { modelMatrix.Transpose(), normalMatrix.Transpose(), material->getPhongMaterial() };//esto solo sirve para el ejercicio 6, habra que cambiarlo
					commandList->SetGraphicsRootConstantBufferView(2, app->getRingBuffer()->AllocBuffer(&perInstance, alignUp(sizeof(perInstance), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT)));
				}
				else if (app->getCurrentExerciseIndex() == 7)
				{
					PerInstancePBR perInstance = {};
					perInstance = { modelMatrix.Transpose(), normalMatrix.Transpose(), material->getPBRMaterial() };//esto solo sirve para el ejercicio 6, habra que cambiarlo
					commandList->SetGraphicsRootConstantBufferView(2, app->getRingBuffer()->AllocBuffer(&perInstance, alignUp(sizeof(perInstance), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT)));
				}
				commandList->SetGraphicsRootDescriptorTable(3, material->getGPUHandle());
			}
			else
			{
				commandList->SetGraphicsRootConstantBufferView(1, materialBuffers[mesh->getMaterialIndex()]->GetGPUVirtualAddress()); //ej 5
				commandList->SetGraphicsRootDescriptorTable(2, material->getGPUHandle());
			}
			
			//commandList->SetGraphicsRootDescriptorTable(3, material->getDescriptors()->getGPUHandle(0)); //0 pq es el indice 0 el q tiene el material
			

			mesh->draw(commandList);
		}
	}
}
