#include "DrawFuncHelper.h"

DrawFuncHelper::TextureRender::TextureRender(DrawingByRasterize& arg_rasterize, const std::string& arg_textureFilePass, bool arg_isUIFlag)
{
	if (arg_isUIFlag)
	{
		m_drawCommand.renderTargetHandle = -1;
	}
	m_drawCommand = DrawFuncData::SetSpriteAlphaData(DrawFuncData::GetSpriteAlphaShader());
	m_drawCommandData = arg_rasterize.SetPipeline(m_drawCommand);
	m_textureBuffer = TextureResourceMgr::Instance()->LoadGraphBuffer(arg_textureFilePass);
	Error();
	m_textureSize =
	{
		static_cast<float>(m_textureBuffer.bufferWrapper->GetBuffer().Get()->GetDesc().Width),
		static_cast<float>(m_textureBuffer.bufferWrapper->GetBuffer().Get()->GetDesc().Height)
	};
}

DrawFuncHelper::TextureRender::TextureRender(DrawingByRasterize& arg_rasterize, const std::string& arg_textureFilePass, const DrawFuncData::DrawCallData& arg_drawCall, bool arg_isUIFlag)
{
	if (arg_isUIFlag)
	{
		m_drawCommand.renderTargetHandle = -1;
	}
	m_drawCommand = arg_drawCall;
	m_drawCommandData = arg_rasterize.SetPipeline(m_drawCommand);
	m_textureBuffer = TextureResourceMgr::Instance()->LoadGraphBuffer(arg_textureFilePass);
	Error();
	m_textureSize =
	{
		static_cast<float>(m_textureBuffer.bufferWrapper->GetBuffer().Get()->GetDesc().Width),
		static_cast<float>(m_textureBuffer.bufferWrapper->GetBuffer().Get()->GetDesc().Height)
	};
}

DrawFuncHelper::TextureRender::TextureRender(DrawingByRasterize& arg_rasterize, const DrawFuncData::DrawCallData& arg_drawCall, bool arg_isUIFlag)
{
	if (arg_isUIFlag)
	{
		m_drawCommand.renderTargetHandle = -1;
	}
	m_drawCommand = arg_drawCall;
	m_drawCommandData = arg_rasterize.SetPipeline(m_drawCommand);
}

DrawFuncHelper::TextureRender::TextureRender(DrawingByRasterize& arg_rasterize, bool arg_isUIFlag)
{
	if (arg_isUIFlag)
	{
		m_drawCommand.renderTargetHandle = -1;
	}
	m_drawCommand = DrawFuncData::SetSpriteAlphaData(DrawFuncData::GetSpriteShader());
	m_drawCommandData = arg_rasterize.SetPipeline(m_drawCommand);
}

DrawFuncHelper::TextureRender::TextureRender()
{
}

void DrawFuncHelper::TextureRender::Load(DrawingByRasterize& arg_rasterize, const std::string& arg_textureFilePass, bool arg_isUIFlag)
{
	if (arg_isUIFlag)
	{
		m_drawCommand.renderTargetHandle = -1;
	}
	m_drawCommand = DrawFuncData::SetSpriteAlphaData(DrawFuncData::GetSpriteAlphaShader());
	m_drawCommandData = arg_rasterize.SetPipeline(m_drawCommand);
	m_textureBuffer = TextureResourceMgr::Instance()->LoadGraphBuffer(arg_textureFilePass);
	Error();
	m_textureSize =
	{
		static_cast<float>(m_textureBuffer.bufferWrapper->GetBuffer().Get()->GetDesc().Width),
		static_cast<float>(m_textureBuffer.bufferWrapper->GetBuffer().Get()->GetDesc().Height)
	};
}

void DrawFuncHelper::TextureRender::Load(DrawingByRasterize& arg_rasterize, const KazBufferHelper::BufferData& arg_textureBuffer, bool arg_isUIFlag)
{
	if (arg_isUIFlag)
	{
		m_drawCommand.renderTargetHandle = -1;
	}
	m_drawCommand = DrawFuncData::SetSpriteAlphaData(DrawFuncData::GetSpriteAlphaShader());
	m_drawCommandData = arg_rasterize.SetPipeline(m_drawCommand);
	m_textureBuffer = arg_textureBuffer;
	Error();
	m_textureSize =
	{
		static_cast<float>(m_textureBuffer.bufferWrapper->GetBuffer().Get()->GetDesc().Width),
		static_cast<float>(m_textureBuffer.bufferWrapper->GetBuffer().Get()->GetDesc().Height)
	};
}

void DrawFuncHelper::TextureRender::Load(DrawingByRasterize& arg_rasterize, const DrawFuncData::DrawCallData& arg_drawCall, bool arg_isUIFlag)
{
	if (arg_isUIFlag)
	{
		m_drawCommand.renderTargetHandle = -1;
	}
	m_drawCommand = arg_drawCall;
	m_drawCommandData = arg_rasterize.SetPipeline(m_drawCommand);
}

