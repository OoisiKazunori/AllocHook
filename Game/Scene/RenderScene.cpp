#include "RenderScene.h"
#include"../KazLibrary/Imgui/MyImgui.h"
#include"../KazLibrary/Helper/MemoryProfilerData.h"
#include"../Game/Debug/MemoryProfiler.h"
#include"../KazLibrary/Input/KeyBoradInputManager.h"

RenderScene::RenderScene(DrawingByRasterize &arg_rasterize) :
	m_sceneNum(-1),
	m_sponzaModelRender(arg_rasterize, "Resource/DefferdRendering/Sponza/", "Sponza.gltf")
{
	m_sponzaModelTransform.scale = { 0.1f,0.1f,0.1f };
	m_sponzaModelTransform.Rotation(KazMath::Vec3<float>(0.0f, 1.0f, 0.0f), KazMath::AngleToRadian(90.0f));

	//書き込まれているGBufferを入手する
	for (int i = 0; i < GBUFFER_MAX; ++i)
	{
		m_gBufferRender[i].m_tex.Load(
			arg_rasterize,
			RenderTargetStatus::Instance()->GetBuffer(GBufferMgr::Instance()->GetRenderTarget()[i]),
			true
		);
	}

	m_alphaModel.Load(arg_rasterize, "Resource/DefferdRendering/AlphaSphere/", "Sphere.gltf", false);
	m_alphaModel.m_model.m_drawCommand.depthHandle = 0;


	const std::string filePass = "Resource/DefferdRendering/";
	std::array<std::string, MODEL_MAX_NUM> modelFilePassArray;
	modelFilePassArray[0] = filePass + "Avocado/";
	modelFilePassArray[1] = filePass + "BoomBox/";
	modelFilePassArray[2] = filePass + "Corset/";
	modelFilePassArray[3] = filePass + "WaterBottle/";
	modelFilePassArray[4] = filePass + "BarramundiFish/";
	modelFilePassArray[5] = filePass + "Lantern/";
	modelFilePassArray[6] = filePass + "AntiqueCamera/";
	std::array<std::string, MODEL_MAX_NUM> modelFileNameArray;
	modelFileNameArray[0] = "Avocado.gltf";
	modelFileNameArray[1] = "BoomBox.gltf";
	modelFileNameArray[2] = "Corset.gltf";
	modelFileNameArray[3] = "WaterBottle.gltf";
	modelFileNameArray[4] = "BarramundiFish.gltf";
	modelFileNameArray[5] = "Lantern.gltf";
	modelFileNameArray[6] = "AntiqueCamera.gltf";
	std::array<float, MODEL_MAX_NUM>scaleArray;
	scaleArray[0] = 100.0f;
	scaleArray[1] = 500.0f;
	scaleArray[2] = 100.0f;
	scaleArray[3] = 25.0f;
	scaleArray[4] = 10.0f;
	scaleArray[5] = 0.5f;
	scaleArray[6] = 1.0f;
	int index = 0;
	//モデル生成
	for (int z = 0; z < m_models.size(); ++z)
	{
		KazMath::Transform3D transform(
			KazMath::Vec3<float>(-55.0f + static_cast<float>(z) * 17.0f, 8.5f, -105.0f),
			KazMath::Vec3<float>(scaleArray[index], scaleArray[index], scaleArray[index])
		);
		m_models[z].Load(
			arg_rasterize,
			modelFilePassArray[index],
			modelFileNameArray[index],
			transform
		);
		++index;
	}
	//ライト用のモデル生成
	for (int z = 0; z < m_lights.size(); ++z)
	{
		KazMath::Transform3D transform(
			KazMath::Vec3<float>(-55.0f + static_cast<float>(z) * 20.0f, 2.0f, -105.0f),
			KazMath::Vec3<float>(1.0f, 1.0f, 1.0f)
		);
		m_lights[z].Load(
			arg_rasterize,
			"Resource/DefferdRendering/BoxTextured/",
			"BoxTextured.gltf",
			transform
		);
	}
	m_renderTransform.pos = { WIN_X / 2,WIN_Y / 2 };


	//ライト座標をVRAMに上げる--------------------------------
	m_uploadLightBuffer = KazBufferHelper::SetUploadBufferData(
		KazBufferHelper::GetBufferSize<BUFFER_SIZE>(
			m_lights[0].GetPosArray().size() * m_lights.size(),
			sizeof(DirectX::XMFLOAT3))
	);
	m_defaultLightBuffer = KazBufferHelper::SetGPUBufferData(
		KazBufferHelper::GetBufferSize<BUFFER_SIZE>(
			m_lights[0].GetPosArray().size() * m_lights.size(),
			sizeof(DirectX::XMFLOAT3))
	);

	std::vector<KazMath::Vec3<float>>lightPosArray;
	for (int i = 0; i < m_lights.size(); ++i)
	{
		for (int posIndex = 0; posIndex < m_lights[i].GetPosArray().size(); ++posIndex)
		{
			lightPosArray.emplace_back(m_lights[i].GetPosArray()[posIndex]);
		}
	}
	m_uploadLightBuffer.bufferWrapper->TransData(lightPosArray.data(), KazBufferHelper::GetBufferSize<unsigned int>(m_lights[0].GetPosArray().size() * m_lights.size(), sizeof(DirectX::XMFLOAT3)));
	m_defaultLightBuffer.bufferWrapper->CopyBuffer(m_uploadLightBuffer.bufferWrapper->GetBuffer());
	m_defaultLightBuffer.bufferWrapper->ChangeBarrierUAV();
	//ライト座標をVRAMに上げる--------------------------------


	DrawFuncData::PipelineGenerateData pipelineData;
	pipelineData.desc = DrawFuncPipelineData::SetTex();
	pipelineData.desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "DeferredShading/" + "GBufferDrawFinal.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
	pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "DeferredShading/" + "GBufferDrawFinal.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);
	pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::NONE;
	DrawFuncData::DrawCallData drawCall = DrawFuncData::SetSpriteAlphaData(pipelineData);
	//ライトの配列数
	drawCall.extraBufferArray[1].~BufferData();
	drawCall.extraBufferArray[1] = KazBufferHelper::SetConstBufferData(sizeof(int));
	drawCall.extraBufferArray[1].rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
	drawCall.extraBufferArray[1].rootParamType = GRAPHICS_PRAMTYPE_DATA2;
	int num = static_cast<int>(m_lights[0].GetPosArray().size() * m_lights.size());
	drawCall.extraBufferArray[1].bufferWrapper->TransData(&num, sizeof(int));
	//ライトの座標
	drawCall.extraBufferArray[2] = m_defaultLightBuffer;
	drawCall.extraBufferArray[2].rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	drawCall.extraBufferArray[2].rootParamType = GRAPHICS_PRAMTYPE_DATA;
	//ALBEDO
	drawCall.extraBufferArray.emplace_back(RenderTargetStatus::Instance()->GetBuffer(GBufferMgr::Instance()->GetRenderTarget()[GBufferMgr::ALBEDO]));
	drawCall.extraBufferArray[3].rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
	drawCall.extraBufferArray[3].rootParamType = GRAPHICS_PRAMTYPE_DATA;
	//NORMAL
	drawCall.extraBufferArray.emplace_back(RenderTargetStatus::Instance()->GetBuffer(GBufferMgr::Instance()->GetRenderTarget()[GBufferMgr::NORMAL]));
	drawCall.extraBufferArray[4].rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
	drawCall.extraBufferArray[4].rootParamType = GRAPHICS_PRAMTYPE_DATA2;
	//WORLD
	drawCall.extraBufferArray.emplace_back(RenderTargetStatus::Instance()->GetBuffer(GBufferMgr::Instance()->GetRenderTarget()[GBufferMgr::WORLD]));
	drawCall.extraBufferArray[5].rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
	drawCall.extraBufferArray[5].rootParamType = GRAPHICS_PRAMTYPE_DATA3;
	//ライトの状態
	drawCall.extraBufferArray.emplace_back(KazBufferHelper::SetConstBufferData(sizeof(LightData)));
	drawCall.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
	drawCall.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA3;
	m_finalRender.Load(arg_rasterize, drawCall, true);

	//ImGuiの項目で使用する変数の初期化--------------------------------
	m_gBufferType = GBUFFER_MAX;
	m_drawLightFlag = true;
	m_drawLightPosFlag = false;
	m_alphaTransform.scale = { 10.0f,10.0f,10.0f };
}

