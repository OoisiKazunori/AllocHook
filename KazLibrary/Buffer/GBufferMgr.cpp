#include "GBufferMgr.h"
#include"../KazLibrary/Buffer/UavViewHandleMgr.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"RenderTarget/RenderTargetStatus.h"
#include"../KazLibrary/PostEffect/GaussianBlur.h"
#include"../KazLibrary/Helper/Compute.h"
#include"../PostEffect/Outline.h"
#include"../KazLibrary/Easing/easing.h"
#include"../Game/Effect/ChromaticAberration.h"
#include <Helper/Compute.h>

//ワールド座標、ラフネス、メタルネス、スぺキュラ、オブジェクトが反射するか屈折するか(インデックス)、Albedo、法線、カメラ座標(定数バッファでも可能)
GBufferMgr::GBufferMgr()
{
	KazMath::Vec2<UINT>winSize(1280, 720);

	//G-Buffer用のレンダーターゲット生成
	{
		std::vector<MultiRenderTargetData> multiRenderTargetArray(MAX);
		m_gBufferFormatArray.resize(MAX);
		m_gBufferFormatArray[ALBEDO] = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_gBufferFormatArray[NORMAL] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		m_gBufferFormatArray[R_M_S_ID] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		m_gBufferFormatArray[WORLD] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		m_gBufferFormatArray[EMISSIVE] = DXGI_FORMAT_R8G8B8A8_UNORM;

		//アルベド
		multiRenderTargetArray[ALBEDO].backGroundColor = { 0.0f,0.0f,0.0f };
		multiRenderTargetArray[ALBEDO].graphSize = winSize;
		multiRenderTargetArray[ALBEDO].format = m_gBufferFormatArray[ALBEDO];
		multiRenderTargetArray[ALBEDO].bufferName = "GBuffer-Albedo";
		//ノーマル
		multiRenderTargetArray[NORMAL].backGroundColor = { 0.0f,0.0f,0.0f };
		multiRenderTargetArray[NORMAL].graphSize = winSize;
		multiRenderTargetArray[NORMAL].format = m_gBufferFormatArray[NORMAL];
		multiRenderTargetArray[NORMAL].bufferName = "GBuffer-Normal";

		//ラフネス、メタルネス、スぺキュラ、屈折判定(0...反射しない、1...反射する、2...屈折する)
		multiRenderTargetArray[R_M_S_ID].backGroundColor = { 0.0f,0.0f,0.0f };
		multiRenderTargetArray[R_M_S_ID].graphSize = winSize;
		multiRenderTargetArray[R_M_S_ID].format = m_gBufferFormatArray[R_M_S_ID];
		multiRenderTargetArray[R_M_S_ID].bufferName = "GBuffer-R_M_S_ID";
		//ワールド座標
		multiRenderTargetArray[WORLD].backGroundColor = { 0.0f,0.0f,0.0f };
		multiRenderTargetArray[WORLD].graphSize = winSize;
		multiRenderTargetArray[WORLD].format = m_gBufferFormatArray[WORLD];
		multiRenderTargetArray[WORLD].bufferName = "GBuffer-World";
		//エミッシブ
		multiRenderTargetArray[EMISSIVE].backGroundColor = { 0.0f,0.0f,0.0f };
		multiRenderTargetArray[EMISSIVE].graphSize = winSize;
		multiRenderTargetArray[EMISSIVE].format = m_gBufferFormatArray[EMISSIVE];
		multiRenderTargetArray[EMISSIVE].bufferName = "GBuffer-Emissive";

		m_gBufferRenderTargetHandleArray = RenderTargetStatus::Instance()->CreateMultiRenderTarget(multiRenderTargetArray);

		m_finalGBuffer = KazBufferHelper::SetUAVTexBuffer(winSize.x, winSize.y);
		m_finalGBuffer.bufferWrapper->CreateViewHandle(UavViewHandleMgr::Instance()->GetHandle());
		DescriptorHeapMgr::Instance()->CreateBufferView(
			m_finalGBuffer.bufferWrapper->GetViewHandle(),
			KazBufferHelper::SetUnorderedAccessTextureView(sizeof(DirectX::XMFLOAT4), winSize.x * winSize.y),
			m_finalGBuffer.bufferWrapper->GetBuffer().Get()
		);

		m_raytracingGBuffer = KazBufferHelper::SetUAVTexBuffer(winSize.x, winSize.y, DXGI_FORMAT_R8G8B8A8_UNORM);
		m_raytracingGBuffer.bufferWrapper->CreateViewHandle(UavViewHandleMgr::Instance()->GetHandle());
		DescriptorHeapMgr::Instance()->CreateBufferView(
			m_raytracingGBuffer.bufferWrapper->GetViewHandle(),
			KazBufferHelper::SetUnorderedAccessTextureView(sizeof(DirectX::XMFLOAT4), winSize.x * winSize.y),
			m_raytracingGBuffer.bufferWrapper->GetBuffer().Get()
		);

		m_lensFlareLuminanceGBuffer = KazBufferHelper::SetUAVTexBuffer(winSize.x, winSize.y, DXGI_FORMAT_R8G8B8A8_UNORM);
		m_lensFlareLuminanceGBuffer.bufferWrapper->CreateViewHandle(UavViewHandleMgr::Instance()->GetHandle());
		DescriptorHeapMgr::Instance()->CreateBufferView(
			m_lensFlareLuminanceGBuffer.bufferWrapper->GetViewHandle(),
			KazBufferHelper::SetUnorderedAccessTextureView(sizeof(DirectX::XMFLOAT4), winSize.x * winSize.y),
			m_lensFlareLuminanceGBuffer.bufferWrapper->GetBuffer().Get()
		);

		m_emissiveGBuffer = KazBufferHelper::SetUAVTexBuffer(winSize.x, winSize.y, DXGI_FORMAT_R8G8B8A8_UNORM);
		m_emissiveGBuffer.bufferWrapper->CreateViewHandle(UavViewHandleMgr::Instance()->GetHandle());
		DescriptorHeapMgr::Instance()->CreateBufferView(
			m_emissiveGBuffer.bufferWrapper->GetViewHandle(),
			KazBufferHelper::SetUnorderedAccessTextureView(sizeof(DirectX::XMFLOAT4), winSize.x * winSize.y),
			m_emissiveGBuffer.bufferWrapper->GetBuffer().Get()
		);

		m_backBufferCopyBuffer = KazBufferHelper::SetUAVTexBuffer(winSize.x, winSize.y, DXGI_FORMAT_R8G8B8A8_UNORM);
		m_backBufferCopyBuffer.bufferWrapper->CreateViewHandle(UavViewHandleMgr::Instance()->GetHandle());
		DescriptorHeapMgr::Instance()->CreateBufferView(
			m_backBufferCopyBuffer.bufferWrapper->GetViewHandle(),
			KazBufferHelper::SetUnorderedAccessTextureView(sizeof(DirectX::XMFLOAT4), winSize.x * winSize.y),
			m_backBufferCopyBuffer.bufferWrapper->GetBuffer().Get()
		);

		m_backBufferCompositeBuffer = KazBufferHelper::SetUAVTexBuffer(winSize.x, winSize.y, DXGI_FORMAT_R8G8B8A8_UNORM);
		m_backBufferCompositeBuffer.bufferWrapper->CreateViewHandle(UavViewHandleMgr::Instance()->GetHandle());
		DescriptorHeapMgr::Instance()->CreateBufferView(
			m_backBufferCompositeBuffer.bufferWrapper->GetViewHandle(),
			KazBufferHelper::SetUnorderedAccessTextureView(sizeof(DirectX::XMFLOAT4), winSize.x * winSize.y),
			m_backBufferCompositeBuffer.bufferWrapper->GetBuffer().Get()
		);

		m_outlineBuffer = KazBufferHelper::SetUAVTexBuffer(winSize.x, winSize.y, DXGI_FORMAT_R8G8B8A8_UNORM);
		m_outlineBuffer.bufferWrapper->CreateViewHandle(UavViewHandleMgr::Instance()->GetHandle());
		DescriptorHeapMgr::Instance()->CreateBufferView(
			m_outlineBuffer.bufferWrapper->GetViewHandle(),
			KazBufferHelper::SetUnorderedAccessTextureView(sizeof(DirectX::XMFLOAT4), winSize.x * winSize.y),
			m_outlineBuffer.bufferWrapper->GetBuffer().Get()
		);
	}


	//以下は他人の処理です--------------------------------

	//レンズフレア用のブラー
	m_lensFlareBlur = std::make_shared<PostEffect::GaussianBlur>(m_lensFlareLuminanceGBuffer);
	m_outline = std::make_shared<PostEffect::Outline>(m_outlineBuffer);

	//レンズフレア合成関連。
	m_lensFlareConposeBuffTexture = KazBufferHelper::SetUAVTexBuffer(1280, 720, DXGI_FORMAT_R8G8B8A8_UNORM);
	m_lensFlareConposeBuffTexture.bufferWrapper->CreateViewHandle(UavViewHandleMgr::Instance()->GetHandle());
	DescriptorHeapMgr::Instance()->CreateBufferView(
		m_lensFlareConposeBuffTexture.bufferWrapper->GetViewHandle(),
		KazBufferHelper::SetUnorderedAccessTextureView(sizeof(DirectX::XMFLOAT4), 1280 * 720),
		m_lensFlareConposeBuffTexture.bufferWrapper->GetBuffer().Get()
	);
	{
		//レンズフレア用のシェーダーを用意。
		std::vector<KazBufferHelper::BufferData>extraBuffer =
		{
			 m_lensFlareConposeBuffTexture,
			 m_lensFlareLuminanceGBuffer,
			 m_raytracingGBuffer,
		};
		extraBuffer[0].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		extraBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_TEX;
		extraBuffer[1].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		extraBuffer[1].rootParamType = GRAPHICS_PRAMTYPE_TEX2;
		extraBuffer[2].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		extraBuffer[2].rootParamType = GRAPHICS_PRAMTYPE_TEX3;
		m_lensFlareComposeShader = std::make_shared<ComputeShader>();
		m_lensFlareComposeShader->Generate(ShaderOptionData(KazFilePathName::RelativeShaderPath + "PostEffect/LensFlare/" + "LensFlareComposeShader.hlsl", "main", "cs_6_4", SHADER_TYPE_COMPUTE), extraBuffer);
	}
	{
		m_chromaticAberrationData = KazBufferHelper::SetConstBufferData(sizeof(KazMath::Vec4<float>));
		//バックバッファ合成用シェーダーを用意。
		std::vector<KazBufferHelper::BufferData>extraBuffer =
		{
			 m_backBufferCopyBuffer,
			 m_raytracingGBuffer,
			 m_backBufferCompositeBuffer,
			 m_chromaticAberrationData,
		};
		extraBuffer[0].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		extraBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_TEX;
		extraBuffer[1].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		extraBuffer[1].rootParamType = GRAPHICS_PRAMTYPE_TEX2;
		extraBuffer[2].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		extraBuffer[2].rootParamType = GRAPHICS_PRAMTYPE_TEX3;
		extraBuffer[3].rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		extraBuffer[3].rootParamType = GRAPHICS_PRAMTYPE_DATA;
		m_backBufferRaytracingCompositeShader = std::make_shared<ComputeShader>();
		m_backBufferRaytracingCompositeShader->Generate(ShaderOptionData(KazFilePathName::RelativeShaderPath + "Raytracing/" + "BackBufferComposeShader.hlsl", "main", "cs_6_4", SHADER_TYPE_COMPUTE), extraBuffer);
	}

	m_cameraPosBuffer = KazBufferHelper::SetConstBufferData(sizeof(CameraEyePosBufferData));
	m_lightBuffer = KazBufferHelper::SetConstBufferData(sizeof(LightConstData));
	m_lightConstData.m_dirLight.m_dir = KazMath::Vec3<float>(0.0f, -0.4f, 0.8f).GetNormal();
	m_lightConstData.m_dirLight.m_isActive = true;
	m_lightConstData.m_pointLight[0].m_isActive = true;
	m_lightConstData.m_pointLight[1].m_isActive = true;
	m_lightConstData.m_pointLight[2].m_isActive = true;
	m_lightConstData.m_pointLight[3].m_isActive = true;
	m_lightConstData.m_pointLight[4].m_isActive = true;
	m_lightConstData.m_pointLight[5].m_isActive = true;
	m_lightConstData.m_pointLight[6].m_isActive = true;
	m_lightConstData.m_pointLight[7].m_isActive = true;
	m_lightConstData.m_pointLight[8].m_isActive = true;
	m_lightConstData.m_pointLight[9].m_isActive = true;
	m_lightConstData.m_pointLight[0].m_pos = { 196.0f, 10.0f, 74.0f };
	m_lightConstData.m_pointLight[1].m_pos = { 146.0f, 10.0f, -106.0f };
	m_lightConstData.m_pointLight[2].m_pos = { 35.0f, 10.0f, -213.0f };
	m_lightConstData.m_pointLight[3].m_pos = { -162.0f, 35.0f, -335.0f };
	m_lightConstData.m_pointLight[4].m_pos = { -505.0f, 20.0f, -380.0f };
	m_lightConstData.m_pointLight[5].m_pos = { -568.0f, 20.0f, -220.0f };
	m_lightConstData.m_pointLight[6].m_pos = { -20.0f, 40.0f, 210.0f };
	m_lightConstData.m_pointLight[7].m_pos = { -217.0f, 35.0f, 38.0f };
	m_lightConstData.m_pointLight[8].m_pos = { -450.0f, 25.0f, -78.0f };
	m_lightConstData.m_pointLight[9].m_pos = { -382.0f, 30.0f, -483.0f };
}

