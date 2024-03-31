#include "SceneManager.h"
#include"../Helper/KazHelper.h"
#include"../Sound/SoundManager.h"
#include"../Sound/SoundManager.h"
#include"../Helper/ResourceFilePass.h"
#include"../Buffer/DescriptorHeapMgr.h"
#include"../Fps/FPSManager.h"
#include"../KazLibrary/Input/KeyBoradInputManager.h"
#include"../Game/Effect/ShakeMgr.h"
#include"../Scene/ModelToolScene.h"
#include"../../Game/Effect/StopMgr.h"
#include <Input/ControllerInputManager.h>
#include"../KazLibrary/Sound/SoundManager.h"
#include"../Buffer/UavViewHandleMgr.h"
#include <cstdint>
#include <iostream>
#include <mutex>
#include <thread>
#include"../KazLibrary/Singlton/LoadingBar.h"

SceneManager::SceneManager() :gameFirstInitFlag(false)
{
	SoundManager::Instance()->SettingSoundManager();

	//デモ用のゲームシーンを設定。
	m_nowScene = GetScene(0);
	m_nowScene->Init();
	//シーン遷移を設定
	m_sceneChange = std::make_unique<ChangeScene::SceneChange>(m_rasterize);
	m_rasterize.GeneratePipeline();

	//シーン番号、遷移に関するパラメーターを設定。
	m_nowSceneNumber = -1;
	m_nextSceneNumber = -1;
	itisInArrayFlag = true;
	endGameFlag = false;
	initGameFlag = false;

	//レイトレーシング周りを設定
	Raytracing::HitGroupMgr::Instance()->Setting();
	m_pipelineShaders.push_back({ "Resource/ShaderFiles/RayTracing/RaytracingShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS", L"checkHitRayMS"}, {L"mainCHS", L"mainAnyHit"} });
	int payloadSize = sizeof(float) * 7;
	m_rayPipeline = std::make_unique<Raytracing::RayPipeline>(m_pipelineShaders, Raytracing::HitGroupMgr::DEF, 6, 2, 4, payloadSize, static_cast<int>(sizeof(KazMath::Vec2<float>)), 6);


	//ボリュームテクスチャを生成。
	m_volumeFogTextureBuffer = KazBufferHelper::SetUAV3DTexBuffer(256, 256, 256, DXGI_FORMAT_R8G8B8A8_UNORM);
	m_volumeFogTextureBuffer.bufferWrapper->CreateViewHandle(UavViewHandleMgr::Instance()->GetHandle());
	DescriptorHeapMgr::Instance()->CreateBufferView(
		m_volumeFogTextureBuffer.bufferWrapper->GetViewHandle(),
		KazBufferHelper::SetUnorderedAccess3DTextureView(sizeof(DirectX::XMFLOAT4), 256 * 256 * 256),
		m_volumeFogTextureBuffer.bufferWrapper->GetBuffer().Get()
	);
	//ボリュームノイズパラメーター
	m_noiseParamData = KazBufferHelper::SetConstBufferData(sizeof(NoiseParam));
	//ボリュームノイズのパラメーターを設定
	m_noiseParam.m_timer = 0.0f;
	m_noiseParam.m_windSpeed = 10.00f;
	m_noiseParam.m_windStrength = 1.0f;
	m_noiseParam.m_threshold = 0.42f;
	m_noiseParam.m_skydormScale = 356.0f;
	m_noiseParam.m_octaves = 4;
	m_noiseParam.m_persistence = 0.5f;
	m_noiseParam.m_lacunarity = 2.5f;
	m_noiseParamData.bufferWrapper->TransData(&m_noiseParam, sizeof(NoiseParam));
	//ボリュームノイズ書き込み
	{
		std::vector<KazBufferHelper::BufferData>extraBuffer =
		{
			 m_volumeFogTextureBuffer,
			 m_noiseParamData
		};
		extraBuffer[0].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		extraBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_TEX;
		extraBuffer[1].rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		extraBuffer[1].rootParamType = GRAPHICS_PRAMTYPE_DATA;
		m_volumeNoiseShader.Generate(ShaderOptionData(KazFilePathName::RelativeShaderPath + "Raytracing/" + "Write3DNoise.hlsl", "CSmain", "cs_6_4", SHADER_TYPE_COMPUTE), extraBuffer);

		m_rayPipeline->SetVolumeFogTexture(&m_volumeFogTextureBuffer);
	}

	//レイマーチングのパラメーターを設定。
	m_raymarchingParam.m_pos = KazMath::Vec3<float>(-100.0f, 1.0f, 1.0f);
	m_raymarchingParam.m_color = KazMath::Vec3<float>(1.0f, 1.0f, 1.0f);
	m_raymarchingParam.m_wrapCount = 20.0f;
	m_raymarchingParam.m_gridSize = 4.0f;
	m_raymarchingParam.m_wrapCount = 30.0f;
	m_raymarchingParam.m_density = 0.65f;
	m_raymarchingParam.m_sampleLength = 8.0f;
	m_raymarchingParam.m_isSimpleFog = 0;
	m_raymarchingParam.m_isActive = false;
	m_raymarchingParamData = KazBufferHelper::SetConstBufferData(sizeof(RaymarchingParam));
	m_raymarchingParamData.bufferWrapper->TransData(&m_raymarchingParam, sizeof(RaymarchingParam));

	//レイマーチングのパラメーター用定数バッファをセット。
	m_rayPipeline->SetRaymarchingConstData(&m_raymarchingParamData);

	//OnOffデバッグ用のパラメーターを用意。
	m_debugRaytracingParam.m_debugReflection = 0;
	m_debugRaytracingParam.m_debugShadow = 0;
	m_debugRaytracingParam.m_sliderRate = 1280.0f / 2.0f;
	m_debugRaytracingParamData = KazBufferHelper::SetConstBufferData(sizeof(DebugRaytracingParam));
	m_debugRaytracingParamData.bufferWrapper->TransData(&m_debugRaytracingParam, sizeof(DebugRaytracingParam));
	m_isDebugRaytracing = false;
	m_isOldDebugRaytracing = false;
	m_isDebugCamera = false;
	m_isDebugTimeZone = false;
	m_isDebugVolumeFog = false;
	m_isDebugSea = false;

	//ヒットストップのタイマーを初期化。
	StopMgr::Instance()->Init();

	m_debugRaytracingParam.m_skyFacter = 1.0f;

	//OnOffデバッグ用のパラメーターを用意。
	m_rayPipeline->SetDebugOnOffConstData(&m_debugRaytracingParamData);
	m_debugLineScale = 0;
}