RenderScene::~RenderScene()
{
}

void RenderScene::Init()
{
}

void RenderScene::PreInit()
{
}

void RenderScene::Finalize()
{
}

void RenderScene::Input()
{
	if (KeyBoradInputManager::Instance()->InputTrigger(DIK_SPACE))
	{
		//メモリプロファイラー用
		MemoryProfiler::Instance()->Generate(
			BufferMemoryData(
				"Avocad",
				KazMath::Rand<int32_t>(20, 10),
				static_cast<BufferType>(KazMath::Rand<int>(MAX, 0)),
				D3D12_RESOURCE_DESC(),
				std::source_location::current()
			)
		);
	}
}

void RenderScene::Update(DrawingByRasterize &arg_rasterize)
{
	m_camera.Update();

	ImGui::Begin("DemoInspector");
	ImGui::RadioButton("GBuffer-Albedo", &m_gBufferType, 0);
	ImGui::RadioButton("GBuffer-Normal", &m_gBufferType, 1);
	ImGui::RadioButton("GBuffer-World", &m_gBufferType, 3);
	ImGui::RadioButton("GBuffer-Final", &m_gBufferType, 4);
	ImGui::DragFloat("LightRadius", &m_lightData.m_lightRadius);
	ImGui::Checkbox("DrawLight", &m_drawLightFlag);
	ImGui::Checkbox("DrawLightPos", &m_drawLightPosFlag);
	KazImGuiHelper::InputTransform3D("AlphaModel", &m_alphaTransform);
	ImGui::End();

	if (m_drawLightFlag)
	{
		int num = static_cast<int>(m_lights[0].GetPosArray().size() * m_lights.size());
		m_finalRender.m_drawCommand.extraBufferArray[1].bufferWrapper->TransData(&num, sizeof(int));
	}
	else
	{
		int num = 0;
		m_finalRender.m_drawCommand.extraBufferArray[1].bufferWrapper->TransData(&num, sizeof(int));
	}


	m_finalRender.m_drawCommand.extraBufferArray.back().bufferWrapper->TransData(&m_lightData, sizeof(LightData));
}

