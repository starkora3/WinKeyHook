#include "stdafx.h"
#include "MainWnd.h"
#include "KeyHook.h"

using namespace MYAPP;
using namespace KEYHOOK_DLL;

extern HINSTANCE hInst;
extern std::auto_ptr<CMainWnd> g_pMain;

const ULONG CMainWnd::m_uWMNotifyIcon = ::RegisterWindowMessage(STR_APPNAME);
const ULONG CMainWnd::m_uWMTaskTrayRestore = ::RegisterWindowMessage(_T("__TASKTRAY_RESTORE__"));


//! �L�[�t�b�N���X�i�[
class CKeyHookListener :public IKeyHookListener
{
public:
	virtual BOOL OnKeyboardEvent(DWORD dwMsg, KBDLLHOOKSTRUCT* pInfo)
	{
		BOOL bRet = FALSE;
		switch (dwMsg)
		{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			// �L�[�A�b�v�ƃ_�E���̂ݏ���
			if (g_pMain.get())
			{
				bRet = g_pMain->OnKeyboardEvent(dwMsg, pInfo);
			}
			break;
		}

		return bRet;
	};
	virtual void AddLog(LPCTSTR str)
	{
		if (g_pMain.get())
		{
			g_pMain->AddLog(str);
		}
	};
};
CKeyHookListener g_cListener;


/**
* ������
*/
BOOL CMainWnd::Initialize()
{
	try
	{
		CreateLogEdit();
		// �^�X�N�o�[�A�C�R���̒ǉ�
		AddNotifyIcon();

		// �L�[�t�b�N���X�i�[
		SetKeyHookListener(&g_cListener);

#ifdef _DEBUG
		m_bKeyDisp=TRUE;
#else
		m_bKeyDisp=FALSE;
#endif
		UpdateMenuState();


	}
	catch (...)
	{
		SetClose();
		return FALSE;
	}
	return TRUE;
};

/**
 * ���j���[��ԍX�V
 */
void CMainWnd::UpdateMenuState()
{
	HMENU hMenu = ::GetMenu(GetSafeHwnd());
	if(hMenu)
	{
		HMENU hSub = ::GetSubMenu(hMenu,IDX_MENU_LOG);
		if(hSub)
		{
			::CheckMenuItem(hSub,IDM_KEYDISP,MF_BYCOMMAND|(m_bKeyDisp?MF_CHECKED:MF_UNCHECKED));
		}
	}
}


/**
* �^�X�N�g���C�A�C�R���̒ǉ�
*/
BOOL CMainWnd::AddNotifyIcon()
{
	HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SMALL));
	m_cTaskTray.cbSize = sizeof(NOTIFYICONDATA);
	m_cTaskTray.hWnd = GetSafeHwnd();
	m_cTaskTray.uID = 1;
	m_cTaskTray.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_cTaskTray.hIcon = hIcon;
	m_cTaskTray.uCallbackMessage = m_uWMNotifyIcon;
	lstrcpy(m_cTaskTray.szTip, STR_APPNAME);
	return Shell_NotifyIcon(NIM_ADD, &m_cTaskTray);
}

/**
* �^�X�N�g���C�A�C�R���̍폜
*/
BOOL CMainWnd::DeleteNotifyIcon()
{
	return Shell_NotifyIcon(NIM_DELETE, &m_cTaskTray);
}
/**
* �^�X�N�g���C���b�Z�[�W�n���h��
*/
LRESULT CMainWnd::OnNotifyTaskBarIcon(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case WM_RBUTTONUP:
		HMENU hMenu, hSubMenu;
		POINT pt;

		hMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDC_TASKTRAYMENU));
		hSubMenu = GetSubMenu(hMenu, 0);
		GetCursorPos(&pt);
		SetForegroundWindow(GetSafeHwnd());
		TrackPopupMenu(
			hSubMenu, TPM_BOTTOMALIGN, pt.x, pt.y, 0, GetSafeHwnd(), NULL);
		DestroyMenu(hMenu);

		// �Ō�Ɏ��̃N���b�N�ɔ����ł���悤�Ƀ��b�Z�[�W���|�X�g
		PostMessage(GetSafeHwnd(), WM_NULL, 0, 0);
		break;
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
		Show();
		break;
	}
	return NULL;
}


