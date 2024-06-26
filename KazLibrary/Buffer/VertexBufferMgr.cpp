#include "VertexBufferMgr.h"
#include"DescriptorHeapMgr.h"

RESOURCE_HANDLE VertexBufferMgr::GenerateBuffer(const VertexGenerateData &arg_vertexData, bool arg_generateInVRAMFlag, std::string arg_bufferName)
{
	RESOURCE_HANDLE outputHandle = m_vertexHandle.GetHandle();
	bool pushBackFlag = false;
	if (m_drawDataArray.size() <= outputHandle)
	{
		m_drawDataArray.emplace_back();
		m_polygonBufferArray.emplace_back(PolygonGenerateData(arg_vertexData.verticesPos, arg_vertexData.structureSize, arg_vertexData.arraySize));
		pushBackFlag = true;
	}
	else
	{
		m_polygonBufferArray[outputHandle] = PolygonGenerateData(arg_vertexData.verticesPos, arg_vertexData.structureSize, arg_vertexData.arraySize, arg_bufferName);
	}

	std::shared_ptr<KazBufferHelper::BufferData>vertexBuffer;
	if (arg_generateInVRAMFlag)
	{
		vertexBuffer = m_polygonBufferArray[outputHandle].m_gpuBuffer.m_buffer;
	}
	else
	{
		vertexBuffer = m_polygonBufferArray[outputHandle].m_cpuBuffer.m_buffer;
	}
	vertexBuffer->structureSize = arg_vertexData.structureSize;
	vertexBuffer->elementNum = static_cast<UINT>(arg_vertexData.arraySize);
	m_drawDataArray[outputHandle].vertBuffer = vertexBuffer;

	//頂点情報
	KazRenderHelper::IASetVertexBuffersData setVertDataArray;
	setVertDataArray.numViews = 1;
	setVertDataArray.slot = 0;
	setVertDataArray.vertexBufferView = KazBufferHelper::SetVertexBufferView(vertexBuffer->bufferWrapper->GetGpuAddress(), KazBufferHelper::GetBufferSize<BUFFER_SIZE>(arg_vertexData.arraySize, arg_vertexData.structureSize), arg_vertexData.oneArraySize);


	//頂点ビュー
	RESOURCE_HANDLE handle = DescriptorHeapMgr::Instance()->GetSize(DESCRIPTORHEAP_MEMORY_IAPOLYGONE).startSize + sDescHandle;
	D3D12_SHADER_RESOURCE_VIEW_DESC view;
	view.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	view.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	view.Format = DXGI_FORMAT_UNKNOWN;
	view.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	view.Buffer.NumElements = static_cast<UINT>(arg_vertexData.arraySize);
	view.Buffer.FirstElement = 0;
	view.Buffer.StructureByteStride = arg_vertexData.structureSize;
	DescriptorHeapMgr::Instance()->CreateBufferView(handle, view, vertexBuffer->bufferWrapper->GetBuffer().Get());
	m_drawDataArray[outputHandle].vertBuffer->bufferWrapper->CreateViewHandle(handle);
	++sDescHandle;


	m_drawDataArray[outputHandle].instanceData.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_drawDataArray[outputHandle].instanceData.vertexBufferDrawData = setVertDataArray;
	//描画コマンド情報
	KazRenderHelper::DrawInstancedData result;
	result.vertexCountPerInstance = static_cast<UINT>(arg_vertexData.arraySize);
	result.instanceCount = 1;
	result.startInstanceLocation = 0;
	result.startVertexLocation = 0;
	m_drawDataArray[outputHandle].instanceData.drawInstanceData = result;

	return outputHandle;
}

