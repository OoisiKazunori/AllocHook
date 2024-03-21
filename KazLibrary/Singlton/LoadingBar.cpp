#include "LoadingBar.h"

LoadingBar::LoadingBar()
{
	Init();
}

void LoadingBar::Init()
{
	m_loadMaxBar = 0;
	m_loadNowBar = 0;
}

void LoadingBar::SetMaxBar(int arg_num)
{
	std::lock_guard<std::mutex> lock(m_mtx);
	m_loadMaxBar += arg_num;
}

void LoadingBar::IncreNowNum(int arg_num)
{
	std::lock_guard<std::mutex> lock(m_mtx);
	m_loadNowBar += arg_num;
}

float LoadingBar::GetNowRate()
{
	return static_cast<float>(m_loadNowBar) / static_cast<float>(m_loadMaxBar);
}

bool LoadingBar::IsDone()
{
	return 1.0f <= GetNowRate();
}
