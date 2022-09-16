#pragma once
#include <mutex>
#include <d2d1.h>
#include <d2d1_1.h>
#include "ComPtr.hpp"

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
