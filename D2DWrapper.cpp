#include "pch.h"
#include "D2DWrapper.h"
#include <assert.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

std::wstring wstrText =
	L"teshi demo Meta ��ϯִ�й�������˲����Ա 11,000 �ˣ�ռFacebook��ѷ��ȷ�ϼƻ�������� 10,000 ����˾�ͼ��� wang test zhi dao hhhhhhhahahaha ְλ�����������ޱ��������ơ����ɡ��ҿ⡣���Ƕ��ڲ�Ա��"
	L"���ǲ��������ڲ�Ա�������ǻ�ȡ����һЩ�Ѿ���Ϊ�Ƽ����������ʵĽ�����\n\n"
	L"\tÿ�ҹ�˾�����Լ����ص��������ƶ������ɱ��Ĵ�ʩ��������Ҳ�м������ԭ�����ȣ��Ƽ���˾�Ǵ����в���Ӯ�ҡ��������߱����ڼ���μ� Zoom ���顢Peloton "
	L"���к͹ۿ� Netflix ʱ���Ƽ���˾�Ĺ�Ʊ�ͻ����ǡ����ǻ���˴����ֽ�ע�벢���������š����ܶ࣬��ʱ���ڷ���Խ��Խ��Ĵ�ֱ���򡣵����ž��ö񻯺�ͨ������"
	L"���к͹ۿ� Netflix ʱ���Ƽ���˾�Ĺ�Ʊ�ͻ����ǡ����ǻ���˴����ֽ�ע�벢���������š����ܶ࣬��ʱ���ڷ���Խ��Խ��Ĵ�ֱ���򡣵����ž��ö񻯺�ͨ������"
	L"�����������Ʒ��ɣ�Ͷ��������Ѱ�Ҹ���ȫ�Ķ�ע���Ƽ���˾���ڻع��������ս���������\n\n"
	L"\t�����ɱ�����һ�����ԭ������ Recode �� Peter Kafka ��Today, Explained����˵�������������ĿƼ���˾�����Ѿ����졣���仰˵�������޷�ΪͶ�����ṩ"
	L"�� 2000 ������ں� 2010 �������ʱ����ͬ�Ĵ��ģ�������⽫�Ը���ҵ�Ĳ����߲�������Ӱ�졣\n\n"
	L"�����ǶԻ���ժ¼��Ϊƪ���������Ƚ����˱༭�����������л��и������ݣ����������Today, Explained�������ںδ���ò��ͣ�����Apple Podcasts��Google "
	L"Podcasts��Spotify��Stitcher��";

D2DWrapper::~D2DWrapper()
{
	Uninit();
}

bool D2DWrapper::Init(HWND hWnd)
{
	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	LOGFONT info = {};
	if (GetObject(hFont, sizeof(LOGFONT), &info) != 0) {
		OutputDebugStringA("=============== font : ");
		OutputDebugStringW(info.lfFaceName);
		OutputDebugStringA(" =============== \n ");
	}

	Uninit();

	//---------------------------------------------------- common ----------------------------------------------------------
	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &m_pD2DFactory);
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
		return false;

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&m_pDWriteFactory));
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
		return false;

	//---------------------------------------------------- common for text style ----------------------------------------------------------
	hr = m_pDWriteFactory->CreateTextFormat(info.lfFaceName, NULL,
						DWRITE_FONT_WEIGHT_NORMAL,          // �Ӵ�
						DWRITE_FONT_STYLE_NORMAL,           // �Ƿ�б��
						DWRITE_FONT_STRETCH_EXTRA_EXPANDED, // ˮƽ�����ı����
						FONT_SIZE, L"", &m_pTextFormat);
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
		return false;

	m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);                // �ı�ˮƽ����
	m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);       // �ı���ֱ����
	m_pTextFormat->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM, 20.f, 4.0f); // �����ı��м��

	if (1) {
		// ��Ȳ���ʱ �Զ����� ��DWRITE_WORD_WRAPPING_WHOLE_WORD���ܱ�֤�Ե���Ϊ��λ����
		// ��һ��ö��ֵ��DWRITE_WORD_WRAPPING_WRAP�����ܱ�֤��������λΪ��λ���л���
		m_pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
	} else {
		// ��ʹ��Ȳ��� Ҳ������ ����������򽫱��ü���
		m_pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

		hr = m_pDWriteFactory->CreateEllipsisTrimmingSign(m_pTextFormat, m_pTextCutShow.Assign()); // ʡ�Ժ�
		if (SUCCEEDED(hr)) {
			const DWRITE_TRIMMING sc_trimming = {DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
			m_pTextFormat->SetTrimming(&sc_trimming, m_pTextCutShow); // �������߽�ʱ ��ʾʡ�Ժ�
		}
	}

	//---------------------------------------------------------------------------------
	RECT rc;
	GetClientRect(hWnd, &rc);

	D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
	hr = m_pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hWnd, size), m_pRenderTarget.Assign());
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
		return false;

	hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0, 0, 1.f, 0.5f), m_pBlueAlpha.Assign());
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
	m_pBlueAlpha = nullptr;
	m_pRenderTarget = nullptr;

	m_pTextCutShow = nullptr;
	m_pTextFormat = nullptr;
	m_pDWriteFactory = nullptr;
	m_pD2DFactory = nullptr;
}

