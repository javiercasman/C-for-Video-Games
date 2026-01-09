#include "Globals.h"
#include "Mesh.h"
#include "ModuleResources.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"

#include "gltf_utils.h"

Mesh::Mesh()
{

}

Mesh::~Mesh()
{
}

void Mesh::load(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive)
{
	//Load primitives
	const auto& itPos = primitive.attributes.find("POSITION");
	if (itPos != primitive.attributes.end()) // If no position no geometry data
	{
		//Load vertex attributes
		numVertices = uint32_t(model.accessors[itPos->second].count);
		vertices = std::make_unique<Vertex[]>(numVertices);
		uint8_t* vertexData = reinterpret_cast<uint8_t*>(vertices.get()); // Casts Vertex Buffer to Bytes (uint8_t*) buffer
		loadAccessorData(vertexData + offsetof(Vertex, position), sizeof(Vector3), sizeof(Vertex), numVertices, model, itPos->second);
		loadAccessorData(vertexData + offsetof(Vertex, texCoord0), sizeof(Vector2), sizeof(Vertex), numVertices, model, primitive.attributes, "TEXCOORD_0");
		loadAccessorData(vertexData + offsetof(Vertex, normal), sizeof(Vector3), sizeof(Vertex), numVertices, model, primitive.attributes, "NORMAL");

		//Load indices
		if (primitive.indices >= 0)
		{
			const tinygltf::Accessor& indAcc = model.accessors[primitive.indices];

			if (indAcc.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT ||
				indAcc.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT ||
				indAcc.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE)
			{
				indexElementSize = tinygltf::GetComponentSizeInBytes(indAcc.componentType);
				numIndices = uint32_t(indAcc.count);
				indices = std::make_unique<uint8_t[]>(numIndices * indexElementSize);
				loadAccessorData(indices.get(), indexElementSize, indexElementSize, numIndices, model, primitive.indices);
			}
		}

		ModuleResources* resources = app->getResources();

		vertexBuffer = resources->createDefaultBuffer(vertices.get(), sizeof(Vertex) * numVertices, "vertexBuffer");
		indexBuffer = resources->createDefaultBuffer(indices.get(), indexElementSize * numIndices, "indexBuffer");

		vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vertexBufferView.StrideInBytes = sizeof(Vertex);
		vertexBufferView.SizeInBytes = numVertices * sizeof(Vertex);

		static const DXGI_FORMAT formats[3] = { DXGI_FORMAT_R8_UINT,	// UNSIGNED BYTE INDICES
												DXGI_FORMAT_R16_UINT,	// UNSIGNED SHORT INDICES
												DXGI_FORMAT_R32_UINT }; // UNSIGNED INT INDICES

		indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
		indexBufferView.Format = formats[indexElementSize >> 1];	// Elements size is 4 or 2 or 1
																	// >> 1 is equivalent to a division by 2
																	// which gives 2, 1, 0 == index in formats array 
		indexBufferView.SizeInBytes = numIndices * indexElementSize;

		materialIndex = primitive.material;
	}

}

void Mesh::draw(ID3D12GraphicsCommandList* commandList) const
{
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	if (numIndices > 0)
	{
		commandList->IASetIndexBuffer(&indexBufferView);
		commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
	}
	else 
		commandList->DrawInstanced(numVertices, 1, 0, 0);
}