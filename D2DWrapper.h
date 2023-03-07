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

���壬��С
��ɫ
��ֱˮƽ����
�Ӵ�
б��
�Զ�����/�����нض�����
�»���(֧��ֻ���ָ���ַ�)
ɾ����(֧��ֻ���ָ���ַ�)
��ת
3D������
����ɫ https://www.pianshen.com/article/76621683515/
������(�������)https://codeantenna.com/a/bsKMXrctOX
*/

static const WCHAR FONT_NAME[] = L"Arial";
static const FLOAT FONT_SIZE = 15;

class D2DWrapper {
public:
	~D2DWrapper();

	bool Init(HWND hWnd);
	void Uninit();

	void Render(HWND hWnd);

private:
	// common object
	ComPtr<ID2D1Factory> m_pFactory = nullptr;
	ComPtr<IDWriteFactory> m_pDWriteFactory = nullptr;
	ComPtr<IDWriteTextFormat> m_pTextFormat = nullptr;
	ComPtr<IDWriteInlineObject> m_pTextCutShow = NULL;

	ComPtr<ID2D1HwndRenderTarget> m_pRenderTarget = nullptr;
	ComPtr<ID2D1SolidColorBrush> m_pRedBrush = nullptr;
	ComPtr<ID2D1SolidColorBrush> m_pBlackBrush = nullptr;
	ComPtr<ID2D1SolidColorBrush> m_pBlueAlpha = nullptr;
};
