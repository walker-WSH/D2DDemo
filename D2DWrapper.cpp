#include "pch.h"
#include "D2DWrapper.h"
#include <assert.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

std::wstring wstrText = L"wangshaohui Meta ��ϯִ�й�������˲����Ա 11,000 �ˣ�ռFacebook��ѷ��ȷ�ϼƻ�������� 10,000 ����˾�ͼ���ְλ�����������ޱ��������ơ����ɡ��ҿ⡣���Ƕ��ڲ�Ա��"
			L"���ǲ��������ڲ�Ա�������ǻ�ȡ����һЩ�Ѿ���Ϊ�Ƽ����������ʵĽ�����\n\n"
			L"ÿ�ҹ�˾�����Լ����ص��������ƶ������ɱ��Ĵ�ʩ��������Ҳ�м������ԭ�����ȣ��Ƽ���˾�Ǵ����в���Ӯ�ҡ��������߱����ڼ���μ� Zoom ���顢Peloton "
			L"���к͹ۿ� Netflix ʱ���Ƽ���˾�Ĺ�Ʊ�ͻ����ǡ����ǻ���˴����ֽ�ע�벢���������š����ܶ࣬��ʱ���ڷ���Խ��Խ��Ĵ�ֱ���򡣵����ž��ö񻯺�ͨ������"
			L"���к͹ۿ� Netflix ʱ���Ƽ���˾�Ĺ�Ʊ�ͻ����ǡ����ǻ���˴����ֽ�ע�벢���������š����ܶ࣬��ʱ���ڷ���Խ��Խ��Ĵ�ֱ���򡣵����ž��ö񻯺�ͨ������"
			L"�����������Ʒ��ɣ�Ͷ��������Ѱ�Ҹ���ȫ�Ķ�ע���Ƽ���˾���ڻع��������ս���������\n\n"
			L"�����ɱ�����һ�����ԭ������ Recode �� Peter Kafka ��Today, Explained����˵�������������ĿƼ���˾�����Ѿ����졣���仰˵�������޷�ΪͶ�����ṩ"
			L"�� 2000 ������ں� 2010 �������ʱ����ͬ�Ĵ��ģ�������⽫�Ը���ҵ�Ĳ����߲�������Ӱ�졣\n\n"
			L"�����ǶԻ���ժ¼��Ϊƪ���������Ƚ����˱༭�����������л��и������ݣ����������Today, Explained�������ںδ���ò��ͣ�����Apple Podcasts��Google "
			L"Podcasts��Spotify��Stitcher��";

D2DWrapper::~D2DWrapper()
{
	Uninit();
}

bool D2DWrapper::Init(HWND hWnd)
{
	Uninit();

	RECT rc;
	GetClientRect(hWnd, &rc);

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

	// �ı�ˮƽ����
	m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	// �ı���ֱ����
	m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	// �ı����в��� �Ƿ��� ��ô����
	m_pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_WHOLE_WORD);
	//m_pTextFormat->SetTrimming();

	//---------------------------------------------------------------------------------
	D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
	hr = m_pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hWnd, size), m_pRenderTarget.Assign());
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
		return false;

	hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), m_pBlackBrush.Assign());
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
		return false;

	hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0, 0, 0), m_pRedBrush.Assign());
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
		return false;

	// ��ͼ�����
	m_pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	// �ı������
	m_pRenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);

	return true;
}

void D2DWrapper::Uninit()
{
	m_pRedBrush = nullptr;
	m_pBlackBrush = nullptr;
	m_pRenderTarget = nullptr;
	m_pFactory = nullptr;

	m_pTextFormat = nullptr;
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

	// ��ʼ��Ⱦ
	m_pRenderTarget->BeginDraw();

	// ����ɫ���
	m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

	// �����Բ
	m_pRenderTarget->FillEllipse(ellipse, m_pBlackBrush);

	// ��Բ
	m_pRenderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(150.0f, 150.0f), 100.0f, 100.0f), m_pRedBrush, 3.0);

	// ͳ����Ⱦfps
	static DWORD startTime = GetTickCount();
	static DWORD renderCount = 0;
	static float fps = 0.f;
	++renderCount;
	DWORD crtTime = GetTickCount();
	if (crtTime - startTime >= 3000) {
		fps = float(renderCount) * 1000 / float(crtTime - startTime);
		renderCount = 1;
		startTime = crtTime;
	}

	WCHAR time[200];
	swprintf_s(time, 200, L"render text \n\t %.2ffps  %us : %ums  \n\n", fps, crtTime / 1000, crtTime % 1000);
	std::wstring str = std::wstring(time) + wstrText;
	// ��Ⱦ�ı�
	if (0) {
		D2D1_SIZE_F size = m_pRenderTarget->GetSize();
		m_pRenderTarget->DrawText(str.c_str(), str.size(), m_pTextFormat, D2D1::RectF(0, 0, size.width, size.height), m_pRedBrush);
	} else {
		// ֻҪIDWriteTextLayout�Ŀ���������˱仯 ����Ҫ���´���IDWriteTextLayout����
		// ������Ⱦ�������ı� ��������Ӧ���
		// ����ע�� �������Ŀ�ߴ��ڴ���ʵ�ʿ��ʱ ��ʱ��û���ı�����Ⱦ���� û����ԭ��
		ComPtr<IDWriteTextLayout> pTextLayout = nullptr;
		HRESULT hr = m_pDWriteFactory->CreateTextLayout(str.c_str(), str.size(), m_pTextFormat,
								float(rc.right - rc.left), // maxWidth
								float(rc.bottom - rc.top), // maxHeight
								&pTextLayout);
		assert(SUCCEEDED(hr));

		DWRITE_TEXT_RANGE range;
		range.startPosition = 0;
		range.length = 40;

		pTextLayout->SetUnderline(true, range);
		pTextLayout->SetStrikethrough(true, range);

		m_pRenderTarget->DrawTextLayout(D2D1::Point2F(0, 0), pTextLayout, m_pBlackBrush, D2D1_DRAW_TEXT_OPTIONS_NONE);
	}

	// ������Ⱦ check device error and call reinitialize
	if (D2DERR_RECREATE_TARGET == m_pRenderTarget->EndDraw())
		Uninit();
}
