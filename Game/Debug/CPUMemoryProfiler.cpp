#include "CPUMemoryProfiler.h"
#include <iostream>
#include <sstream>

const int CPUMemoryProfiler::LOG_MAX_NUM = 60 * 60;
int CPUMemoryProfiler::FLAME_COUNT = 0;

CPUMemoryProfiler::CPUMemoryProfiler() :m_registerArrayNum(0), m_caputureFlag(false)
{
}

void CPUMemoryProfiler::Generate(
	const unsigned char *arg_fileName,
	AllocType arg_allocType,
	size_t m_allocSize,
	void *m_address
)
{
	EmplaceBack();
	const int ARRAY_NUM = static_cast<int>(m_memoryAllocDataArray.size() - 1);
	m_memoryAllocDataArray[ARRAY_NUM].m_fileName = arg_fileName;
	m_memoryAllocDataArray[ARRAY_NUM].m_allocType = arg_allocType;
	m_memoryAllocDataArray[ARRAY_NUM].m_allocSize = m_allocSize;
	m_memoryAllocDataArray[ARRAY_NUM].m_address = m_address;
	if (LOG_MAX_NUM <= ARRAY_NUM)
	{
		PopFront();
	}
}

void CPUMemoryProfiler::InitLog()
{
	m_memoryAllocDataArray.clear();
	m_memoryAllocDataArray.shrink_to_fit();
}

void CPUMemoryProfiler::Caputure()
{
	printf("\n");
	printf("FLAME:%d", FLAME_COUNT);
	printf("\n");
	++FLAME_COUNT;

	m_caputureFlag = true;
	for (auto &obj : m_memoryAllocDataArray)
	{
		std::string printString = " 呼び出し場所:";
		if (obj.m_fileName)
		{
			printString += reinterpret_cast<const char *>(obj.m_fileName);//禁じ手のキャスト。他の方法を探す
		}
		else
		{
			printString += "none";
		}
		printf(" アドレス:%p", &obj.m_address);
		printString += " サイズ:";
		printString += std::to_string(uintptr_t(obj.m_allocSize));
		printString += " アロックの種類:";
		switch (obj.m_allocType)
		{
		case 0:
			printString += "ALLOC";
			break;
		case 1:
			printString += "REALLOC";
			break;
		case 2:
			printString += "FREE";
			break;
		default:
			break;
		}
		printf(printString.c_str());
		printf("\n");
	}

	m_caputureFlag = false;
}

void CPUMemoryProfiler::EmplaceBack()
{
	m_caputureFlag = true;
	m_memoryAllocDataArray.emplace_back();
	m_caputureFlag = false;
}

void CPUMemoryProfiler::PopFront()
{
	m_caputureFlag = true;
	m_memoryAllocDataArray.erase(m_memoryAllocDataArray.begin());
	m_caputureFlag = false;
}
