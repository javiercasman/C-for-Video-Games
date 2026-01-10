#pragma once

#include<vector>
#include "Material.h"

class Mesh;

class Model
{
public:
	Model();
	~Model();

	bool load(const char* assetFileName, const char* basePath, Material::MaterialType type);
	void draw(ID3D12GraphicsCommandList* commandList) const;
	void setModelMatrix(const Matrix& matrix) { modelMatrix = matrix; }
	Matrix getModelMatrix() const { return modelMatrix; }
	std::vector<Material*> getMaterials() const { return materials; }
private:
	std::vector<Mesh*> meshes;
	std::vector<Material*> materials;
	std::vector<ComPtr<ID3D12Resource>> materialBuffers;

	Matrix modelMatrix;
	Matrix normalMatrix;
};