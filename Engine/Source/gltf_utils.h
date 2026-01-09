#pragma once

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE 
#include "tiny_gltf.h"


bool loadAccessorData(uint8_t* data, size_t elemSize, size_t stride, size_t elemCount, const tinygltf::Model& model, int accesorIndex)
{
	const tinygltf::Accessor& accessor = model.accessors[accesorIndex];
	size_t defaultStride = tinygltf::GetComponentSizeInBytes(accessor.componentType) * tinygltf::GetNumComponentsInType(accessor.type);

	if(elemCount == accessor.count && defaultStride == elemSize)
	{
		const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
		const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
		const uint8_t* srcData = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;
		if(bufferView.byteStride == 0)
		{
			// Tightly packed
			memcpy(data, srcData, elemSize * elemCount);
		}
		else
		{
			// With stride
			for(size_t i = 0; i < elemCount; ++i)
			{
				memcpy(data + i * stride, srcData + i * bufferView.byteStride, elemSize);
			}
		}
		return true;
	}
	return false;
}

inline bool loadAccessorData(uint8_t* data, size_t elemSize, size_t stride, size_t elemCount, const tinygltf::Model& model, const std::map<std::string, int>& attributes, const char* accesorName)
{
	const auto& it = attributes.find(accesorName);
	if (it != attributes.end())
	{
		return loadAccessorData(data, elemSize, stride, elemCount, model, it->second);
	}
	return false;
}
