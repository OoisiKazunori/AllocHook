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

	//メモリの確保
	enum MemoryType
	{
		GENERATE_DIRECTX_WIN,//ウィンドウや描画APIの初期化
		GENERATE_SCENE,		//シーンの生成
		IN_GAME,			//インゲーム
		MAX
	};

	void SetMemoryType(MemoryType arg_memoryType);
	void SetAddress(void *arg_address);
	bool GetIsCapture();
	void CaptureMaxSize();

	bool m_allReleaseFlag;//ウィンドウを閉じた時に全てのログを確認する。
private:
	std::array<CPUHookMemoryData, MemoryType::MAX>m_memoryData;
	MemoryType m_memoryType;
	static int FLAME_COUNT;
};

