#pragma once
#include"Helper/ISinglton.h"
#include"Helper/HandleMaker.h"
#include"../KazLibrary/Imgui/MyImgui.h"
#include"../KazLibrary/Helper/ConvertEnumToString.h"
#include"../KazLibrary/Helper/BufferResourceData.h"
#include"../KazLibrary/Helper/MemoryProfilerData.h"

class MemoryProfiler :public ISingleton<MemoryProfiler>
{
public:
	MemoryProfiler();

	void CalucurateBufferMemory();
	void Generate(const BufferMemoryData &arg_bufferData);
	void DrawImGui();
	void Capture();

	struct CPUMemoryData
	{
		void *m_ptr;
		size_t m_size;
	};
	static std::vector<CPUMemoryData> m_cpuMemoryDataArray;
private:
	HandleMaker m_handle;
	//生成されたバッファの情報配列
	std::vector<BufferMemoryData>m_bufferMemoryDataArray;
	bool m_isGenerateFlag, m_isDeleteFlag;

	//バッファの種類ごとで確保したメモリ領域のサイズ数を計算する
	std::array<uint32_t, MAX>m_sumBufferMemoryArray;
	uint32_t m_bufferMaxSize;

	//バッファの表示名
	std::array<const char *, MAX>m_bufferNameArray;
	//種類別のバッファの情報保存
	std::array<std::list<BufferMemoryData>, MAX>m_bufferDataArray;


	//バッファの生成破棄の検知
	bool IsBufferNumChange()
	{
		return !m_isGenerateFlag && !m_isDeleteFlag;
	}

	void InitBufferMemoryCount()
	{
		//リセット
		for (auto &obj : m_sumBufferMemoryArray)
		{
			obj = 0;
		}
		m_bufferMaxSize = 0;
	}


	// utility structure for realtime plot
	struct ScrollingBuffer
	{
		int MaxSize;
		int Offset;
		ImVector<ImVec2> Data;
		ScrollingBuffer(int max_size = 2000)
		{
			MaxSize = max_size;
			Offset = 0;
			Data.reserve(MaxSize);
		}
		void AddPoint(float x, float y) {
			if (Data.size() < MaxSize)
				Data.push_back(ImVec2(x, y));
			else {
				Data[Offset] = ImVec2(x, y);
				Offset = (Offset + 1) % MaxSize;
			}
		}
		void Erase() {
			if (Data.size() > 0) {
				Data.shrink(0);
				Offset = 0;
			}
		}
	};
	std::array<MemoryProfiler::ScrollingBuffer, MAX> m_sDataArray;

	int GetSizeNum(uint32_t arg_size, int *arg_num = nullptr, int arg_caluNum = -1);

	std::string GetSize(uint32_t arg_size);

	bool isNumber(const std::string &str)
	{
		return str.find_first_not_of("0123456789") == std::string::npos;
	}
	int GetSizeFromFormat(DXGI_FORMAT arg_format);

	//キャプチャ処理--------------------------------
	//キャプチャ予定のデータ
	std::array<MemoryProfiler::ScrollingBuffer, MAX> m_caputureDataArray;

};
