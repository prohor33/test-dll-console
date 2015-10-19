
// Project3Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "Project3.h"
#include "Project3Dlg.h"
#include "afxdialogex.h"

#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CProject3Dlg dialog



CProject3Dlg::CProject3Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CProject3Dlg::IDD, pParent),
	m_hConsoleProcess(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProject3Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DLL_LOADS_N, m_eDllLoadsN);
	DDX_Control(pDX, IDC_EDIT_OBJ_N, m_eObjectsN);
	DDX_Control(pDX, IDC_EDIT_CONSOLE_RESULT, m_eConsoleResult);
	DDX_Control(pDX, IDC_EDIT_CURRENT_DLL_N, m_eCurrentDll);
}

BEGIN_MESSAGE_MAP(CProject3Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START_CONSOLE, &CProject3Dlg::OnBnClickedButtonStartConsole)
	ON_BN_CLICKED(IDC_BUTTON_STOP_CONSOLE, &CProject3Dlg::OnBnClickedButtonStopConsole)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CProject3Dlg::OnBnClickedButtonClose)
END_MESSAGE_MAP()


// CProject3Dlg message handlers

BOOL CProject3Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_eDllLoadsN.SetWindowTextW(std::to_wstring(1000).c_str());
	m_eObjectsN.SetWindowTextW(std::to_wstring(1000).c_str());

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CProject3Dlg::OnPaint()
{
	if (IsIconic())	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	} else {
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CProject3Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CProject3Dlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_COPYDATA:
		{
			COPYDATASTRUCT* msg = (COPYDATASTRUCT*)lParam;
			if (msg->lpData)
				m_eCurrentDll.SetWindowTextW(std::to_wstring(*((int*)msg->lpData)).c_str());
			break;
		}
	case GUIThreadData::WM_GUI_THREAD_HAVE_DATA:
		{
			GUIThreadData* data = (GUIThreadData*)lParam;
			if (data)
				m_eConsoleResult.SetWindowTextW(std::to_wstring(data->console_exit_code).c_str());
			delete data;
			break;
		}
	}
	return CDialogEx::WindowProc(message, wParam, lParam);
 }

UINT ConsoleThreadProc(LPVOID pParam);

void CProject3Dlg::OnBnClickedButtonStartConsole()
{
	CString text;
	m_eDllLoadsN.GetWindowTextW(text);
	if (text.IsEmpty())
		return;
	int dll_loads_count = std::stoi(text.GetString());
	m_eObjectsN.GetWindowTextW(text);
	if (text.IsEmpty())
		return;
	int obj_count = std::stoi(text.GetString());

	std::wstring process_to_start(L"Project2.exe");
	std::wstring args = std::to_wstring(dll_loads_count) + L" " + std::to_wstring(obj_count) + L" " + std::to_wstring((int)m_hWnd);

	SHELLEXECUTEINFO shellExInfo;
	shellExInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	shellExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	shellExInfo.hwnd = NULL;
	shellExInfo.lpVerb = L"open";
	shellExInfo.lpFile = process_to_start.c_str();
	shellExInfo.lpParameters = args.c_str();
	shellExInfo.lpDirectory = L".\\";
	shellExInfo.nShow = SW_SHOW;
	shellExInfo.hInstApp = NULL;

	// start process
	if (!ShellExecuteEx(&shellExInfo)) {
		MessageBox(_T("Can't start console"), _T("Error"), NULL);
		return;
	}

	// retrieve PID
	if (!GetProcessId(shellExInfo.hProcess)) {
		MessageBox(_T("Error while GetProcessId()"), _T("Error"), NULL);
		return;
	}
	m_hConsoleProcess = shellExInfo.hProcess;

	ConsoleThreadProcData* data = new ConsoleThreadProcData();
	data->m_hConsoleProcess = m_hConsoleProcess;
	data->m_hWnd = m_hWnd;
	AfxBeginThread(ConsoleThreadProc, data);
}


void CProject3Dlg::OnBnClickedButtonStopConsole()
{
	if (m_hConsoleProcess) {
		TerminateProcess(m_hConsoleProcess, 0);
		m_hConsoleProcess = 0;
	}
}


void CProject3Dlg::OnBnClickedButtonClose()
{
	exit(0);
}

UINT ConsoleThreadProc(LPVOID pParam)
{
	ConsoleThreadProcData* data = (ConsoleThreadProcData*) pParam;
	if (!data) {
		assert(data);
		return 1;
	}

	while (true) {
		// Wait 10 ms or until child process exits
		DWORD res = WaitForSingleObject(data->m_hConsoleProcess, (DWORD)(10));
		if (res == WAIT_FAILED) {
			return 1;
		} else if (res == WAIT_TIMEOUT) {
			continue;
		} else if (res == WAIT_OBJECT_0) {
			DWORD exit_code;
			GetExitCodeProcess(data->m_hConsoleProcess, &exit_code);

			GUIThreadData* console_data = new GUIThreadData();
			console_data->console_exit_code = exit_code;
			PostMessage(data->m_hWnd, GUIThreadData::WM_GUI_THREAD_HAVE_DATA, 0, reinterpret_cast<LPARAM>(console_data));

			return 0;
		}
	}

	return 1;
}