void DrawFuncHelper::TextureRender::operator=(const KazBufferHelper::BufferData& rhs)
{
	m_textureBuffer = rhs;
	m_textureSize =
	{
		static_cast<float>(m_textureBuffer.bufferWrapper->GetBuffer().Get()->GetDesc().Width),
		static_cast<float>(m_textureBuffer.bufferWrapper->GetBuffer().Get()->GetDesc().Height)
	};
}

void DrawFuncHelper::TextureRender::Draw2D(DrawingByRasterize& arg_rasterize, const KazMath::Transform2D& arg_trasform2D, const KazMath::Color& arg_addColor)
{
	Error();

	KazMath::Transform2D transform(arg_trasform2D);
	//テクスチャのサイズに割合をかける
	transform.scale *= m_textureSize;
	DrawFunc::DrawTextureIn2D(m_drawCommand, transform, m_textureBuffer, arg_addColor);
	arg_rasterize.UIRender(m_drawCommandData);
}

void DrawFuncHelper::TextureRender::Draw2D(DrawingByRasterize& arg_rasterize, const KazMath::Transform2D& arg_trasform2D, const KazBufferHelper::BufferData& arg_textureBuffer, const KazMath::Color& arg_addColor)
{
	KazMath::Transform2D transform(arg_trasform2D);
	m_textureSize.x = static_cast<float>(arg_textureBuffer.bufferWrapper->GetBuffer()->GetDesc().Width);
	m_textureSize.y = static_cast<float>(arg_textureBuffer.bufferWrapper->GetBuffer()->GetDesc().Height);

	//テクスチャのサイズに割合をかける
	transform.scale *= m_textureSize;
	DrawFunc::DrawTextureIn2D(m_drawCommand, transform, arg_textureBuffer, arg_addColor);
	arg_rasterize.UIRender(m_drawCommandData);
}

void DrawFuncHelper::TextureRender::Draw3D(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec, const KazMath::Transform3D& arg_trasform3D, const KazMath::Color& arg_addColor)
{
	Error();

	KazMath::Transform3D transform(arg_trasform3D);
	//テクスチャのサイズに割合をかける
	transform.scale *= {m_textureSize.x, -m_textureSize.y, 1.0f};
	DrawFunc::DrawTextureIn3D(m_drawCommand, transform, m_textureBuffer, arg_addColor);
	arg_rasterize.ObjectRender(m_drawCommandData);
	StackOnBlas(arg_blasVec, transform.GetMat());
}

void DrawFuncHelper::TextureRender::Draw3D(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec, const KazMath::Transform3D& arg_trasform3D, const KazBufferHelper::BufferData& arg_textureBuffer, const KazMath::Color& arg_addColor)
{
	KazMath::Transform3D transform(arg_trasform3D);
	m_textureSize.x = static_cast<float>(arg_textureBuffer.bufferWrapper->GetBuffer()->GetDesc().Width);
	m_textureSize.y = static_cast<float>(arg_textureBuffer.bufferWrapper->GetBuffer()->GetDesc().Height);

	//テクスチャのサイズに割合をかける
	transform.scale.x *= m_textureSize.x;
	transform.scale.y *= m_textureSize.y;
	DrawFunc::DrawTextureIn3D(m_drawCommand, transform, arg_textureBuffer, arg_addColor);
	arg_rasterize.ObjectRender(m_drawCommandData);
	StackOnBlas(arg_blasVec, transform.GetMat());
}

void DrawFuncHelper::TextureRender::Draw3DBillBoard(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec, const KazMath::Transform3D& arg_trasform3D, const KazMath::Color& arg_addColor)
{
	Error();

	KazMath::Transform3D transform(arg_trasform3D);
	//テクスチャのサイズに割合をかける
	transform.scale *= {m_textureSize.x, -m_textureSize.y, 1.0f};
	DrawFunc::DrawTextureIn3DBillBoard(m_drawCommand, transform, m_textureBuffer, arg_addColor);
	arg_rasterize.ObjectRender(m_drawCommandData);
	StackOnBlas(arg_blasVec, transform.GetMat());
}

void DrawFuncHelper::TextureRender::Draw3DBillBoard(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec, const KazMath::Transform3D& arg_trasform3D, const KazBufferHelper::BufferData& arg_textureBuffer, const KazMath::Color& arg_addColor)
{
	KazMath::Transform3D transform(arg_trasform3D);
	m_textureSize.x = static_cast<float>(arg_textureBuffer.bufferWrapper->GetBuffer()->GetDesc().Width);
	m_textureSize.y = static_cast<float>(arg_textureBuffer.bufferWrapper->GetBuffer()->GetDesc().Height);

	//テクスチャのサイズに割合をかける
	transform.scale.x *= m_textureSize.x;
	transform.scale.y *= m_textureSize.y;
	DrawFunc::DrawTextureIn3DBillBoard(m_drawCommand, transform, arg_textureBuffer, arg_addColor);
	arg_rasterize.ObjectRender(m_drawCommandData);
	StackOnBlas(arg_blasVec, transform.GetMat());
}

