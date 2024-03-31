#include "MemoryProfiler.h"

std::vector<MemoryProfiler::CPUMemoryData> MemoryProfiler::m_cpuMemoryDataArray = {};

MemoryProfiler::MemoryProfiler() :
	m_isGenerateFlag(false), m_isDeleteFlag(false)
{
	InitBufferMemoryCount();

	m_bufferNameArray[0] = "ConstBuffer";
	m_bufferNameArray[1] = "Texture";
	m_bufferNameArray[2] = "UnorderAccesssBuffer";
	m_bufferNameArray[3] = "RenderTarget";
	m_bufferNameArray[4] = "Mesh";
}

void MemoryProfiler::CalucurateBufferMemory()
{
	//バッファの確保領域の計算のDirtyFlag
	if (IsBufferNumChange())
	{
		return;
	}
	InitBufferMemoryCount();

	//計算
	for (auto &obj : m_bufferMemoryDataArray)
	{
		//バッファの種類ごとの合計
		m_sumBufferMemoryArray[obj.m_bufferType] += obj.m_bufferSize;
		//全てのバッファで確保した合計の計算
		m_bufferMaxSize += obj.m_bufferSize;

		int num = 0;
		//バッファの名前が重複したら末尾に数字を追加する
		for (auto &buffer : m_bufferDataArray[obj.m_bufferType])
		{
			//名前の重複
			if (buffer.m_bufferName == obj.m_bufferName)
			{
				++num;
			}
			//重複処理済みの名前の比較
			if (buffer.m_bufferName == obj.m_bufferName + "(" + std::to_string(num) + ")")
			{
				++num;
			}
		}
		//種類ごとで情報を格納する
		m_bufferDataArray[obj.m_bufferType].emplace_back(obj);
		if (num != 0)
		{
			m_bufferDataArray[obj.m_bufferType].back().m_bufferName += "(" + std::to_string(num) + ")";
		}
	}

	//大きさ順にソートをかける
	for (auto &obj : m_bufferDataArray)
	{
		obj.sort([](const BufferMemoryData &a, const BufferMemoryData &b)
			{
				int32_t aHandle = a.m_bufferSize, bHandle = b.m_bufferSize;
				if (aHandle < bHandle)
				{
					return false;
				}
				else if (bHandle < aHandle)
				{
					return true;
				}
				else
				{
					return false;
				}
			});
	}

	m_isGenerateFlag = false;
	m_isDeleteFlag = false;

	m_cpuMemoryDataArray;
}