static DWORD64 crtTime;
static double fps = 0.f;
std::wstring getText()
{
	// ͳ����Ⱦfps
	static DWORD64 startTime = GetTickCount();
	static DWORD64 renderCount = 0;
	++renderCount;
	crtTime = GetTickCount64();
	if (crtTime - startTime >= 3000) {
		fps = double(renderCount) * 1000 / double(crtTime - startTime);
		renderCount = 1;
		startTime = crtTime;
	}

	WCHAR time[200];
	swprintf_s(time, 200, L"123456789�����ı� render text \n\t %llu(FPS)  %llus : %llums  \n\n", (DWORD64)fps, crtTime / 1000, crtTime % 1000);

	std::wstring str = std::wstring(time) + wstrText;
	return str;
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
	m_pRenderTarget->FillEllipse(ellipse, m_pBlueAlpha);

	// ��Բ
	m_pRenderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(150.0f, 150.0f), 100.0f, 100.0f), m_pRedBrush, 3.0);

	// ���������� ���ܸܺߣ�
	D2D1_POINT_2F pos1;
	D2D1_POINT_2F pos2;
	for (auto i = 0; i < rc.right; i += 160) {
		pos1.y = 0.f;
		pos2.y = (float)rc.bottom;
		pos1.x = pos2.x = float(i);
		m_pRenderTarget->DrawLine(pos1, pos2, m_pRedBrush, 1);
	}
	for (auto i = 0; i < rc.bottom; i += 90) {
		pos1.x = 0.f;
		pos2.x = (float)rc.right;
		pos1.y = pos2.y = float(i);
		m_pRenderTarget->DrawLine(pos1, pos2, m_pRedBrush, 1);
	}

	// ��ֱ�Ǿ���
	D2D1_RECT_F rcDraw;
	rcDraw.left = 350;
	rcDraw.top = 100;
	rcDraw.right = rcDraw.left + 500;
	rcDraw.bottom = rcDraw.top + 350;
	m_pRenderTarget->DrawRectangle(rcDraw, m_pBlueAlpha, 5.f); // ָ������߿���ʱ���߿���ĺ������������ŵ�

	// ���Բ�Ǿ���
	D2D1_ROUNDED_RECT rcFill;
	rcFill.rect.left = 300;
	rcFill.rect.top = 500;
	rcFill.rect.right = rcFill.rect.left + 200;
	rcFill.rect.bottom = rcFill.rect.top + 200;
	rcFill.radiusX = 10;
	rcFill.radiusY = 10;
	m_pRenderTarget->FillRoundedRectangle(rcFill, m_pBlueAlpha);

	// ��Ⱦ�ı�
	std::wstring str = getText();
	if (0) {
		D2D1_SIZE_F size = m_pRenderTarget->GetSize();
		m_pRenderTarget->DrawText(str.c_str(), (UINT32)str.size(), m_pTextFormat, D2D1::RectF(0, 0, size.width, size.height), m_pRedBrush);
	} else {
		// ֻҪIDWriteTextLayout�Ŀ���������˱仯 ����Ҫ���´���IDWriteTextLayout����
		// ������Ⱦ�������ı� ��������Ӧ���
		// ����ע�� �������Ŀ�ߴ��ڴ���ʵ�ʿ��ʱ ��ʱ��û���ı�����Ⱦ���� û����ԭ��
		ComPtr<IDWriteTextLayout> pTextLayout = nullptr;
		HRESULT hr = m_pDWriteFactory->CreateTextLayout(str.c_str(), (UINT32)str.size(), m_pTextFormat,
								float(rc.right - rc.left), // maxWidth
								float(rc.bottom - rc.top), // maxHeight
								&pTextLayout);
		assert(SUCCEEDED(hr));

		DWRITE_TEXT_RANGE range;
		range.startPosition = 0;
		range.length = 12; // ���Է���һ�����ֻ�һ������ ��������һ���ַ�����Ϊ�õ���WCHAR��

		pTextLayout->SetUnderline(true, range);     // �»���
		pTextLayout->SetStrikethrough(true, range); // ɾ����

		m_pRenderTarget->DrawTextLayout(D2D1::Point2F(0, 0), pTextLayout, m_pBlackBrush, D2D1_DRAW_TEXT_OPTIONS_NONE);
	}

	// ������Ⱦ check device error and call reinitialize
	if (D2DERR_RECREATE_TARGET == m_pRenderTarget->EndDraw())
		Uninit();
}
