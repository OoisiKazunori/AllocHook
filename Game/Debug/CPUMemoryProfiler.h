#pragma once
#include"Helper/ISinglton.h"
#include<vector>
#include<string>

class CPUMemoryProfiler :public ISingleton<CPUMemoryProfiler>
{
public:
	enum AllocType
	{
		NONE,
		ALLOC,
		REALLOC,
		FREE
	};
	struct MemoryData
	{
		const unsigned char *m_fileName;
		void *m_address;
		AllocType m_allocType;
		size_t m_allocSize;
		MemoryData() :m_fileName(), m_allocType(NONE), m_address(nullptr), m_allocSize(0)
		{};
	};

	CPUMemoryProfiler();
	void Generate(
		const unsigned char *arg_fileName,
		AllocType arg_allocType,
		size_t m_allocSize,
		void *m_address

	);
	void InitLog();
	void Caputure();
	bool m_caputureFlag;	//出力中はメモリの記録を行わない
private:

	void EmplaceBack();
	void PopFront();
	static int FLAME_COUNT;
	static const int LOG_MAX_NUM;						//ログを記録する最大の数
	int m_registerArrayNum;								//キャプチャした際に記録する用のインデックス
	std::vector<MemoryData>m_memoryAllocDataArray;
	std::string printString;
};

