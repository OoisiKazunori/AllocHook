#pragma once
#include"AllockHook.h"

template<class T>
struct STLAllocator
{
	using value_type = T;

	STLAllocator() = default;

	STLAllocator(const T &)
	{
	}
	STLAllocator(const T *)
	{
	}

	template<class U>
	STLAllocator(const U &)
	{
	}
	template<class U>
	STLAllocator(const U *)
	{
	}

	T *allocate(std::size_t num)
	{
		std::size_t size = sizeof(T) * num;
		return static_cast<T *>(malloc(size));
	}

	void deallocate(T *p, std::size_t /*num*/)
	{
		free(p);
	}
};

//-------------------------------------------------------------------------
template<class T, class U>
inline bool operator==(const STLAllocator<T> &lhs, const STLAllocator<U> &rhs)
{
	return true;
}

//-------------------------------------------------------------------------
template<class T, class U>
inline bool operator!=(const STLAllocator<T> &lhs, const STLAllocator<U> &rhs)
{
	return false;
}