void DrawFuncHelper::TextureRender::DrawGaussian(DrawingByRasterize& arg_rasterize, const KazMath::Transform2D& arg_trasform2D)
{
	Error();

	KazMath::Transform2D transform(arg_trasform2D);
	//テクスチャのサイズに割合をかける
	transform.scale *= m_textureSize;
	//行列情報
	DirectX::XMMATRIX mat = transform.GetMat() * CameraMgr::Instance()->GetOrthographicMatProjection();
	m_drawCommand.extraBufferArray[0].bufferWrapper->TransData(&mat, sizeof(DirectX::XMMATRIX));

	m_drawCommand.extraBufferArray[1].rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
	m_drawCommand.extraBufferArray[1].rootParamType = GRAPHICS_PRAMTYPE_DATA2;
	//テクスチャ情報
	m_drawCommand.extraBufferArray[2] = m_textureBuffer;
	m_drawCommand.extraBufferArray[2].rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
	m_drawCommand.extraBufferArray[2].rootParamType = GRAPHICS_PRAMTYPE_DATA;
	//描画命令
	//arg_rasterize.ObjectRender(m_drawCommand);
}

void DrawFuncHelper::TextureRender::Error()
{
	if (!m_textureBuffer.bufferWrapper)
	{
		//テクスチャ読み込み失敗
		assert(0);
	}
	else
	{
		m_textureSize.x = static_cast<float>(m_textureBuffer.bufferWrapper->GetBuffer()->GetDesc().Width);
		m_textureSize.y = static_cast<float>(m_textureBuffer.bufferWrapper->GetBuffer()->GetDesc().Height);
	}
}

void DrawFuncHelper::TextureRender::StackOnBlas(Raytracing::BlasVector& arg_blasVec, const DirectX::XMMATRIX& arg_worldMat)
{
	for (auto& obj : m_drawCommand.m_raytracingData.m_blas)
	{
		arg_blasVec.Add(obj, arg_worldMat);
	}
}

DrawFuncHelper::ModelRender::ModelRender(const std::string& arg_fileDir, const std::string& arg_filePass, bool arg_deletePipelineInScene)
{
	Load(arg_fileDir, arg_filePass);
	m_drawCommand.SetupRaytracing(true);
}

DrawFuncHelper::ModelRender::ModelRender(const std::shared_ptr<ModelInfomation>& arg_modelInfomation, const DrawFuncData::DrawCallData& arg_drawCall, bool arg_deletePipelineInScene)
{
	m_modelInfo = arg_modelInfomation;
	m_drawCommand = arg_drawCall;
	LoadAnimation();
	m_drawCommand.SetupRaytracing(true);
}

DrawFuncHelper::ModelRender::ModelRender()
{
	m_drawCommand.SetupRaytracing(true);
}

void DrawFuncHelper::ModelRender::Load(const std::string& arg_fileDir, const std::string& arg_filePass)
{
	m_modelInfo = ModelLoader::Instance()->Load(arg_fileDir, arg_filePass);

	Error();
	if (LoadAnimation())
	{
		m_drawCommand = DrawFuncData::SetDrawGLTFAnimationIndexMaterialData(*m_modelInfo, DrawFuncData::GetAnimationModelShader());
	}
	else
	{
		m_drawCommand = DrawFuncData::SetDrawGLTFAnimationIndexMaterialData(*m_modelInfo, DrawFuncData::GetDefferedModelShader());
	}
}

void DrawFuncHelper::ModelRender::Load(const std::shared_ptr<ModelInfomation>& arg_modelInfomation, const DrawFuncData::DrawCallData& arg_drawCall)
{
	m_modelInfo = arg_modelInfomation;
	m_drawCommand = arg_drawCall;
	Error();
	LoadAnimation();
}

bool DrawFuncHelper::ModelRender::LoadAnimation()
{
	//アニメーションあり
	if (3 < m_modelInfo->skelton->bones.size())
	{
		m_animator = std::make_shared<ModelAnimator>(m_modelInfo);
		return true;
	}
	else
	{
		m_drawCommand.extraBufferArray.back() = InitAnimationData::Instance()->GetBoneInitBuffer(GRAPHICS_PRAMTYPE_DATA4);
		return false;
	}
}