RESOURCE_HANDLE VertexBufferMgr::GenerateBuffer(const std::vector<VertexAndIndexGenerateData, STLAllocator<VertexAndIndexGenerateData>> &arg_vertexData, bool arg_generateInVRAMFlag, std::string arg_bufferName)
{
	//配列の中から設定する要素を決める--------------------------------
	RESOURCE_HANDLE outputHandle = m_handle.GetHandle();
	bool pushBackFlag = false;
	if (m_drawIndexDataArray.size() <= outputHandle)
	{
		m_drawIndexDataArray.emplace_back();
		m_polygonIndexBufferArray.emplace_back();
		pushBackFlag = true;
	}
	std::vector<std::vector<IAPolygonBufferData, STLAllocator<IAPolygonBufferData>>> *polygonBuffer = &m_polygonIndexBufferArray[outputHandle];
	PolygonMultiMeshedIndexData *polygonMeshData = &m_drawIndexDataArray[outputHandle];
	//配列の中から設定する要素を決める--------------------------------

	//設定開始--------------------------------
	polygonMeshData->index.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	for (const auto &meshData : arg_vertexData)
	{
		//頂点バッファ生成
		polygonBuffer->emplace_back();
		polygonBuffer->back().emplace_back();
		GenerateIABuffer(&polygonBuffer->back().back(), { meshData.verticesPos,meshData.structureSize, meshData.arraySize }, arg_bufferName);
		if (arg_generateInVRAMFlag)
		{
			polygonMeshData->vertBuffer.emplace_back(polygonBuffer->back().back().m_gpuBuffer.m_buffer);
		}
		else
		{
			polygonMeshData->vertBuffer.emplace_back(polygonBuffer->back().back().m_cpuBuffer.m_buffer);
		}
		//頂点バッファの描画情報の設定
		polygonMeshData->index.vertexBufferDrawData.emplace_back(
			0,
			1,
			KazBufferHelper::SetVertexBufferView(
				polygonBuffer->back().back().m_gpuBuffer.m_buffer->bufferWrapper->GetGpuAddress(),
				KazBufferHelper::GetBufferSize<BUFFER_SIZE>(meshData.arraySize, meshData.structureSize),
				meshData.oneArraySize)
		);

		//インデックスバッファ生成
		polygonBuffer->back().emplace_back();
		GenerateIABuffer(&polygonBuffer->back().back(), { (void *)meshData.indices.data(),sizeof(UINT), meshData.indices.size() }, arg_bufferName);
		polygonMeshData->indexBuffer.emplace_back(polygonBuffer->back().back().m_gpuBuffer.m_buffer);
		//インデックスの描画情報の設定
		polygonMeshData->index.indexBufferView.emplace_back(
			KazBufferHelper::SetIndexBufferView(
				polygonBuffer->back().back().m_gpuBuffer.m_buffer->bufferWrapper->GetGpuAddress(),
				KazBufferHelper::GetBufferSize<BUFFER_SIZE>(meshData.indices.size(), sizeof(UINT))
			)
		);

		//描画コマンド情報
		polygonMeshData->index.drawIndexInstancedData.emplace_back(SetIndexInstanceData(meshData.indices.size()));


		D3D12_SHADER_RESOURCE_VIEW_DESC view = SetView(polygonMeshData->vertBuffer.back()->structureSize, polygonMeshData->vertBuffer.back()->elementNum);
		//頂点バッファビュー生成
		SetInDescriptorHeap(polygonMeshData->vertBuffer.back(), view);
		//インデックスバッファビュー生成
		view = SetView(static_cast<UINT>(meshData.indices.size()), sizeof(UINT));
		SetInDescriptorHeap(polygonMeshData->indexBuffer.back(), view);
	}
	return outputHandle;
}

