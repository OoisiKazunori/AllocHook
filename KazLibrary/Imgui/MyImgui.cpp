#include "../Imgui/MyImgui.h"

#include"../DirectXCommon/DirectX12Device.h"
#include"../DirectXCommon/DirectX12CmdList.h"

#include"Buffer/DescriptorHeapMgr.h"

MyImgui::MyImgui() 
{
}

MyImgui::~MyImgui()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	ImPlot::DestroyContext();
}

void MyImgui::Create(HWND hwnd) 
{
	//ヒープの生成
	heapForImgui = DescriptorHeapMgr::Instance()->heaps;

	if (ImGui::CreateContext() == nullptr)
	{
		assert(0);
	}

	//初期化1
	bool blnResult = ImGui_ImplWin32_Init(hwnd);
	if (!blnResult)
	{
		assert(0);
	}

	//初期化2
	blnResult = ImGui_ImplDX12_Init(
		DirectX12Device::Instance()->dev.Get(),
		3,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		heapForImgui.Get(),
		heapForImgui.Get()->GetCPUDescriptorHandleForHeapStart(),
		heapForImgui.Get()->GetGPUDescriptorHandleForHeapStart()
	);
	ImPlot::CreateContext();
}

void MyImgui::NewFlame() 
{
	auto &io = ImGui::GetIO();
	// Enable docking(available in imgui `docking` branch at the moment)
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

}


void MyImgui::Set() 
{
	ImGui::Render();

	DirectX12CmdList::Instance()->cmdList->SetDescriptorHeaps(
	1,
	heapForImgui.GetAddressOf());
	ImGui_ImplDX12_RenderDrawData(
	ImGui::GetDrawData(),
	DirectX12CmdList::Instance()->cmdList.Get());
}