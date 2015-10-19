
// Project3Dlg.h : header file
//

#pragma once
#include "afxwin.h"

class GUIThreadData {
public:
	GUIThreadData() :
		console_exit_code(-2) {};

	static const int WM_GUI_THREAD_HAVE_DATA = 0x1111;

	DWORD console_exit_code;
};

struct ConsoleThreadProcData {
	ConsoleThreadProcData() :
		m_hConsoleProcess(0),
		m_hWnd(0) {};

	HANDLE m_hConsoleProcess;
	HWND m_hWnd;
};

// CProject3Dlg dialog
class CProject3Dlg : public CDialogEx
{
// Construction
public:
	CProject3Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PROJECT3_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;

public:
	CEdit m_eDllLoadsN;
	CEdit m_eObjectsN;
	CEdit m_eConsoleResult;
	CEdit m_eCurrentDll;
	afx_msg void OnBnClickedButtonStartConsole();
	afx_msg void OnBnClickedButtonStopConsole();
	afx_msg void OnBnClickedButtonClose();

private:
	HANDLE m_hConsoleProcess;
};