void DrawFuncHelper::ModelRender::Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec, KazMath::Transform3D& arg_trasform3D, const KazMath::Color& arg_addColor, float arg_timeScale)
{
	if (m_animator)
	{
		m_animator->Update(arg_timeScale);
		DrawFunc::DrawModel(m_drawCommand, arg_trasform3D, m_animator->GetBoneMatBuff(), arg_addColor);
	}
	else
	{
		DrawFunc::DrawModel(m_drawCommand, arg_trasform3D, arg_addColor);
	}
	//ラスタライザ描画命令
	arg_rasterize.ObjectRender(m_drawCommandData);
	//レイトレ描画命令
	for (auto& obj : m_drawCommand.m_raytracingData.m_blas)
	{
		arg_blasVec.Add(obj, arg_trasform3D.GetMat());
	}
}

void DrawFuncHelper::ModelRender::Error()
{
	if (!m_modelInfo)
	{
		//モデル読み込み失敗
		assert(0);
	}
}

DrawFuncHelper::LineRender::LineRender(DrawingByRasterize& arg_rasterize, const DrawFuncData::DrawCallData& arg_drawCall)
{
	VertexGenerateData data(&posArray, sizeof(DirectX::XMFLOAT3), 2, sizeof(DirectX::XMFLOAT3));
	m_drawCommand = arg_drawCall;
	//頂点情報を用意する
	m_drawCommand.m_modelVertDataHandle = VertexBufferMgr::Instance()->GenerateBuffer(data, false);
	m_drawCommand.drawInstanceCommandData = VertexBufferMgr::Instance()->GetVertexBuffer(m_drawCommand.m_modelVertDataHandle).instanceData;
	m_drawCommand.drawInstanceCommandData.topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	m_drawCommand.renderTargetHandle = GBufferMgr::Instance()->GetRenderTarget()[0];
	m_drawCommand.pipelineData.desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

	m_drawCommandData = arg_rasterize.SetPipeline(m_drawCommand);
}

DrawFuncHelper::LineRender::LineRender()
{
}

void DrawFuncHelper::LineRender::Generate(DrawingByRasterize& arg_rasterize)
{
	VertexGenerateData data(&posArray, sizeof(DirectX::XMFLOAT3), 2, sizeof(DirectX::XMFLOAT3));
	m_drawCommand = DrawFuncData::SetLine(DrawFuncData::GetBasicShader());
	//頂点情報を用意する
	m_drawCommand.m_modelVertDataHandle = VertexBufferMgr::Instance()->GenerateBuffer(data, false);
	m_drawCommand.drawInstanceCommandData = VertexBufferMgr::Instance()->GetVertexBuffer(m_drawCommand.m_modelVertDataHandle).instanceData;
	m_drawCommand.pipelineData.desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

	m_drawCommandData = arg_rasterize.SetPipeline(m_drawCommand);
}

void DrawFuncHelper::LineRender::Generate(DrawingByRasterize& arg_rasterize, const DrawFuncData::DrawCallData& arg_drawCall)
{
	VertexGenerateData data(&posArray, sizeof(DirectX::XMFLOAT3), 2, sizeof(DirectX::XMFLOAT3));
	m_drawCommand = arg_drawCall;
	//頂点情報を用意する
	m_drawCommand.m_modelVertDataHandle = VertexBufferMgr::Instance()->GenerateBuffer(data, false);
	m_drawCommand.drawInstanceCommandData = VertexBufferMgr::Instance()->GetVertexBuffer(m_drawCommand.m_modelVertDataHandle).instanceData;
	m_drawCommand.pipelineData.desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

	m_drawCommandData = arg_rasterize.SetPipeline(m_drawCommand);
}

void DrawFuncHelper::LineRender::Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec, const KazMath::Vec3<float>& arg_startPos, const KazMath::Vec3<float>& arg_endPos, const KazMath::Color& arg_color)
{
	posArray[0] = arg_startPos;
	posArray[1] = arg_endPos;
	//座標更新
	VertexBufferMgr::Instance()->GetVertexBuffer(m_drawCommand.m_modelVertDataHandle).vertBuffer->bufferWrapper->TransData(&posArray, sizeof(DirectX::XMFLOAT3) * 2);
	DirectX::XMMATRIX mat(DirectX::XMMatrixIdentity() * CameraMgr::Instance()->GetViewMatrix() * CameraMgr::Instance()->GetPerspectiveMatProjection());
	m_drawCommand.extraBufferArray[0].bufferWrapper->TransData(&mat, sizeof(DirectX::XMMATRIX));
	//色
	DirectX::XMFLOAT4 color = arg_color.ConvertColorRateToXMFLOAT4();
	m_drawCommand.extraBufferArray[1].bufferWrapper->TransData(&color, sizeof(DirectX::XMFLOAT4));

	arg_rasterize.ObjectRender(m_drawCommandData);
}
