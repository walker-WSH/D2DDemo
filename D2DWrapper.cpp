#include "pch.h"
#include "D2DWrapper.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

D2DWrapper::~D2DWrapper()
{
	Uninit();
}

bool D2DWrapper::Init(HWND hWnd)
{
	Uninit();

	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &m_pFactory);
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
		return false;

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&m_pDWriteFactory));
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
		return false;

	hr = m_pDWriteFactory->CreateTextFormat(msc_fontName, NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, msc_fontSize,
						L"", //locale
						&m_pTextFormat);
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
		return false;

	// 文本水平对齐
	m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	// 文本垂直对齐
	m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
	// 文本换行策略 是否换行 怎么换行
	m_pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	//m_pTextFormat->SetTrimming();

	RECT rc;
	GetClientRect(hWnd, &rc);

	D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
	hr = m_pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hWnd, size), m_pRenderTarget.Assign());
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
		return false;

	hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0, 0, 1.0f), m_pSolidBrush.Assign());
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
		return false;

	hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), m_pTextBrush.Assign());
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
		return false;

	// 绘图抗锯齿
	m_pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	// 文本抗锯齿
	m_pRenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);

	return true;
}

void D2DWrapper::Uninit()
{
	m_pRenderTarget = nullptr;
	m_pSolidBrush = nullptr;
	m_pFactory = nullptr;

	m_pTextFormat = nullptr;
	m_pTextBrush = nullptr;
	m_pDWriteFactory = nullptr;
}

void D2DWrapper::Render(HWND hWnd)
{
	RECT rc;
	GetClientRect(hWnd, &rc);

	if (m_pRenderTarget) {
		D2D1_SIZE_F size = m_pRenderTarget->GetSize();
		if (rc.right != (LONG)size.width || rc.bottom != (LONG)size.height) {
			D2D1_SIZE_U size;
			size.width = rc.right;
			size.height = rc.bottom;

			if (FAILED(m_pRenderTarget->Resize(size)))
				Uninit();
		}
	}

	if (!m_pRenderTarget) {
		if (!Init(hWnd))
			return;
	}

	float x = (float)rc.right / 2;
	float y = (float)rc.bottom / 2;
	float radius = 200;
	D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius);

	// 开始渲染
	m_pRenderTarget->BeginDraw();

	// 背景色清空
	m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

	// 填充椭圆
	m_pRenderTarget->FillEllipse(ellipse, m_pSolidBrush);

	// 渲染文本
	D2D1_SIZE_F size = m_pRenderTarget->GetSize();
	m_pRenderTarget->DrawText(sc_helloWorld, ARRAYSIZE(sc_helloWorld) - 1, m_pTextFormat, D2D1::RectF(0, 0, size.width, size.height), m_pTextBrush);

	// 结束渲染 check device error and call reinitialize
	if (D2DERR_RECREATE_TARGET == m_pRenderTarget->EndDraw())
		Uninit();
}
