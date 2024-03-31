#include<vector>
#include<string>

/// <summary>
/// フックして得た情報を記録
/// </summary>
class CPUHookMemoryData
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
		bool m_isReleaseFlag;
		MemoryData() :m_fileName(), m_allocType(NONE), m_address(nullptr), m_allocSize(0), m_isReleaseFlag(false)
		{};
	};

	CPUHookMemoryData();
	void Generate(
		const unsigned char *arg_fileName,
		AllocType arg_allocType,
		size_t m_allocSize,
		void *m_address

	);
	void InitLog();
	void CaptureMaxMemorySize();
	void Caputure();
	bool m_caputureFlag;	//出力中はメモリの記録を行わない
	std::vector<MemoryData>m_memoryAllocDataArray;
private:

	template<typename T>
	void EmplaceBack(std::vector<T> &arg_vector);
	template<typename T>
	void PopFront(std::vector<T> &arg_vector);

	static const int LOG_MAX_NUM;						//ログを記録する最大の数
	int m_registerArrayNum;								//キャプチャした際に記録する用のインデックス
	std::string printString;
	std::vector<void *>m_freeAddress;					//解放されたポインタの配列

	void PrintData(const MemoryData &arg_data);
	//最も確保したメモリ
	int m_allocSizeMaxHandle;
};

template<typename T>
inline void CPUHookMemoryData::EmplaceBack(std::vector<T> &arg_vector)
{
	m_caputureFlag = true;
	arg_vector.emplace_back();
	m_caputureFlag = false;
}

template<typename T>
inline void CPUHookMemoryData::PopFront(std::vector<T> &arg_vector)
{
	m_caputureFlag = true;
	arg_vector.erase(arg_vector.begin());
	m_caputureFlag = false;
}
