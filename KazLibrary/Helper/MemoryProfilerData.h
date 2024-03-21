#pragma once
#include<string>
#include <source_location>

enum BufferType
{
	NONE = -1,
	ConstBuffer,
	Texture,
	UnorderAccesssBuffer,
	RenderTarget,
	Mesh,
	CPU,
	MAX
};

//バッファのメモリ領域可視化用のデータ
struct BufferMemoryData
{
	std::string m_bufferName;		//バッファ名
	int32_t m_bufferSize;			//バッファのサイズ
	BufferType m_bufferType;		//バッファの種類
	D3D12_RESOURCE_DESC m_bufferDesc;//バッファの仕様

	std::source_location m_sourceLocation;

	BufferMemoryData(std::string arg_name, int32_t arg_bufferSize, BufferType arg_type, D3D12_RESOURCE_DESC arg_desc, std::source_location arg_sourceLocation) :
		m_bufferName(arg_name), m_bufferSize(arg_bufferSize), m_bufferType(arg_type), m_bufferDesc(arg_desc), m_sourceLocation(arg_sourceLocation) {};
};