std::vector<RESOURCE_HANDLE> GBufferMgr::GetRenderTarget()
{
	return m_gBufferRenderTargetHandleArray;
}

std::vector<DXGI_FORMAT> GBufferMgr::GetRenderTargetFormat()
{
	return m_gBufferFormatArray;
}

D3D12_GPU_DESCRIPTOR_HANDLE GBufferMgr::GetGPUHandle(BufferType arg_type)
{
	RESOURCE_HANDLE handle = RenderTargetStatus::Instance()->GetBuffer(m_gBufferRenderTargetHandleArray[arg_type]).bufferWrapper->GetViewHandle();
	return DescriptorHeapMgr::Instance()->GetGpuDescriptorView(handle);
}

void GBufferMgr::ComposeBackBuffer()
{
	DispatchData dispatchData;
	dispatchData.x = static_cast<UINT>(1280 / 16) + 1;
	dispatchData.y = static_cast<UINT>(720 / 16) + 1;
	dispatchData.z = static_cast<UINT>(1);
	m_backBufferRaytracingCompositeShader->Compute(dispatchData);
}

void GBufferMgr::ApplyLensFlareBlur()
{
	m_lensFlareBlur->ApplyBlur();
}

void GBufferMgr::ComposeLensFlareAndScene()
{

	//シーン画像のステータスを変更。
	BufferStatesTransition(m_raytracingGBuffer.bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

	//一旦コピーしておく用のテクスチャのステータスを変更。
	BufferStatesTransition(m_lensFlareConposeBuffTexture.bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST);

	//コピーを実行
	DirectX12CmdList::Instance()->cmdList->CopyResource(m_lensFlareConposeBuffTexture.bufferWrapper->GetBuffer().Get(), m_raytracingGBuffer.bufferWrapper->GetBuffer().Get());

	//シーン画像のステータスを元に戻す。
	BufferStatesTransition(m_raytracingGBuffer.bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	//コピー先のステータスを元に戻す。
	BufferStatesTransition(m_lensFlareConposeBuffTexture.bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	//合成を実行。
	DispatchData composeData;
	composeData.x = static_cast<UINT>(1280 / 16) + 1;
	composeData.y = static_cast<UINT>(720 / 16) + 1;
	composeData.z = static_cast<UINT>(1);
	m_lensFlareComposeShader->Compute(composeData);




	//色収差の係数を求める。
	float iroShuusaFacter = EasingMaker(In, Cubic, ChromaticAberration::Instance()->m_facter);

	//係数を決める、
	const float IROSHUUSA_MIN_X = 2.0f;
	const float IROSHUUSA_MIN_Y = 4.0f;
	const float IROSHUUSA_MIN_Z = 8.0f;
	const float IROSHUUSA_MAX_X = 40.0f;
	const float IROSHUUSA_MAX_Y = 80.0f;
	const float IROSHUUSA_MAX_Z = 120.0f;

	//色収差のやつを更新。
	m_iroShuusa.x = IROSHUUSA_MIN_X + (IROSHUUSA_MAX_X - IROSHUUSA_MIN_X) * iroShuusaFacter;
	m_iroShuusa.y = IROSHUUSA_MIN_Y + (IROSHUUSA_MAX_Y - IROSHUUSA_MIN_Y) * iroShuusaFacter;
	m_iroShuusa.z = IROSHUUSA_MIN_Z + (IROSHUUSA_MAX_Z - IROSHUUSA_MIN_Z) * iroShuusaFacter;
	m_iroShuusa.a = 1.0f;
	m_chromaticAberrationData.bufferWrapper->TransData(&m_iroShuusa, sizeof(m_iroShuusa));

}

void GBufferMgr::BufferStatesTransition(ID3D12Resource* arg_resource, D3D12_RESOURCE_STATES arg_before, D3D12_RESOURCE_STATES arg_after)
{

	D3D12_RESOURCE_BARRIER barriers[] = {
		CD3DX12_RESOURCE_BARRIER::Transition(
		arg_resource,
		arg_before,
		arg_after),
	};
	DirectX12CmdList::Instance()->cmdList->ResourceBarrier(_countof(barriers), barriers);

}