RESOURCE_HANDLE VertexBufferMgr::GeneratePlaneBuffer()
{
	//頂点バッファ生成---------------------------------------
	struct VertUvData
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 uv;
	};
	std::vector<DirectX::XMFLOAT3, STLAllocator<DirectX::XMFLOAT3>> lVertices = GetPlaneVertices(
		KazMath::Vec2<float>(0.5f, 0.5f),
		KazMath::Vec2<float>(1.0f, 1.0f),
		KazMath::Vec2<int>(1, 1)
	);
	std::vector<DirectX::XMFLOAT2, STLAllocator<DirectX::XMFLOAT2>> lUv(4);
	KazRenderHelper::InitUvPos(&lUv[0], &lUv[1], &lUv[2], &lUv[3]);
	std::vector<VertUvData, STLAllocator<VertUvData>> lVertUv;
	for (int i = 0; i < lVertices.size(); ++i)
	{
		lVertUv.emplace_back();
		lVertUv[i].pos = { lVertices[i].x,lVertices[i].y,lVertices[i].z };
		lVertUv[i].uv = lUv[i];
	}
	std::vector<UINT, STLAllocator<UINT>> lIndices;
	for (int i = 0; i < 6; ++i)
	{
		lIndices.emplace_back(KazRenderHelper::InitIndciesForPlanePolygon()[i]);
	}

	VertexAndIndexGenerateData generateData(lVertUv.data(), sizeof(VertUvData), lVertUv.size(), sizeof(lVertUv[0]), lIndices);
	//頂点バッファ生成---------------------------------------



	RESOURCE_HANDLE outputHandle = m_handle.GetHandle();
	bool pushBackFlag = false;
	if (m_drawIndexDataArray.size() <= outputHandle)
	{
		m_drawIndexDataArray.emplace_back();
		m_polygonIndexBufferArray.emplace_back();
		pushBackFlag = true;
	}
	m_polygonIndexBufferArray[outputHandle].emplace_back();
	m_polygonIndexBufferArray[outputHandle].back().emplace_back(PolygonGenerateData(generateData.verticesPos, generateData.structureSize, generateData.arraySize));
	m_polygonIndexBufferArray[outputHandle].back().emplace_back(PolygonGenerateData((void *)generateData.indices.data(), sizeof(UINT), generateData.indices.size()));


	std::shared_ptr<KazBufferHelper::BufferData>vertexBuffer(m_polygonIndexBufferArray[outputHandle].back()[0].m_gpuBuffer.m_buffer);
	std::shared_ptr<KazBufferHelper::BufferData>indexBuffer(m_polygonIndexBufferArray[outputHandle].back()[1].m_gpuBuffer.m_buffer);

	vertexBuffer->structureSize = generateData.structureSize;
	vertexBuffer->elementNum = static_cast<UINT>(generateData.arraySize);
	indexBuffer->structureSize = sizeof(UINT);
	indexBuffer->elementNum = static_cast<UINT>(generateData.indices.size());


	std::vector<KazRenderHelper::IASetVertexBuffersData> setVertDataArray;
	std::vector<D3D12_INDEX_BUFFER_VIEW> indexBufferViewArray;
	std::vector<KazRenderHelper::DrawIndexedInstancedData>drawCommandDataArray;

	//頂点情報
	setVertDataArray.emplace_back();
	setVertDataArray.back().numViews = 1;
	setVertDataArray.back().slot = 0;
	setVertDataArray.back().vertexBufferView =
		KazBufferHelper::SetVertexBufferView(
			vertexBuffer->bufferWrapper->GetGpuAddress(),
			KazBufferHelper::GetBufferSize<BUFFER_SIZE>(4, sizeof(PolygonBuffer::VertUvData)),
			sizeof(PolygonBuffer::VertUvData)
		);

	//インデックス情報
	indexBufferViewArray.emplace_back(
		KazBufferHelper::SetIndexBufferView(
			indexBuffer->bufferWrapper->GetGpuAddress(),
			KazBufferHelper::GetBufferSize<BUFFER_SIZE>(6, sizeof(UINT))
		)
	);

	//描画コマンド情報
	KazRenderHelper::DrawIndexedInstancedData result;
	result.indexCountPerInstance = static_cast<UINT>(6);
	result.instanceCount = 1;
	result.startIndexLocation = 0;
	result.baseVertexLocation = 0;
	result.startInstanceLocation = 0;
	drawCommandDataArray.emplace_back(result);

	m_drawIndexDataArray[outputHandle].vertBuffer.emplace_back(vertexBuffer);
	m_drawIndexDataArray[outputHandle].indexBuffer.emplace_back(indexBuffer);
	m_drawIndexDataArray[outputHandle].index.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_drawIndexDataArray[outputHandle].index.vertexBufferDrawData = setVertDataArray;
	m_drawIndexDataArray[outputHandle].index.indexBufferView = indexBufferViewArray;
	m_drawIndexDataArray[outputHandle].index.drawIndexInstancedData = drawCommandDataArray;

	return outputHandle;
}

