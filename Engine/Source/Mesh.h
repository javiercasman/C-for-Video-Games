#pragma once

namespace tinygltf { class Model;  struct Mesh; struct Primitive; }

class Mesh
{
public:
	struct Vertex
	{
		Vector3 position;
		Vector2 texCoord0;
		Vector3 normal;
		//Vector3 tangent;
	};
public:
	Mesh();
	~Mesh();

	void load(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive);
	void draw(ID3D12GraphicsCommandList* commandList) const;

	UINT getMaterialIndex() const { return materialIndex; }
private:
	typedef std::unique_ptr<Vertex[]> VertexArray;
	typedef std::unique_ptr<uint8_t[]> IndexArray;


	ComPtr<ID3D12Resource> vertexBuffer;
	ComPtr<ID3D12Resource> indexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	UINT materialIndex;

	UINT numVertices = 0;
	UINT numIndices = 0;
	UINT indexElementSize = 0;

	VertexArray vertices;
	IndexArray indices;
};