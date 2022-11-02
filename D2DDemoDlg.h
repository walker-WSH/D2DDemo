
// D2DDemoDlg.h: 头文件
//

#pragma once
#include "D2DWrapper.h"

// CD2DDemoDlg 对话框
class CD2DDemoDlg : public CDialogEx
{
// 构造
public:
	CD2DDemoDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_D2DDEMO_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	D2DWrapper m_D2DRender;

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
