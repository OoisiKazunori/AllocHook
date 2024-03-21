#pragma once
#include<vector>
#include <iostream>
#include <source_location>
#include"../KazLibrary/Math/KazMath.h"
#include"../Pipeline/Shader.h"
#include"../KazLibrary/Buffer/BufferDuplicateBlocking.h"
#include"../KazLibrary/Buffer/DrawFuncData.h"

/// <summary>
/// スタックされた情報を元にラスタライズで描画する
/// </summary>
class DrawingByRasterize
{
public:
	DrawingByRasterize();

	//描画情報の生成命令を積む
	const DrawFuncData::DrawData* SetPipeline(DrawFuncData::DrawCallData& arg_drawData, bool arg_deleteInSceneFlag = false);
	/// <summary>
	/// スタックされた描画情報の生成(マルチスレッド対応の為に一括で生成する処理)
	/// </summary>
	void GeneratePipeline();
	/// <summary>
	/// 前シーンの描画命令破棄
	/// </summary>
	void ReleasePipeline();
	/// <summary>
	/// シーン内で描画命令破棄
	/// </summary>
	void ReleasePipelineInScene();

	void ObjectRender(const DrawFuncData::DrawData* arg_drawData);
	void UIRender(const DrawFuncData::DrawData* arg_drawData);

	void SortAndRender();
	void UISortAndRender();


	const DrawFuncData::DrawData* GenerateSceneChangePipeline(DrawFuncData::DrawCallData*arg_drawCall);
private:

	//事前生成向け-----
	//描画生成命令のキュー
	std::list<DrawFuncData::DrawCallData*>m_drawCallStackDataArray;
	//描画命令のデータ
	std::vector<std::unique_ptr<DrawFuncData::DrawData>>m_drawCallArray;
	//描画命令のキュー
	std::list<const DrawFuncData::DrawData*>m_stackDataArray;
	std::list<const DrawFuncData::DrawData*>m_uiStackDataArray;
	//削除された描画情報のハンドル
	std::vector<int>m_deleteHandleArray;
	//削除されたハンドルから描画パイプライン生成ハンドル
	std::vector<int>m_generateFromHandleArray;

	std::unique_ptr<DrawFuncData::DrawData> m_sceneChange;


	//パイプラインの情報----------------------------------------

	PipelineDuplicateBlocking m_piplineBufferMgr;
	ShaderDuplicateBlocking m_shaderBufferMgr;
	RootSignatureDuplicateBlocking m_rootSignatureBufferMgr;

	//パイプラインの情報----------------------------------------

	//レンダーターゲット情報----------------------------------------
	RESOURCE_HANDLE prevRenderTargetHandle;
	struct RenderTargetClearData
	{
		RESOURCE_HANDLE handle;
		bool isOpenFlag;
		bool clearFlag;
	};
	std::vector<RenderTargetClearData>renderTargetClearArray;
	//レンダーターゲット情報----------------------------------------

	//描画に必要なバッファをコマンドリストに積む
	void SetBufferOnCmdList(const  std::vector<KazBufferHelper::BufferData>& BUFFER_ARRAY, std::vector<RootSignatureParameter> ROOT_PARAM);

	//頂点情報のセット
	void MultiMeshedDrawIndexInstanceCommand(const KazRenderHelper::MultipleMeshesDrawIndexInstanceCommandData& DATA, const std::vector<std::vector<KazBufferHelper::BufferData>>& MATERIAL_BUFFER, std::vector<RootSignatureParameter> ROOT_PARAM);
	void DrawIndexInstanceCommand(const KazRenderHelper::DrawIndexInstanceCommandData& DATA);
	void DrawInstanceCommand(const KazRenderHelper::DrawInstanceCommandData& DATA);

	void DrawExecuteIndirect(const KazRenderHelper::MultipleMeshesDrawIndexInstanceCommandData& DATA, const Microsoft::WRL::ComPtr<ID3D12CommandSignature>& arg_commandSignature, const DrawFuncData::ExcuteIndirectArgumentData& arg_argmentData);

	//何処の描画関数から呼び出されたかエラー文を書く
	void ErrorCheck(RESOURCE_HANDLE HANDLE, const std::source_location& DRAW_SOURCE_LOCATION)
	{
		if (HANDLE == -1)
		{
			FailCheck(ErrorMail(DRAW_SOURCE_LOCATION));
			assert(0);
		}
	}

	std::string ErrorMail(const std::source_location& DRAW_SOURCE_LOCATION);

	void DrawCall(const std::list<const DrawFuncData::DrawData*> arg_drawCall);
};