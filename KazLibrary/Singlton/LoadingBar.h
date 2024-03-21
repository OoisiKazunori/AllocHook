#pragma once
#include"Helper/ISinglton.h"
#include <cstdint>
#include <iostream>
#include <mutex>
#include <thread>

/// <summary>
/// ロード画面のゲージの数値管理
/// </summary>
class LoadingBar :public ISingleton<LoadingBar>
{
public:
	LoadingBar();
	void Init();
	void SetMaxBar(int arg_num);
	void IncreNowNum(int arg_num);
	float GetNowRate();

	bool IsDone();
private:
	int m_loadMaxBar;
	int m_loadNowBar;
	//排他処理
	std::mutex m_mtx;
};

