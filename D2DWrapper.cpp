#include "pch.h"
#include "D2DWrapper.h"

#pragma comment(lib, "D2d1.lib")

D2DWrapper::~D2DWrapper()
{
	Uninit();
	m_pFactory = nullptr;
}

bool D2DWrapper::Init(HWND hWnd)
{
	Uninit();

	if (!m_pFactory) {
		HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &m_pFactory);
		assert(SUCCEEDED(hr));
		if (FAILED(hr))
			return false;
	}

	RECT rc;
	GetClientRect(hWnd, &rc);

	D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
	HRESULT hr = m_pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hWnd, size), m_pRenderTarget.Assign());
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
		return false;

	hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0, 0, 1.0f), m_pSolidBrush.Assign());
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
		return false;

	// »æÍ¼¿¹¾â³Ý
	m_pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	// ÎÄ±¾¿¹¾â³Ý
	m_pRenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);

	return true;
}

void D2DWrapper::Uninit()
{
	m_pRenderTarget = nullptr;
	m_pSolidBrush = nullptr;
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

	// ¿ªÊ¼äÖÈ¾
	m_pRenderTarget->BeginDraw();

	// ±³¾°É«Çå¿Õ
	m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Gray));

	// Ìî³äÍÖÔ²
	m_pRenderTarget->FillEllipse(ellipse, m_pSolidBrush);

	// ½áÊøäÖÈ¾ check device error and call reinitialize
	if (D2DERR_RECREATE_TARGET == m_pRenderTarget->EndDraw())
		Uninit();
}
