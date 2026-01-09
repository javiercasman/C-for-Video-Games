#pragma once

#include<vector>

class Mesh;
class Material;

class Model
{
public:
	Model();
	~Model();

	bool load(const char* assetFileName, const char* basePath);
	void draw(ID3D12GraphicsCommandList* commandList) const;
	void setModelMatrix(const Matrix& matrix) { modelMatrix = matrix; }
	Matrix getModelMatrix() const { return modelMatrix; }
private:
	std::vector<Mesh*> meshes;
	std::vector<Material*> materials;
	std::vector<ComPtr<ID3D12Resource>> materialBuffers;

	Matrix modelMatrix;
};