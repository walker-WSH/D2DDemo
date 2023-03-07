#include "pch.h"
#include "D2DWrapper.h"
#include <assert.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

std::wstring wstrText =
	L"teshi demo Meta 首席执行官马克扎克伯格裁员 11,000 人，占Facebook的逊已确认计划削减多达 10,000 个公司和技术 wang test zhi dao hhhhhhhahahaha 职位。来福车。罗宾汉。条纹。网飞。币库。他们都在裁员。"
	L"他们不仅仅是在裁员——他们还取消了一些已经成为科技工作代名词的津贴。\n\n"
	L"\t每家公司都有自己独特的问题来推动削减成本的措施。但收缩也有几个宏观原因。首先，科技公司是大流行病的赢家。当消费者被困在家里参加 Zoom 会议、Peloton "
	L"骑行和观看 Netflix 时，科技公司的股票就会上涨。他们获得了大量现金注入并用它来扩张——很多，有时是在风险越来越大的垂直领域。但随着经济恶化和通胀上升"
	L"骑行和观看 Netflix 时，科技公司的股票就会上涨。他们获得了大量现金注入并用它来扩张——很多，有时是在风险越来越大的垂直领域。但随着经济恶化和通胀上升"
	L"而大流行限制放松，投资者正在寻找更安全的赌注，科技公司正在回归地球。因此勒紧裤腰带。\n\n"
	L"\t削减成本的另一个宏观原因，正如 Recode 的 Peter Kafka 在Today, Explained中所说的那样，是最大的科技公司现在已经成熟。换句话说，它们无法为投资者提供"
	L"与 2000 年代后期和 2010 年代繁荣时期相同的大规模增长。这将对该行业的参与者产生各种影响。\n\n"
	L"以下是对话的摘录，为篇幅和清晰度进行了编辑。完整播客中还有更多内容，因此请收听Today, Explained无论您在何处获得播客，包括Apple Podcasts、Google "
	L"Podcasts、Spotify和Stitcher。";

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
						DWRITE_FONT_WEIGHT_NORMAL,          // 加粗
						DWRITE_FONT_STYLE_NORMAL,           // 是否斜体
						DWRITE_FONT_STRETCH_EXTRA_EXPANDED, // 水平方向文本间距
						FONT_SIZE, L"", &m_pTextFormat);
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
		return false;

	m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);                // 文本水平对齐
	m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);       // 文本垂直对齐
	m_pTextFormat->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM, 20.f, 4.0f); // 设置文本行间距

	if (1) {
		// 宽度不够时 自动换行 （DWRITE_WORD_WRAPPING_WHOLE_WORD）能保证以单词为单位换行
		// 另一个枚举值（DWRITE_WORD_WRAPPING_WRAP）不能保证是整个单位为单位进行换行
		m_pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
	} else {
		// 即使宽度不够 也不换行 （多出的区域将被裁剪）
		m_pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

		hr = m_pDWriteFactory->CreateEllipsisTrimmingSign(m_pTextFormat, m_pTextCutShow.Assign()); // 省略号
		if (SUCCEEDED(hr)) {
			const DWRITE_TRIMMING sc_trimming = {DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
			m_pTextFormat->SetTrimming(&sc_trimming, m_pTextCutShow); // 当超出边界时 显示省略号
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

	// 绘图抗锯齿
	m_pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	// 文本抗锯齿
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
	// 统计渲染fps
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
	swprintf_s(time, 200, L"123456789测试文本 render text \n\t %llu(FPS)  %llus : %llums  \n\n", (DWORD64)fps, crtTime / 1000, crtTime % 1000);

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

	// 开始渲染
	m_pRenderTarget->BeginDraw();

	// 背景色清空
	m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

	// 填充椭圆
	m_pRenderTarget->FillEllipse(ellipse, m_pBlueAlpha);

	// 画圆
	m_pRenderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(150.0f, 150.0f), 100.0f, 100.0f), m_pRedBrush, 3.0);

	// 画网格线条 性能很高！
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

	// 画直角矩形
	D2D1_RECT_F rcDraw;
	rcDraw.left = 350;
	rcDraw.top = 100;
	rcDraw.right = rcDraw.left + 500;
	rcDraw.bottom = rcDraw.top + 350;
	m_pRenderTarget->DrawRectangle(rcDraw, m_pBlueAlpha, 5.f); // 指定矩阵边框厚度时，线框画面的厚度是向外测扩张的

	// 填充圆角矩形
	D2D1_ROUNDED_RECT rcFill;
	rcFill.rect.left = 300;
	rcFill.rect.top = 500;
	rcFill.rect.right = rcFill.rect.left + 200;
	rcFill.rect.bottom = rcFill.rect.top + 200;
	rcFill.radiusX = 10;
	rcFill.radiusY = 10;
	m_pRenderTarget->FillRoundedRectangle(rcFill, m_pBlueAlpha);

	// 渲染文本
	std::wstring str = getText();
	if (0) {
		D2D1_SIZE_F size = m_pRenderTarget->GetSize();
		m_pRenderTarget->DrawText(str.c_str(), (UINT32)str.size(), m_pTextFormat, D2D1::RectF(0, 0, size.width, size.height), m_pRedBrush);
	} else {
		// 只要IDWriteTextLayout的宽高区域发生了变化 就需要重新创建IDWriteTextLayout对象
		// 否则渲染出来的文本 不能自适应宽高
		// 另外注意 当创建的宽高大于窗口实际宽高时 有时候没有文本被渲染出来 没搞清原因
		ComPtr<IDWriteTextLayout> pTextLayout = nullptr;
		HRESULT hr = m_pDWriteFactory->CreateTextLayout(str.c_str(), (UINT32)str.size(), m_pTextFormat,
								float(rc.right - rc.left), // maxWidth
								float(rc.bottom - rc.top), // maxHeight
								&pTextLayout);
		assert(SUCCEEDED(hr));

		DWRITE_TEXT_RANGE range;
		range.startPosition = 0;
		range.length = 12; // 测试发现一个数字或一个汉字 都是算作一个字符（因为用的是WCHAR）

		pTextLayout->SetUnderline(true, range);     // 下划线
		pTextLayout->SetStrikethrough(true, range); // 删除线

		m_pRenderTarget->DrawTextLayout(D2D1::Point2F(0, 0), pTextLayout, m_pBlackBrush, D2D1_DRAW_TEXT_OPTIONS_NONE);
	}

	// 结束渲染 check device error and call reinitialize
	if (D2DERR_RECREATE_TARGET == m_pRenderTarget->EndDraw())
		Uninit();
}
