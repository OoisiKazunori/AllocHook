#include"../Scene/SceneChange.h"
#include"../Scene/SceneChange.h"
#include"../Helper/ResourceFilePass.h"
#include"../Loader/TextureResourceMgr.h"
#include "../Easing/easing.h"
#include "../Imgui/MyImgui.h"

using namespace ChangeScene;

SceneChange::SceneChange(DrawingByRasterize& arg_rasterize) :allHidenFlag(false), startFlag(false)
{
	m_render = DrawFuncData::SetSpriteAlphaData(DrawFuncData::GetSpriteAlphaDepthAlwaysShader());
	texBuffer = TextureResourceMgr::Instance()->LoadGraphBuffer("Resource/SceneUI/SceneChange/SceneChange.png");
	DrawFunc::DrawTextureIn2D(m_render, m_transform, texBuffer, KazMath::Color(255, 255, 255, 255));
	m_renderCallData = arg_rasterize.GenerateSceneChangePipeline(&m_render);

	m_transform.pos.y = WIN_Y / 2.0f;
	m_transform.scale = { 1280.0f, 720.0f };
}

void SceneChange::Init()
{
	startFlag = false;
}

void SceneChange::Finalize()
{
	startFlag = false;
}

void SceneChange::Update()
{
	if (startFlag)
	{
		//“ü‚è
		if (startOutInT[0] < 1.0f)
		{
			Rate(&startOutInT[0], 0.03f, 1.0f);
			m_transform.pos.x = WIN_X + (WIN_X / 2) + EasingMaker(Out, Cubic, startOutInT[0]) * -WIN_X;
			tmp = m_transform.pos.x;
		}
		//I‚í‚è
		else
		{
			//‰B‚ê‚½ƒtƒ‰ƒO
			if (!initFlag)
			{
				allHidenFlag = true;
				initFlag = true;
			}

			Rate(&startOutInT[1], 0.03f, 1.0f);
			m_transform.pos.x = tmp + EasingMaker(In, Cubic, startOutInT[1]) * static_cast<float>(-WIN_X);
		}

		if (1.0 <= startOutInT[1])
		{
			startFlag = false;
		}
	}
	else
	{
		initFlag = false;
		startOutInT[0] = 0;
		startOutInT[1] = 0;
		m_transform.pos.x = WIN_X + (WIN_X / 2);
	}
}

void SceneChange::Draw(DrawingByRasterize& arg_rasterize)
{
	DrawFunc::DrawTextureIn2D(m_render, m_transform, texBuffer, KazMath::Color(255, 255, 255, 255));
	arg_rasterize.UIRender(m_renderCallData);
}

void SceneChange::Start()
{
	startFlag = true;
}

bool SceneChange::AllHiden()
{
	if (allHidenFlag)
	{
		allHidenFlag = false;
		return true;
	}
	return false;
}
