#pragma once
#include"../DirectXCommon/Base.h"
#include"../Math/KazMath.h"
#include"../Helper/DirtyFlag.h"
#include"../KazLibrary/Render/DrawingByRasterize.h"
#include"../KazLibrary/Render/BasicDraw.h"

namespace ChangeScene {

	class SceneChange
	{
	public:
		SceneChange(DrawingByRasterize& arg_rasterize);

		void Init();
		void Finalize();
		void Update();
		void Draw(DrawingByRasterize& arg_rasterize);

		/// <summary>
		/// ƒV[ƒ“‘JˆÚ‚ğŠJn‚µ‚Ü‚·
		/// </summary>
		void Start();

		/// <summary>
		/// ‰æ–Ê‚ª‰B‚ê‚½uŠÔ‚Ì”»’è‚ğæ‚è‚Ü‚·
		/// •K‚¸ƒgƒŠƒK[”»’è‚Åæ‚é–
		/// </summary>
		/// <returns>true...‰B‚ê‚½,false...‰B‚ê‚Ä‚È‚¢</returns>
		bool AllHiden();

	private:
		bool startFlag, finishFlag;
		bool allHidenFlag;
		bool initFlag;

		float tmp;
		float startOutInT[2];

		KazMath::Transform2D m_transform;
		DrawFuncData::DrawCallData m_render;
		const DrawFuncData::DrawData *m_renderCallData;
		KazBufferHelper::BufferData texBuffer;
	};
}