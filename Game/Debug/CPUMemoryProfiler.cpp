#include "CPUMemoryProfiler.h"
#include <iostream>
#include <sstream>

int CPUMemoryProfiler::FLAME_COUNT = 0;

CPUMemoryProfiler::CPUMemoryProfiler() :m_memoryType(CPUMemoryProfiler::GENERATE_DIRECTX_WIN)
{
}

void CPUMemoryProfiler::Generate(
	const unsigned char *arg_fileName,
	CPUHookMemoryData::AllocType arg_allocType,
	size_t m_allocSize,
	void *m_address
)
{
	m_memoryData[m_memoryType].Generate(arg_fileName, arg_allocType, m_allocSize, m_address);
}

void CPUMemoryProfiler::InitLog()
{
	m_memoryData[m_memoryType].InitLog();
}

void CPUMemoryProfiler::Caputure()
{
	m_memoryData[m_memoryType].Caputure();
}

void CPUMemoryProfiler::InitInGameLog()
{
	m_memoryData[CPUMemoryProfiler::IN_GAME].InitLog();
}

void CPUMemoryProfiler::SetMemoryType(MemoryType arg_memoryType)
{
	m_memoryType = arg_memoryType;
}

void CPUMemoryProfiler::SetAddress(void *arg_address)
{
	m_memoryData[m_memoryType].m_memoryAllocDataArray.back().m_address = arg_address;
}

bool CPUMemoryProfiler::GetIsCapture()
{
	return m_memoryData[m_memoryType].m_caputureFlag;
}

void CPUMemoryProfiler::CaptureMaxSize()
{
	m_memoryData[m_memoryType].CaptureMaxMemorySize();
}
