#include "LoadScene.h"
#include"../KazLibrary/Singlton/LoadingBar.h"

LoadScene::LoadScene() :m_bar(m_rasterize, "Resource/")
{
	//描画情報生成
	//バー

	//描画情報生成
	m_rasterize.GeneratePipeline();
}

void LoadScene::Update()
{
	/*LoadingBar::Instance()->GetNowRate();
	m_blasVector.Update();*/
}

void LoadScene::Draw()
{
	//ロード用の画面描画
	//m_bar.m_tex.Draw2D(m_rasterize, KazMath::Transform2D());
	//ゲージ描画

	//ラスタライザ描画
	m_rasterize.SortAndRender();
	m_rasterize.UISortAndRender();

	//レンダーターゲットの閉じ込め
	//DirectXのGPU命令
}
