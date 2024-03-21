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

//�o�b�t�@�̃������̈�����p�̃f�[�^
struct BufferMemoryData
{
	std::string m_bufferName;		//�o�b�t�@��
	int32_t m_bufferSize;			//�o�b�t�@�̃T�C�Y
	BufferType m_bufferType;		//�o�b�t�@�̎��
	D3D12_RESOURCE_DESC m_bufferDesc;//�o�b�t�@�̎d�l

	std::source_location m_sourceLocation;

	BufferMemoryData(std::string arg_name, int32_t arg_bufferSize, BufferType arg_type, D3D12_RESOURCE_DESC arg_desc, std::source_location arg_sourceLocation) :
		m_bufferName(arg_name), m_bufferSize(arg_bufferSize), m_bufferType(arg_type), m_bufferDesc(arg_desc), m_sourceLocation(arg_sourceLocation) {};
};