RESOURCE_HANDLE VertexBufferMgr::GenerateBoxBuffer()
{
	m_vertexBufferArray.emplace_back();
	m_vertexBufferArray.back().emplace_back(std::make_unique<PolygonBuffer>());
	PolygonIndexData index = m_vertexBufferArray.back().back()->GenerateBoxBuffer(1.0f);


	std::shared_ptr<KazBufferHelper::BufferData>vertexBuffer(index.vertBuffer);
	std::shared_ptr<KazBufferHelper::BufferData>indexBuffer(index.indexBuffer);


	std::vector<KazRenderHelper::IASetVertexBuffersData> setVertDataArray;
	std::vector<D3D12_INDEX_BUFFER_VIEW> indexBufferViewArray;
	std::vector<KazRenderHelper::DrawIndexedInstancedData>drawCommandDataArray;

	//頂点情報
	setVertDataArray.emplace_back();
	setVertDataArray.back().numViews = 1;
	setVertDataArray.back().slot = 0;
	setVertDataArray.back().vertexBufferView =
		KazBufferHelper::SetVertexBufferView(
			vertexBuffer->bufferWrapper->GetGpuAddress(),
			KazBufferHelper::GetBufferSize<BUFFER_SIZE>(4, sizeof(PolygonBuffer::VertUvData)),
			sizeof(PolygonBuffer::VertUvData)
		);

	//インデックス情報
	indexBufferViewArray.emplace_back(
		KazBufferHelper::SetIndexBufferView(
			indexBuffer->bufferWrapper->GetGpuAddress(),
			KazBufferHelper::GetBufferSize<BUFFER_SIZE>(6, sizeof(UINT))
		)
	);

	//描画コマンド情報
	KazRenderHelper::DrawIndexedInstancedData result;
	result.indexCountPerInstance = static_cast<UINT>(6);
	result.instanceCount = 1;
	result.startIndexLocation = 0;
	result.baseVertexLocation = 0;
	result.startInstanceLocation = 0;
	drawCommandDataArray.emplace_back(result);

	m_drawIndexDataArray.emplace_back();
	m_drawIndexDataArray.back().vertBuffer.emplace_back(vertexBuffer);
	m_drawIndexDataArray.back().indexBuffer.emplace_back(indexBuffer);
	m_drawIndexDataArray.back().index.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_drawIndexDataArray.back().index.vertexBufferDrawData = setVertDataArray;
	m_drawIndexDataArray.back().index.indexBufferView = indexBufferViewArray;
	m_drawIndexDataArray.back().index.drawIndexInstancedData = drawCommandDataArray;

	RESOURCE_HANDLE outputHandle = m_handle.GetHandle();
	return outputHandle;
}

RESOURCE_HANDLE VertexBufferMgr::StackVertexBuffer(const std::shared_ptr<KazBufferHelper::BufferData> &arg_vertexBuffer, const std::shared_ptr<KazBufferHelper::BufferData> &arg_indexBuffer)
{
	RESOURCE_HANDLE outputHandle = m_handle.GetHandle();
	if (m_drawIndexDataArray.size() <= outputHandle)
	{
		m_drawIndexDataArray.emplace_back();
		m_polygonIndexBufferArray.emplace_back();
	}
	m_drawIndexDataArray[outputHandle].vertBuffer.emplace_back(arg_vertexBuffer);

	RESOURCE_HANDLE handle = DescriptorHeapMgr::Instance()->GetSize(DESCRIPTORHEAP_MEMORY_IAPOLYGONE).startSize + sDescHandle;
	D3D12_SHADER_RESOURCE_VIEW_DESC view;
	view.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	view.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	view.Format = DXGI_FORMAT_UNKNOWN;
	view.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	view.Buffer.NumElements = static_cast<UINT>(m_drawIndexDataArray[outputHandle].vertBuffer[0]->elementNum);
	view.Buffer.FirstElement = 0;
	view.Buffer.StructureByteStride = m_drawIndexDataArray[outputHandle].vertBuffer[0]->structureSize;
	DescriptorHeapMgr::Instance()->CreateBufferView(handle, view, m_drawIndexDataArray[outputHandle].vertBuffer[0]->bufferWrapper->GetBuffer().Get());
	m_drawIndexDataArray[outputHandle].vertBuffer[0]->bufferWrapper->CreateViewHandle(handle);
	++sDescHandle;


	handle = DescriptorHeapMgr::Instance()->GetSize(DESCRIPTORHEAP_MEMORY_IAPOLYGONE).startSize + sDescHandle;
	view.Buffer.NumElements = static_cast<UINT>(arg_indexBuffer->elementNum);
	view.Buffer.StructureByteStride = sizeof(UINT);
	DescriptorHeapMgr::Instance()->CreateBufferView(handle, view, arg_indexBuffer->bufferWrapper->GetBuffer().Get());
	m_drawIndexDataArray[outputHandle].indexBuffer.emplace_back(arg_indexBuffer);
	m_drawIndexDataArray[outputHandle].indexBuffer.back()->bufferWrapper->CreateViewHandle(handle);
	++sDescHandle;

	return outputHandle;
}

void VertexBufferMgr::ReleaseVeretexIndexBuffer(RESOURCE_HANDLE HANDLE)
{
	m_handle.DeleteHandle(HANDLE);
	m_drawIndexDataArray[HANDLE].index.Finalize();
	for (auto &vertBuffer : m_drawIndexDataArray[HANDLE].vertBuffer)
	{
		vertBuffer.reset();
	}
	for (auto &indexBuffer : m_drawIndexDataArray[HANDLE].indexBuffer)
	{
		indexBuffer.reset();
	}
}

