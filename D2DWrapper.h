#pragma once
#include <mutex>
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include "ComPtr.hpp"

/*
https://github.com/microsoft/Windows-classic-samples/tree/main/Samples/Win7Samples/multimedia/Direct2D/TextAnimationSample
https://github.com/microsoft/Windows-classic-samples/tree/main/Samples/Win7Samples/multimedia/Direct2D/SimpleDirect2DApplication
https://github.com/microsoft/Windows-classic-samples/tree/main/Samples/Win7Samples/multimedia/Direct2D/Direct2DHelloWorld
https://learn.microsoft.com/en-us/windows/win32/api/dwrite/nn-dwrite-idwritetextformat
https://learn.microsoft.com/zh-cn/windows/win32/api/dwrite/nn-dwrite-idwritetextlayout
*/

static const WCHAR msc_fontName[] = L"Arial";
static const FLOAT msc_fontSize = 50;
static const WCHAR sc_helloWorld[] = L"Hello, World! ´ó¼ÒºÃÂð£¿";

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

	ComPtr<IDWriteFactory> m_pDWriteFactory = nullptr;
	ComPtr<IDWriteTextFormat> m_pTextFormat = nullptr;
	ComPtr<ID2D1SolidColorBrush> m_pTextBrush = nullptr;
};
