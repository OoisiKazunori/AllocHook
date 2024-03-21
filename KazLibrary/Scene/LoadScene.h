#pragma once
#include"../KazLibrary/Render/DrawingByRasterize.h"
#include "../KazLibrary/Raytracing/RayPipeline.h"
#include "../KazLibrary/Render/BasicDraw.h"

/// <summary>
/// マルチスレッド向けのロード画面の描画
/// </summary>
class LoadScene
{
public:
	LoadScene();
	void Update();
	void Draw();

private:

	//ラスタライザ描画
	DrawingByRasterize m_rasterize;

	BasicDraw::BasicTextureRender m_bar;
};