void VertexBufferMgr::ReleaseVeretexBuffer(RESOURCE_HANDLE HANDLE)
{
	m_vertexHandle.DeleteHandle(HANDLE);
	m_drawDataArray[HANDLE].vertBuffer.reset();
}

PolygonMultiMeshedIndexData VertexBufferMgr::GetVertexIndexBuffer(RESOURCE_HANDLE HANDLE)
{
	return m_drawIndexDataArray[HANDLE];
}

PolygonInstanceData VertexBufferMgr::GetVertexBuffer(RESOURCE_HANDLE HANDLE)
{
	return m_drawDataArray[HANDLE];
}

std::vector<DirectX::XMFLOAT3, STLAllocator<DirectX::XMFLOAT3>> VertexBufferMgr::GetPlaneVertices(const KazMath::Vec2<float> &anchorPoint, const KazMath::Vec2<float> &scale, const KazMath::Vec2<int> &texSize)
{
	std::vector<DirectX::XMFLOAT3, STLAllocator<DirectX::XMFLOAT3>> vertices(4);
	KazRenderHelper::InitVerticesPos(&vertices[0], &vertices[1], &vertices[2], &vertices[3], anchorPoint.ConvertXMFLOAT2());

	KazMath::Vec2<float>leftUpPos(vertices[0].x, vertices[0].y);
	KazMath::Vec2<float>rightDownPos(vertices[3].x, vertices[3].y);

	std::array<KazMath::Vec2<float>, 4> vertArray = KazRenderHelper::ChangePlaneScale(leftUpPos, rightDownPos, scale, {}, texSize);

	for (int i = 0; i < vertArray.size(); ++i)
	{
		vertices[i] = { vertArray[i].x,vertArray[i].y,0.0f };
	}
	return vertices;
}

void VertexBufferMgr::GenerateIABuffer(IAPolygonBufferData *arg_bufferData, const PolygonGenerateData &arg_generateBufferData, std::string arg_bufferName)
{
	arg_bufferData->GenerateBuffer(PolygonGenerateData(arg_generateBufferData.m_ptr, arg_generateBufferData.m_structureSize, arg_generateBufferData.m_arraySize, arg_bufferName));

	//RAM
	arg_bufferData->m_cpuBuffer.m_buffer->elementNum = static_cast<UINT>(arg_generateBufferData.m_arraySize);
	arg_bufferData->m_cpuBuffer.m_buffer->structureSize = static_cast<UINT>(arg_generateBufferData.m_structureSize);
	//VRAM
	arg_bufferData->m_gpuBuffer.m_buffer->elementNum = static_cast<UINT>(arg_generateBufferData.m_arraySize);
	arg_bufferData->m_gpuBuffer.m_buffer->structureSize = static_cast<UINT>(arg_generateBufferData.m_structureSize);
}

void VertexBufferMgr::GenerateBufferView(const D3D12_SHADER_RESOURCE_VIEW_DESC &arg_view)
{
}

void VertexBufferMgr::SetInDescriptorHeap(const std::shared_ptr<KazBufferHelper::BufferData> &arg_bufferData, const D3D12_SHADER_RESOURCE_VIEW_DESC &arg_viewDesc)
{
	//頂点バッファビューの生成
	RESOURCE_HANDLE handle = DescriptorHeapMgr::Instance()->GetSize(DESCRIPTORHEAP_MEMORY_IAPOLYGONE).startSize + sDescHandle;
	DescriptorHeapMgr::Instance()->CreateBufferView(handle, arg_viewDesc, arg_bufferData->bufferWrapper->GetBuffer().Get());
	arg_bufferData->bufferWrapper->CreateViewHandle(handle);
	++sDescHandle;
}

D3D12_SHADER_RESOURCE_VIEW_DESC VertexBufferMgr::SetView(UINT arg_structureByte, UINT arg_numElement)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC view;
	view.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	view.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	view.Format = DXGI_FORMAT_UNKNOWN;
	view.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	view.Buffer.NumElements = arg_numElement;
	view.Buffer.FirstElement = 0;
	view.Buffer.StructureByteStride = arg_structureByte;
	return view;
}

KazRenderHelper::DrawIndexedInstancedData VertexBufferMgr::SetIndexInstanceData(size_t arg_indexArrayNum)
{
	KazRenderHelper::DrawIndexedInstancedData result;
	result.indexCountPerInstance = static_cast<UINT>(arg_indexArrayNum);
	result.instanceCount = 1;
	result.startIndexLocation = 0;
	result.baseVertexLocation = 0;
	result.startInstanceLocation = 0;
	return result;
}