SceneManager::~SceneManager()
{
}

void SceneManager::Update()
{
	DescriptorHeapMgr::Instance()->SetDescriptorHeap();

	if (StopMgr::Instance()->IsHitStop()) {
		StopMgr::Instance()->Update();
		m_blasVector.Update();
		return;
	}

	//シェイク量を更新。
	ShakeMgr::Instance()->Update();

	//シーン遷移の開始
	if (m_nextSceneNumber != m_nowSceneNumber)
	{
		m_sceneChange->Start();
	}
	const int RESTART_NUM = -2;

	//ゲーム画面が隠された判定
	if (m_sceneChange->AllHiden())
	{
		//リスタート用の処理
		if (m_nextSceneNumber == RESTART_NUM)
		{
			m_nextSceneNumber = m_nowSceneNumber;
			m_nowScene->Init();
		}
		//シーン切り替え
		else
		{
			m_nowSceneNumber = m_nextSceneNumber;
			//シーンの破棄
			m_nowScene.reset();
			//前シーンの描画命令破棄
			m_rasterize.ReleasePipeline();
			//シーンの生成
			m_nowScene = GetScene(m_nextSceneNumber);

			LoadingBar::Instance()->Init();
			//コンストラクタで用意された描画パイプラインの生成
			std::thread th_a(&SceneManager::GeneratePipeline, this);
			std::thread th_b(&SceneManager::AssetLoad, this);
			std::thread th_c(&SceneManager::LoadScene, this);

			th_a.join();
			th_b.join();
			th_c.join();

			m_nowScene->Init();
		}
	}

	//更新処理
	m_nowScene->Input();
	m_nowScene->Update(m_rasterize);
	//シーン切り替えのトリガー
	int sceneNum = m_nowScene->SceneChange();
	if (sceneNum != SCENE_NONE)
	{
		m_nextSceneNumber = sceneNum;
	}
	m_sceneChange->Update();
	//Scene内での描画情報で生成された場合の生成
	if (m_nowScene->OrderGeneratePipeline())
	{
		m_rasterize.GeneratePipeline();
	}

	m_blasVector.Update();

	//fpsを制限(今回は60fps)
	FpsManager::RegulateFps(60);


	//ボリュームノイズを書き込む。
	m_noiseParam.m_timer += 0.001f;
	m_noiseParamData.bufferWrapper->TransData(&m_noiseParam, sizeof(NoiseParam));
	//m_volumeNoiseShader.Compute({ static_cast<UINT>(256 / 8), static_cast<UINT>(256 / 8), static_cast<UINT>(256 / 4) });

	//ノイズ用のタイマーを加算。
	GBufferMgr::Instance()->m_cameraEyePosData.m_noiseTimer += 0.02f;



	//デバッグ用の値の更新処理
	m_debugRaytracingParamData.bufferWrapper->TransData(&m_debugRaytracingParam, sizeof(DebugRaytracingParam));

	GBufferMgr::Instance()->m_lightConstData.m_dirLight.m_dir.Normalize();

	m_noiseParamData.bufferWrapper->TransData(&m_noiseParam, sizeof(NoiseParam));
	m_raymarchingParamData.bufferWrapper->TransData(&m_raymarchingParam, sizeof(RaymarchingParam));

	//データを転送。一旦ここで。
	GBufferMgr::Instance()->m_lightBuffer.bufferWrapper->TransData(&GBufferMgr::Instance()->m_lightConstData, sizeof(GBufferMgr::LightConstData));
}

void SceneManager::Draw()
{
	m_sceneChange->Draw(m_rasterize);

	m_nowScene->Draw(m_rasterize, m_blasVector);
	//ラスタライザ描画
	m_rasterize.SortAndRender();

	//Tlasを構築 or 再構築する。
	//m_tlas.Build(m_blasVector);
	//レイトレ用のデータを構築。
	//m_rayPipeline->BuildShaderTable(m_blasVector);
	if (m_blasVector.GetBlasRefCount() != 0)
	{
	//	m_rayPipeline->TraceRay(m_tlas);
	}
	//UI用の描画
	m_rasterize.UISortAndRender();

	MemoryProfiler::Instance()->CalucurateBufferMemory();
	MemoryProfiler::Instance()->DrawImGui();
}

void SceneManager::AssetLoad()
{
	//タスクの最大値
	LoadingBar::Instance()->SetMaxBar(50);
	for (int i = 0; i < 50; ++i)
	{
		LoadingBar::Instance()->IncreNowNum(1);
	}
}

void SceneManager::GeneratePipeline()
{
	m_rasterize.GeneratePipeline();
}

void SceneManager::LoadScene()
{
	int num = 0;
	while (LoadingBar::Instance()->GetNowRate() < 1.0f)
	{
		m_loadScene.Update();
		m_loadScene.Draw();
		++num;
	}
	bool debug = false;
}