void MemoryProfiler::DrawImGui()
{
	static float t = 0;
	t += ImGui::GetIO().DeltaTime;

	//線グラフによるバッファのメモリ確保領域の表示
	for (int i = 0; i < m_sDataArray.size(); ++i)
	{
		float num = static_cast<float>(GetSizeNum(m_sumBufferMemoryArray[i], nullptr, 3));
		switch (static_cast<BufferType>(i))
		{
		case NONE:
			break;
		case ConstBuffer:
			num *= 500.0f;
			break;
		case Texture:
			break;
		case UnorderAccesssBuffer:
			//num *= 2.0f;
			break;
		case RenderTarget:
			//num *= 5.0f;
			break;
		case Mesh:
			num *= 50.0f;
			break;
		case MAX:
			break;
		default:
			break;
		}
		m_sDataArray[i].AddPoint(t, num);
	}
	ImGui::Begin("MemoryProfiler");
	std::string plotName = "memory-runtime Total:" + GetSize(m_bufferMaxSize);
	if (ImPlot::BeginPlot(plotName.c_str(), ImVec2(-1, 350)))
	{
		ImPlot::SetupAxes("Second", "MB");
		const float history = 10.0f;
		ImPlot::SetupAxisLimits(ImAxis_X1, t - history, t, ImGuiCond_Always);
		ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 4200);
		//線グラフの描画
		for (int i = 0; i < BufferType::MAX; ++i)
		{
			//線
			ImPlot::PlotLine(m_bufferNameArray[i], &m_sDataArray[i].Data[0].x, &m_sDataArray[i].Data[0].y, m_sDataArray[i].Data.size(), 0, m_sDataArray[i].Offset, 2 * sizeof(float));
		}
		ImPlot::EndPlot();
	}

	//ボタン選択
	static BufferType bufferType = ConstBuffer;
	for (int i = 0; i < MAX; ++i)
	{
		if (ImGui::Button(m_bufferNameArray[i]))
		{
			bufferType = static_cast<BufferType>(i);
		}
		if (i != MAX - 1)
		{
			ImGui::SameLine();
		}
	}
	ImGui::Text(m_bufferNameArray[bufferType]);
	//生成されたバッファのCUI
	ImGui::BeginChild(ImGui::GetID((void *)0), ImVec2(450, 450), ImGuiWindowFlags_NoTitleBar);
	for (auto &buffer : m_bufferDataArray[bufferType])
	{
		//個別のバッファの表示
		if (ImGui::TreeNode(buffer.m_bufferName.c_str()))
		{
			std::string string = "BufferSize:";
			string += GetSize(buffer.m_bufferSize);
			ImGui::Text(string.c_str());
			string = "BufferFormat:";
			string += GetEnumName(buffer.m_bufferDesc.Format);
			ImGui::Text(string.c_str());
			//ソースコード表示--------------------------------
			if (ImGui::TreeNode("SourceCode"))
			{
				string = "FileName:";
				string += buffer.m_sourceLocation.file_name();
				ImGui::Text(string.c_str());
				string = "FunctionName:";
				string += buffer.m_sourceLocation.function_name();
				ImGui::Text(string.c_str());
				string = "Line:";
				string += std::to_string(buffer.m_sourceLocation.line());
				ImGui::Text(string.c_str());
				string = "Column:";
				string += std::to_string(buffer.m_sourceLocation.column());
				ImGui::Text(string.c_str());
				ImGui::TreePop();
			}
			//ソースコード表示--------------------------------
			ImGui::TreePop();
		}
	}
	ImGui::EndChild();
	ImGui::End();
}

void MemoryProfiler::Capture()
{
	//線グラフのデータ保存
	for (int i = 0; i < m_sDataArray.size(); ++i)
	{
		m_caputureDataArray[i] = m_sDataArray[i];
	}
}

void MemoryProfiler::Generate(const BufferMemoryData &arg_bufferData)
{
	BufferMemoryData bufferData = arg_bufferData;
	int32_t bitNum = GetSizeFromFormat(arg_bufferData.m_bufferDesc.Format);
	bufferData.m_bufferSize *= bitNum;
	m_bufferMemoryDataArray.emplace_back(bufferData);
	m_isGenerateFlag = true;
}

int MemoryProfiler::GetSizeNum(uint32_t arg_size, int *arg_num, int arg_caluNum)
{
	int num = 0;
	std::string sizeString;
	++num;
	while (1024 <= arg_size)
	{
		++num;
		arg_size /= 1024;
		//指定の単位にする
		if (arg_caluNum <= num && arg_caluNum != -1)
		{
			break;
		}
	}
	//単位確認用
	if (arg_num)
	{
		*arg_num = num;
	}
	return arg_size;
}

std::string MemoryProfiler::GetSize(uint32_t arg_size)
{
	int num = 0;
	int byteSize = GetSizeNum(arg_size, &num);
	std::string sizeString;
	switch (num)
	{
	case 1:
		sizeString = std::to_string(byteSize) + "B";
		break;
	case 2:
		sizeString = std::to_string(byteSize) + "KB";
		break;
	case 3:
		sizeString = std::to_string(byteSize) + "MB";
		break;
	case 4:
		sizeString = std::to_string(byteSize) + "GB";
		break;
	default:
		sizeString = std::to_string(byteSize) + "GB";
		break;
	}
	return sizeString;
};

int MemoryProfiler::GetSizeFromFormat(DXGI_FORMAT arg_format)
{
	std::string enumName = GetEnumName(arg_format);
	int bitNum = 0;
	for (int i = 0; i < enumName.size(); ++i)
	{
		const char chara = enumName[i];
		if (std::isdigit(chara) != 0)
		{
			bitNum += atoi(&chara);
		}
	}
	if (bitNum == 0)
	{
		bitNum = 1;
	}
	return bitNum;
};