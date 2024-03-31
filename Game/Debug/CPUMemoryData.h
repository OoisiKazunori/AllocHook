#include<vector>
#include<string>

/// <summary>
/// �t�b�N���ē��������L�^
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
	bool m_caputureFlag;	//�o�͒��̓������̋L�^���s��Ȃ�
	std::vector<MemoryData>m_memoryAllocDataArray;
private:

	template<typename T>
	void EmplaceBack(std::vector<T> &arg_vector);
	template<typename T>
	void PopFront(std::vector<T> &arg_vector);

	static const int LOG_MAX_NUM;						//���O���L�^����ő�̐�
	int m_registerArrayNum;								//�L���v�`�������ۂɋL�^����p�̃C���f�b�N�X
	std::string printString;
	std::vector<void *>m_freeAddress;					//������ꂽ�|�C���^�̔z��

	void PrintData(const MemoryData &arg_data);
	//�ł��m�ۂ���������
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
