#pragma once
#include"../Game/Debug/CPUMemoryProfiler.h"
#include<Windows.h>

#pragma warning(push,0)
#pragma warning(disable: ALL_CODE_ANALYSIS_WARNINGS)
#if _DEBUG
#include <crtdbg.h>
static void *MyMalloc(size_t size)
{
	void *ptr = _malloc_dbg(size, _NORMAL_BLOCK, __FILE__, __LINE__);
	CPUMemoryProfiler::Instance()->SetAddress(ptr);
	return ptr;
};
#define malloc(size) MyMalloc(size)
#define calloc(s1,s2) _calloc_dbg(s1, s2, _NORMAL_BLOCK, __FILE__, __LINE__ )
#define realloc(p,s) _realloc_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define free(p)  _free_dbg(p, _NORMAL_BLOCK)
inline void *operator new(size_t nSize, LPCSTR lpszFileName, int nLine)
{
	void *ptr = _malloc_dbg(nSize, _NORMAL_BLOCK, lpszFileName, nLine);
	CPUMemoryProfiler::Instance()->SetAddress(ptr);
	return ptr;
}
static void Delete(void *p, LPCSTR lpszFileName, int nLine)
{
	_free_dbg(p, _NORMAL_BLOCK);
}
#define DEBUG_NEW new(__FILE__, __LINE__)
#define MY_NEW DEBUG_NEW
#define DEBUG_DELETE(p) Delete(p,__FILE__, __LINE__)
#define MY_DELETE(p) DEBUG_DELETE(p)
#endif
#pragma warning(pop)
