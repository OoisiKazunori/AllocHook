#pragma once
#include"../DirectXCommon/Base.h"
#include"../KazLibrary/DirectXCommon/DirectX12CmdList.h"
#include"../KazLibrary/DirectXCommon/DirectX12Device.h"
#include"../KazLibrary/Pipeline/GraphicsRootSignature.h"
#include"MemoryProfilerData.h"
#include"BufferResourceData.h"
//class DescriptorHeapMgr;

struct BufferMemorySize
{
	int startSize;
	int endSize;

	BufferMemorySize() :startSize(-1), endSize(-1)
	{};
};

enum BufferMemory
{
	DESCRIPTORHEAP_MEMORY_NONE = -1,
	DESCRIPTORHEAP_MEMORY_TEXTURE_IMGUI,
	DESCRIPTORHEAP_MEMORY_TEXTURE_SPRITE,
	DESCRIPTORHEAP_MEMORY_TEXTURE_OBJ,
	DESCRIPTORHEAP_MEMORY_IAPOLYGONE,
	DESCRIPTORHEAP_MEMORY_TEXTURE_RENDERTARGET,
	DESCRIPTORHEAP_MEMORY_TEXTURE_COMPUTEBUFFER,
	DESCRIPTORHEAP_MEMORY_CBV,
	DESCRIPTORHEAP_MEMORY_SRV,
	DESCRIPTORHEAP_MEMORY_MOVIE,
	DESCRIPTORHEAP_MEMORY_MAX
};

namespace KazBufferHelper
{
	struct BufferResourceData
	{
		D3D12_HEAP_PROPERTIES heapProperties;
		D3D12_HEAP_FLAGS heapFlags;
		D3D12_RESOURCE_DESC resourceDesc;
		D3D12_RESOURCE_STATES resourceState;
		D3D12_CLEAR_VALUE *clearValue;
		std::string BufferName;
		BufferType m_bufferType;
		std::source_location m_sourceLocation;

		BufferResourceData
		(
			const D3D12_HEAP_PROPERTIES &HEAP_PROP,
			const D3D12_HEAP_FLAGS &HEAP_FLAGS,
			const D3D12_RESOURCE_DESC &RESOURCE_DESC,
			const D3D12_RESOURCE_STATES &RESOURCE_STATE,
			D3D12_CLEAR_VALUE *CLEAR_VALUE,
			const std::string &BUFFER_NAME,
			BufferType arg_bufferType,
			std::source_location arg_sourceLocation
		)
			:heapProperties(HEAP_PROP),
			heapFlags(HEAP_FLAGS),
			resourceDesc(RESOURCE_DESC),
			resourceState(RESOURCE_STATE),
			clearValue(CLEAR_VALUE),
			BufferName(BUFFER_NAME),
			m_bufferType(arg_bufferType),
			m_sourceLocation(arg_sourceLocation)
		{
		};
	};

	/// <summary>
	/// 定数バッファを生成する際に必要な設定を簡易に纏めた物
	/// </summary>
	/// <param name="BUFFER_SIZE">定数バッファのサイズ</param>
	/// <returns>定数バッファの生成に必要な設定</returns>
	KazBufferHelper::BufferResourceData SetConstBufferData(const unsigned int &BUFFER_SIZE, const std::string &BUFFER_NAME = "ConstBuffer", std::source_location DRAW_SOURCE_LOCATION = std::source_location::current());

	/// <summary>
	/// シェーダーリソースバッファを生成する際に必要な設定を簡易に纏めた物
	/// <param name="TEXTURE_DATA">読み込んだテクスチャの設定</param>
	/// <returns>シェーダーリソースバッファの生成に必要な設定</returns>
	KazBufferHelper::BufferResourceData SetShaderResourceBufferData(const D3D12_RESOURCE_DESC &TEXTURE_DATA, const std::string &BUFFER_NAME = "ShaderResourceBuffer", std::source_location DRAW_SOURCE_LOCATION = std::source_location::current());


	KazBufferHelper::BufferResourceData SetVertexBufferData(BUFFER_SIZE BUFFER_SIZE, const std::string &BUFFER_NAME = "VertexBuffer", std::source_location DRAW_SOURCE_LOCATION = std::source_location::current());


	KazBufferHelper::BufferResourceData SetIndexBufferData(BUFFER_SIZE BUFFER_SIZE, const std::string &BUFFER_NAME = "IndexBuffer", std::source_location DRAW_SOURCE_LOCATION = std::source_location::current());


	KazBufferHelper::BufferResourceData SetRenderTargetData(const D3D12_HEAP_PROPERTIES &HEAP_PROPERTIES, const D3D12_RESOURCE_DESC &RESOURCE, D3D12_CLEAR_VALUE *CLEAR_COLOR, const std::string &BUFFER_NAME = "RenderTarget", std::source_location DRAW_SOURCE_LOCATION = std::source_location::current());

