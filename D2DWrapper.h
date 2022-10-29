#pragma once
#include <mutex>
#include <d2d1.h>
#include <d2d1_1.h>
#include "ComPtr.hpp"

/*
抗锯齿
字体选择 字体大小 
颜色
垂直居中 水平居中
加粗
斜体
下划线
删除线
外轮廓
*/

class D2DWrapper {
public:
	~D2DWrapper();

	bool Init(HWND hWnd);
	void Uninit();

	void Render(HWND hWnd);

private:
	ComPtr<ID2D1Factory> m_pFactory = nullptr;
	ComPtr<ID2D1HwndRenderTarget> m_pRenderTarget = nullptr;
	ComPtr<ID2D1SolidColorBrush> m_pSolidBrush = nullptr;
};