/**
* ���O�\���G�f�B�b�g�쐬
*/
BOOL CMainWnd::CreateLogEdit()
{
	if (m_hEdit)
	{
		return TRUE;
	}
	HINSTANCE hInst = (HINSTANCE)GetWindowLong(GetSafeHwnd(), GWL_HINSTANCE);

	RECT rc;
	::GetClientRect(GetSafeHwnd(), &rc);


	m_hEdit = ::CreateWindow(_T("EDIT"),	//�G�W�b�g�R���g���[��
		_T(""),					//�ŏ��ɕ\������镶����
		WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL | ES_WANTRETURN,		//�E�B���h�E�X�^�C��
		rc.left, rc.top,			//�\���ʒu
		rc.right - rc.left,
		rc.bottom - rc.top,			//���A����
		GetSafeHwnd(),				//�e�E�B���h�E�̃n���h��
		(HMENU)2001,
		hInst,
		NULL);

	return m_hEdit ? TRUE : FALSE;
}

/**
* ���O�ǉ�
*/
void CMainWnd::AddLog(LPCTSTR str)
{
	if (CreateLogEdit() == FALSE)
	{
		return;
	}

	TCHAR tcBuff[BUFFLEN + 1];
	TCHAR tcBuff2[BUFFLEN + 1];
	::ZeroMemory(tcBuff, (BUFFLEN + 1)*sizeof(TCHAR));
	::ZeroMemory(tcBuff2, (BUFFLEN + 1)*sizeof(TCHAR));

	// ���݂̕\�������擾
	::GetWindowText(m_hEdit, tcBuff, BUFFLEN);

	time_t timer;
	struct tm newtime;

	/* ���ݎ����̎擾 */
	time(&timer);

	/* ���ݎ������\���̂ɕϊ� */
	localtime_s(&newtime, &timer);

	_stprintf_s(tcBuff2, _T("%04d�N%02d��%02d�� %02d��%02d��%02d�b %s\r\n")
		, newtime.tm_year + 1900
		, newtime.tm_mon + 1
		, newtime.tm_mday
		, newtime.tm_hour
		, newtime.tm_min
		, newtime.tm_sec
		, str);

	std::wstring strTemp = tcBuff2;
	strTemp += tcBuff;

	::SetWindowText(m_hEdit, strTemp.c_str());
}

/**
* �E�B���h�E���b�Z�[�W����
* @return TRUE �f�t�H���g���������Ȃ�
*/
BOOL CMainWnd::OnMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	int wmId, wmEvent;
	if (message >= 0xC000)
	{// ���[�U��`���b�Z�[�W
		if (message == m_uWMNotifyIcon)
		{
			lResult = OnNotifyTaskBarIcon(wParam, lParam);
			return TRUE;
		}
		else if(message == m_uWMTaskTrayRestore)
		{// �^�X�N�g���C����
			DeleteNotifyIcon();
			AddNotifyIcon();

			Show();

			AddLog(_T("�^�X�N�g���C�ɍēo�^���܂����B"));
		}
	}
	switch (message)
	{
	case WM_CLOSE:
		if (IsClose() == FALSE)
		{
			Iconic();
			Hide();
			// �����
			return TRUE;
		}
		// �L�[�t�b�N���X�i�[�폜
		SetKeyHookListener(NULL);
		DeleteNotifyIcon();        // �^�X�N�o�[�A�C�R���̍폜
		return FALSE;
	case WM_COMMAND:

		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �I�����ꂽ���j���[�̉��:
		switch (wmId)
		{
		case IDM_EXIT:
			SetClose();
			Close();
			return TRUE;
		case IDM_KEYDISP:
			m_bKeyDisp = !m_bKeyDisp;
			UpdateMenuState();
			break;
		case IDM_SHOW:
			Show();
			return TRUE;
		default:
			break;
		}
		break;
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
		{
			Hide();
		}
		if (m_hEdit)
		{
			RECT rc;
			::GetClientRect(GetSafeHwnd(), &rc);

			::SetWindowPos(m_hEdit, NULL, 0, 0, rc.right, rc.bottom, SWP_NOZORDER);
		}
		break;
	}
	return FALSE;
}