void RenderScene::Draw(DrawingByRasterize &arg_rasterize, Raytracing::BlasVector &arg_blasVec)
{
	//スポンザの描画
	m_sponzaModelRender.m_model.Draw(arg_rasterize, arg_blasVec, m_sponzaModelTransform);

	//G-Bufferの描画
	if (m_gBufferType == 4)
	{
		//合成結果
		KazMath::Transform2D transform(KazMath::Vec2<float>(1280.0f / 2.0f, 720.0f / 2.0f), KazMath::Vec2<float>(1280.0f, 720.0f));
		DirectX::XMMATRIX mat = transform.GetMat() * CameraMgr::Instance()->GetOrthographicMatProjection();
		m_finalRender.m_drawCommand.extraBufferArray[0].bufferWrapper->TransData(&mat, sizeof(DirectX::XMMATRIX));
		arg_rasterize.UIRender(m_finalRender.m_drawCommandData);
	}
	else
	{
		//レンダーターゲットごとの描画
		m_gBufferRender[m_gBufferType].m_tex.Draw2D(arg_rasterize, m_renderTransform);
	}

	//モデル配置
	for (int z = 0; z < m_models.size(); ++z)
	{
		m_models[z].Draw(arg_rasterize, arg_blasVec);
	}

	//ライトの配置
	if (m_drawLightPosFlag)
	{
		for (int z = 0; z < m_lights.size(); ++z)
		{
			m_lights[z].Draw(arg_rasterize, arg_blasVec);
		}
	}

	//αモデル
	m_alphaModel.m_model.Draw(arg_rasterize, arg_blasVec, m_alphaTransform, KazMath::Color(0, 200, 0, 100));
}

int RenderScene::SceneChange()
{
	if (m_sceneNum != SCENE_NONE)
	{
		int tmp = m_sceneNum;
		m_sceneNum = SCENE_NONE;
		return tmp;
	}
	return SCENE_NONE;
}

void RenderScene::ParallelModels::Load(DrawingByRasterize &arg_rasterize, std::string arg_filePass, std::string arg_fileName, const KazMath::Transform3D &arg_baseTransform)
{
	for (int x = 0; x < m_modelDrawArray.size(); ++x)
	{
		for (int y = 0; y < m_modelDrawArray[x].size(); ++y)
		{
			m_modelDrawArray[x][y].Load(arg_rasterize, arg_filePass, arg_fileName, false);

			m_modelTransformArray[x][y] = arg_baseTransform;
			m_modelTransformArray[x][y].pos.y += static_cast<float>(y) * 20.0f;
			m_modelTransformArray[x][y].pos.z += static_cast<float>(x) * 25.0f;
			//座標記録
			m_posArray[x * Y_ARRAY + y] = m_modelTransformArray[x][y].pos;
		}
	}
}

void RenderScene::ParallelModels::Draw(DrawingByRasterize &arg_rasterize, Raytracing::BlasVector &arg_blasVec)
{
	for (int x = 0; x < m_modelDrawArray.size(); ++x)
	{
		for (int y = 0; y < m_modelDrawArray[x].size(); ++y)
		{
			m_modelDrawArray[x][y].m_model.Draw(arg_rasterize, arg_blasVec, m_modelTransformArray[x][y]);
		}
	}
}

std::array<KazMath::Vec3<float>, RenderScene::ParallelModels::X_ARRAY *RenderScene::ParallelModels::Y_ARRAY> RenderScene::ParallelModels::GetPosArray()
{
	return m_posArray;
}