	KazBufferHelper::BufferResourceData SetStructureBuffer(BUFFER_SIZE BUFFER_SIZE, const std::string &BUFFER_NAME = "StructureBuffer", std::source_location DRAW_SOURCE_LOCATION = std::source_location::current());

	KazBufferHelper::BufferResourceData SetRWStructuredBuffer(BUFFER_SIZE BUFFER_SIZE, const std::string &BUFFER_NAME = "RWStructureBuffer", std::source_location DRAW_SOURCE_LOCATION = std::source_location::current());

	KazBufferHelper::BufferResourceData SetUAVTexBuffer(int width, int height, DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT, const std::string &BUFFER_NAME = "UAVTexture", std::source_location DRAW_SOURCE_LOCATION = std::source_location::current());
	KazBufferHelper::BufferResourceData SetUAV3DTexBuffer(int arg_width, int arg_height, int arg_depth, DXGI_FORMAT arg_format = DXGI_FORMAT_R32G32B32A32_FLOAT, const std::string &arg_bufferName = "UAVVolumeTexture", std::source_location DRAW_SOURCE_LOCATION = std::source_location::current());

	KazBufferHelper::BufferResourceData SetOnlyReadStructuredBuffer(BUFFER_SIZE BUFFER_SIZE, const std::string &BUFFER_NAME = "OnlyReadStructureBuffer", std::source_location DRAW_SOURCE_LOCATION = std::source_location::current());


	KazBufferHelper::BufferResourceData SetCommandBufferData(const unsigned int &BUFFER_SIZE, const std::string &BUFFER_NAME = "CommandBuffer", std::source_location DRAW_SOURCE_LOCATION = std::source_location::current());

	D3D12_VERTEX_BUFFER_VIEW SetVertexBufferView(const D3D12_GPU_VIRTUAL_ADDRESS &GPU_ADDRESS, BUFFER_SIZE BUFFER_SIZE, const unsigned int &ONE_VERTICES_SIZE);

	D3D12_INDEX_BUFFER_VIEW SetIndexBufferView(const D3D12_GPU_VIRTUAL_ADDRESS &GPU_ADDRESS, BUFFER_SIZE BUFFER_SIZE);

	D3D12_UNORDERED_ACCESS_VIEW_DESC SetUnorderedAccessView(BUFFER_SIZE STRUCTURE_BYTE_SIZE, UINT NUM_ELEMENTS);
	D3D12_UNORDERED_ACCESS_VIEW_DESC SetUnorderedAccessTextureView(BUFFER_SIZE STRUCTURE_BYTE_SIZE, UINT NUM_ELEMENTS);
	D3D12_UNORDERED_ACCESS_VIEW_DESC SetUnorderedAccess3DTextureView(BUFFER_SIZE arg_structureByteSize, UINT arg_numElements);

	KazBufferHelper::BufferResourceData SetGPUBufferData(BUFFER_SIZE BUFFER_SIZE, const std::string &BUFFER_NAME = "GPUBuffer", std::source_location DRAW_SOURCE_LOCATION = std::source_location::current());
	KazBufferHelper::BufferResourceData SetUploadBufferData(BUFFER_SIZE BUFFER_SIZE, const std::string &BUFFER_NAME = "UploadBuffer", std::source_location DRAW_SOURCE_LOCATION = std::source_location::current());

	template<typename T>
	T GetBufferSize(size_t BUFFER_SIZE, unsigned long long STRUCTURE_SIZE)
	{
		return static_cast<T>(BUFFER_SIZE * static_cast<int>(STRUCTURE_SIZE));
	};



	class ID3D12ResourceWrapper
	{
	public:
		ID3D12ResourceWrapper()
		{};
		ID3D12ResourceWrapper(const KazBufferHelper::BufferResourceData &BUFFER_OPTION)
		{
			CreateBuffer(BUFFER_OPTION);
		};

		ID3D12ResourceWrapper::~ID3D12ResourceWrapper()
		{
			if (isVRAMBufferFlag)
			{
				return;
			}
			for (int i = 0; i < buffer.size(); ++i)
			{
				//buffer[i]->Unmap(0, nullptr);
			}
		}

		/// <summary>
		/// バッファ生成
		/// </summary>
		/// <param name="BUFFER_OPTION">バッファを生成する為に必要な構造体</param>
		void CreateBuffer(const KazBufferHelper::BufferResourceData &BUFFER_OPTION, std::source_location DRAW_SOURCE_LOCATION = std::source_location::current());

		/// <summary>
		/// データをバッファに転送します
		/// </summary>
		/// <param name="DATA">送りたいデータのアドレス</param>
		/// <param name="DATA_SIZE">送りたいデータのサイズ</param>
		void TransData(void *DATA, const unsigned int &DATA_SIZE);
		void TransData(void *DATA, unsigned int START_DATA_SIZE, unsigned int DATA_SIZE);

