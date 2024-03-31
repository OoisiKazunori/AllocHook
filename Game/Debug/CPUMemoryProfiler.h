#pragma once
#include"Helper/ISinglton.h"
#include"CPUMemoryData.h"
#include<array>
#include<string>

class CPUMemoryProfiler :public ISingleton<CPUMemoryProfiler>
{
public:
	CPUMemoryProfiler();
	void Generate(
		const unsigned char *arg_fileName,
		CPUHookMemoryData::AllocType arg_allocType,
		size_t m_allocSize,
		void *m_address

	);
	void InitLog();
	void Caputure();
	void InitInGameLog();

	//�������̊m��
	enum MemoryType
	{
		GENERATE_DIRECTX_WIN,//�E�B���h�E��`��API�̏�����
		GENERATE_SCENE,		//�V�[���̐���
		IN_GAME,			//�C���Q�[��
		MAX
	};

	void SetMemoryType(MemoryType arg_memoryType);
	void SetAddress(void *arg_address);
	bool GetIsCapture();
	void CaptureMaxSize();

	bool m_allReleaseFlag;//�E�B���h�E��������ɑS�Ẵ��O���m�F����B
private:
	std::array<CPUHookMemoryData, MemoryType::MAX>m_memoryData;
	MemoryType m_memoryType;
	static int FLAME_COUNT;
};

