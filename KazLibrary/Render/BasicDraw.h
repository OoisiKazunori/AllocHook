#pragma once
#include"Render/DrawFunc.h"
#include"../KazLibrary/Render/DrawFuncHelper.h"
#include"Render/DrawingByRasterize.h"
#include"Loader/ModelLoader.h"
#include"Buffer/GBufferMgr.h"

namespace BasicDraw
{
	DrawFuncData::DrawCallData SetModel(const std::shared_ptr<ModelInfomation>& arg_modelInfomation);
	DrawFuncData::DrawCallData SetTex();

	struct BasicModelRender
	{
		BasicModelRender(DrawingByRasterize& arg_rasterize, const std::string& arg_fileDir, const std::string& arg_fileName, bool arg_deleteInSceneFlag = false);
		BasicModelRender(DrawingByRasterize& arg_rasterize,bool arg_deleteInSceneFlag = false);
		BasicModelRender();
		void Load(DrawingByRasterize& arg_rasterize, const std::string& arg_fileDir, const std::string& arg_fileName, bool arg_deleteInSceneFlag);
		DrawFuncHelper::ModelRender m_model;
	};

	struct BasicModelInstanceRender
	{
		BasicModelInstanceRender(DrawingByRasterize& arg_rasterize, const std::string& arg_fileDir, const std::string& arg_fileName, bool arg_deleteInSceneFlag = false);
		BasicModelInstanceRender(DrawingByRasterize& arg_rasterize, bool arg_deleteInSceneFlag = false);
		BasicModelInstanceRender();
		void Load(DrawingByRasterize& arg_rasterize, const std::string& arg_fileDir, const std::string& arg_fileName, bool arg_deleteInSceneFlag);

		void Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec);
		DrawFuncHelper::ModelRender m_model;

		KazBufferHelper::RAMToVRAMBufferData m_transformBuffer;
	};

	struct BasicTextureRender
	{
		BasicTextureRender(DrawingByRasterize& arg_rasterize, const std::string& arg_filePass, bool arg_isUIFlag = false);
		BasicTextureRender(DrawingByRasterize& arg_rasterize, bool arg_isUIFlag = false);
		BasicTextureRender();

		DrawFuncHelper::TextureRender m_tex;
	};

	struct BasicLineRender
	{
		BasicLineRender(DrawingByRasterize& arg_rasterize);
		DrawFuncHelper::LineRender m_render;
	};

};