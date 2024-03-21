#pragma once
#include"../DirectXCommon/Base.h"
#include"../Scene/SceneBase.h"
#include"../Scene/SceneChange.h"
#include"../RenderTarget/RenderTargetStatus.h"
#include<memory>
#include"../KazLibrary/Render/DrawingByRasterize.h"
#include "../KazLibrary/Sound/SoundManager.h"
#include "../KazLibrary/Raytracing/RayPipeline.h"
#include"../KazLibrary/Scene/LoadScene.h"
#include"../Game/Scene/RenderScene.h"
#include"../Game/Debug/MemoryProfiler.h"

class SceneManager
{
public:
	SceneManager();
	~SceneManager();
	void Update();
	void Draw();

	bool endGameFlag;
private:
	std::shared_ptr<SceneBase>m_nowScene;
	std::unique_ptr<ChangeScene::SceneChange> m_sceneChange;
	int m_nowSceneNumber, m_nextSceneNumber;
	bool initGameFlag;

	bool sceneChangeFlag;

	bool itisInArrayFlag;
	bool gameFirstInitFlag;



	//ボリュームフォグ用3Dテクスチャ
	KazBufferHelper::BufferData m_volumeFogTextureBuffer;
	ComputeShader m_volumeNoiseShader;
	struct NoiseParam
	{
		KazMath::Vec3<float> m_worldPos;
		float m_timer;
		float m_windSpeed;
		float m_windStrength;
		float m_threshold;
		float m_skydormScale;
		int m_octaves;
		float m_persistence;
		float m_lacunarity;
		float m_pad;
	}m_noiseParam;
	KazBufferHelper::BufferData m_noiseParamData;

	//ボリュームフォグ用定数バッファ
	struct RaymarchingParam
	{
		KazMath::Vec3<float> m_pos; //ボリュームテクスチャのサイズ
		float m_gridSize; //サンプリングするグリッドのサイズ
		KazMath::Vec3<float> m_color; //フォグの色
		float m_wrapCount; //サンプリング座標がはみ出した際に何回までWrapするか
		float m_sampleLength; //サンプリング距離
		float m_density; //濃度係数
		int m_isSimpleFog;
		int m_isActive;
	}m_raymarchingParam;
	KazBufferHelper::BufferData m_raymarchingParamData;

	//OnOffデバッグ用
	struct DebugRaytracingParam
	{
		int m_debugReflection;
		int m_debugShadow;
		float m_sliderRate;
		float m_skyFacter;
	}m_debugRaytracingParam;
	KazBufferHelper::BufferData m_debugRaytracingParamData;

	//デバッグ用のOnOff切り替えライン
	bool m_isOldDebugRaytracing;
	float m_debugLineScale;

	//デバッグ関連の変数
	bool m_isDebugCamera;
	bool m_isDebugRaytracing;
	bool m_isDebugTimeZone;
	bool m_isDebugVolumeFog;
	bool m_isDebugSea;


	//ラスタライザ描画
	DrawingByRasterize m_rasterize;

	//レイトレにモデルを組み込む用の配列クラス
	Raytracing::BlasVector m_blasVector;
	//レイトレで描画するための情報
	Raytracing::Tlas m_tlas;
	//レイトレ用パイプライン
	std::vector<Raytracing::RayPipelineShaderData> m_pipelineShaders;
	std::unique_ptr<Raytracing::RayPipeline> m_rayPipeline;

	std::shared_ptr<SceneBase>GetScene(int arg_sceneNum)
	{
		switch (arg_sceneNum)
		{
		case 0:
			return std::make_shared<RenderScene>(m_rasterize);
			break;
		case 1:
			break;
		case 2:
			break;
		default:
			break;
		}
		return std::make_shared<RenderScene>(m_rasterize);
	}

	void AssetLoad();
	void GeneratePipeline();

	//ロード画面の描画
	LoadScene m_loadScene;
	void LoadScene();
};