		/// <summary>
		/// バッファを開放します
		/// </summary>
		/// <param name="HANDLE">開放したいバッファのハンドル</param>
		void Release();

		/// <summary>
		/// バッファのGPUアドレスを受け取ります
		/// </summary>
		/// <param name="HANDLE">ハンドル</param>
		/// <returns>バッファのGPUアドレス</returns>
		D3D12_GPU_VIRTUAL_ADDRESS GetGpuAddress();

		void *GetMapAddres(int BB_INDEX = -1)const;

		void CopyBuffer(
			const Microsoft::WRL::ComPtr<ID3D12Resource> &SRC_BUFFER
		)const;

		void ChangeBarrier(
			D3D12_RESOURCE_STATES BEFORE_STATE, D3D12_RESOURCE_STATES AFTER_STATE
		);
		void ChangeBarrier(
			D3D12_RESOURCE_STATES AFTER_STATE
		);

		void ChangeBarrierUAV()
		{
			for (int i = 0; i < buffer.size(); ++i)
			{
				DirectX12CmdList::Instance()->cmdList->ResourceBarrier(
					1,
					&CD3DX12_RESOURCE_BARRIER::UAV(buffer[i].Get())
				);
			}
		}

		const Microsoft::WRL::ComPtr<ID3D12Resource> &GetBuffer(int INDEX = -1) const;

		void operator=(const ID3D12ResourceWrapper &rhs);

		void CreateViewHandle(std::vector<RESOURCE_HANDLE>HANDLE_ARRAY)
		{
			viewHandle = HANDLE_ARRAY;
		}
		void CreateViewHandle(RESOURCE_HANDLE HANDLE)
		{
			viewHandle.emplace_back(HANDLE);
		}
		const RESOURCE_HANDLE &GetViewHandle()const
		{
			return viewHandle[0];
		}

		const D3D12_RESOURCE_STATES &GetState()
		{
			buffer[0]->GetDesc().Format;
			return resourceState;
		}
	private:
		static const int BACK_BUFFER_NUM = 1;
		std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, BACK_BUFFER_NUM>buffer;
		std::array<void *, BACK_BUFFER_NUM>bufferMapPtr;
		std::vector<RESOURCE_HANDLE> viewHandle;
		bool isVRAMBufferFlag;
		D3D12_RESOURCE_STATES resourceState;
		std::string bufferName;

		UINT GetIndex()const
		{
			return 0;
		}
	};

	struct BufferData
	{
		std::shared_ptr<ID3D12ResourceWrapper> bufferWrapper;
		std::shared_ptr<ID3D12ResourceWrapper> counterWrapper;
		GraphicsRangeType rangeType;
		GraphicsRootParamType rootParamType;
		UINT structureSize;
		UINT elementNum;


		BufferData(const KazBufferHelper::BufferResourceData &BUFFER_DATA);
		BufferData(const KazBufferHelper::BufferResourceData &BUFFER_DATA, GraphicsRangeType RANGE, GraphicsRootParamType PARAM);
		BufferData();
		~BufferData()
		{
		};

		void GenerateCounterBuffer();
		void CreateUAVView();

		void operator=(const BufferData &rhs);
	private:
	};

	//RAMからVRAMにアップロードしやすくした構造体です
	struct RAMToVRAMBufferData
	{
		RAMToVRAMBufferData(int arg_structureSize, int arg_elementNum)
		{
			m_ramBuffer = KazBufferHelper::SetUploadBufferData(arg_structureSize * arg_elementNum);
			m_ramBuffer.structureSize = arg_structureSize;
			m_ramBuffer.elementNum = arg_elementNum;

			m_vramBuffer = KazBufferHelper::SetGPUBufferData(arg_structureSize * arg_elementNum);
			m_vramBuffer.structureSize = arg_structureSize;
			m_vramBuffer.elementNum = arg_elementNum;
		};
		RAMToVRAMBufferData()
		{};
		void GenerateBuffer(int arg_structureSize, int arg_elementNum)
		{
			m_ramBuffer = KazBufferHelper::SetUploadBufferData(arg_structureSize * arg_elementNum);
			m_ramBuffer.structureSize = arg_structureSize;
			m_ramBuffer.elementNum = arg_elementNum;

			m_vramBuffer = KazBufferHelper::SetGPUBufferData(arg_structureSize * arg_elementNum);
			m_vramBuffer.structureSize = arg_structureSize;
			m_vramBuffer.elementNum = arg_elementNum;
		};
		void TransData(void *arg_dataPtr, const unsigned int &arg_dataSize)
		{
			m_ramBuffer.bufferWrapper->TransData(arg_dataPtr, arg_dataSize);
			m_vramBuffer.bufferWrapper->CopyBuffer(m_ramBuffer.bufferWrapper->GetBuffer());
		}

		KazBufferHelper::BufferData m_ramBuffer, m_vramBuffer;
	};
}

