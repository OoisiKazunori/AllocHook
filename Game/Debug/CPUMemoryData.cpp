#include "CPUMemoryData.h"
#include <iostream>
#include <sstream>

const int CPUHookMemoryData::LOG_MAX_NUM = 1024;

CPUHookMemoryData::CPUHookMemoryData() :m_registerArrayNum(0), m_caputureFlag(false), m_allocSizeMaxHandle(0)
{
}

void CPUHookMemoryData::Generate(
	const unsigned char *arg_fileName,
	AllocType arg_allocType,
	size_t m_allocSize,
	void *m_address
)
{
	switch (arg_allocType)
	{
	case CPUHookMemoryData::NONE:
		break;
	case CPUHookMemoryData::ALLOC:
	case CPUHookMemoryData::REALLOC:
	{
		EmplaceBack(m_memoryAllocDataArray);
		const int ARRAY_NUM = static_cast<int>(m_memoryAllocDataArray.size() - 1);
		m_memoryAllocDataArray[ARRAY_NUM].m_fileName = arg_fileName;
		m_memoryAllocDataArray[ARRAY_NUM].m_allocType = arg_allocType;
		m_memoryAllocDataArray[ARRAY_NUM].m_allocSize = m_allocSize;
		m_memoryAllocDataArray[ARRAY_NUM].m_address = m_address;

		const size_t maxNum = m_memoryAllocDataArray[m_allocSizeMaxHandle].m_allocSize;
		//最も確保されたメモリを見る
		if (maxNum < m_allocSize || ARRAY_NUM == 0)
		{
			m_allocSizeMaxHandle = ARRAY_NUM;
		}
	}
	break;
	case CPUHookMemoryData::FREE:
	{
		//EmplaceBack(m_freeAddress);
		//const int ARRAY_NUM = static_cast<int>(m_freeAddress.size() - 1);
		//m_freeAddress[ARRAY_NUM] = m_address;
	}
	break;
	default:
		break;
	}

}

void CPUHookMemoryData::InitLog()
{
	m_caputureFlag = true;
	m_memoryAllocDataArray.clear();
	m_memoryAllocDataArray.shrink_to_fit();
	m_allocSizeMaxHandle = 0;
	m_caputureFlag = false;
}

void CPUHookMemoryData::CaptureMaxMemorySize()
{
	//最大値出力
	printf("\n\n\n\n");
	printf("最もメモリを確保した箇所");
	PrintData(m_memoryAllocDataArray[m_allocSizeMaxHandle]);
}

void CPUHookMemoryData::Caputure()
{
	m_caputureFlag = true;
	CaptureMaxMemorySize();
	int i = 0;
	for (auto &obj : m_memoryAllocDataArray)
	{
		++i;
		PrintData(obj);
	}


	m_caputureFlag = false;
}

void CPUHookMemoryData::PrintData(const MemoryData &arg_data)
{
	m_caputureFlag = true;
	std::string printString = " 呼び出し場所:";
	if (arg_data.m_fileName)
	{
		printString += reinterpret_cast<const char *>(arg_data.m_fileName);//禁じ手のキャスト。他の方法を探す
	}
	else
	{
		printString += "none";
	}
	if (arg_data.m_address)
	{
		printf_s(" アドレス:%p", &arg_data.m_address);
	}
	else
	{
		printf_s(" アドレス:nullptr");
	}
	printString += " サイズ:";
	printString += std::to_string(uintptr_t(arg_data.m_allocSize));
	printString += " アロックの種類:";
	switch (arg_data.m_allocType)
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
	printf_s(printString.c_str());
	printf_s("\n");
	m_caputureFlag = false;
}
