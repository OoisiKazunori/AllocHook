#pragma once
#include<vector>
#include <iostream>
#include <source_location>
#include"../KazLibrary/Math/KazMath.h"
#include"../Pipeline/Shader.h"
#include"../KazLibrary/Buffer/BufferDuplicateBlocking.h"
#include"../KazLibrary/Buffer/DrawFuncData.h"

/// <summary>
/// 描画の生成情報
/// </summary>
class GenerateRender
{
public:
	//描画情報の生成命令を積む
	const DrawFuncData::DrawData *SetPipeline(DrawFuncData::DrawCallData &arg_drawData, bool arg_deleteInSceneFlag = false);
	/// <summary>
	/// スタックされた描画情報の生成(マルチスレッド対応の為に一括で生成する処理)
	/// </summary>
	void GeneratePipeline();
	/// <summary>
	/// 前シーンの描画命令破棄
	/// </summary>
	void ReleasePipeline();

	void ObjectRender(const DrawFuncData::DrawData *arg_drawData);
	void UIRender(const DrawFuncData::DrawData *arg_drawData);

private:
	//事前生成向け-----
	//描画生成命令のキュー
	std::list<DrawFuncData::DrawCallData *>m_drawCallStackDataArray;
	//描画命令のデータ
	std::vector<std::unique_ptr<DrawFuncData::DrawData>>m_drawCallArray;
	//描画命令のキュー
	std::list<const DrawFuncData::DrawData *>m_stackDataArray;
	std::list<const DrawFuncData::DrawData *>m_uiStackDataArray;


	//パイプラインの情報----------------------------------------

	PipelineDuplicateBlocking piplineBufferMgr;
	ShaderDuplicateBlocking shaderBufferMgr;
	RootSignatureDuplicateBlocking rootSignatureBufferMgr;

	//パイプラインの情報----------------------------------------
};
