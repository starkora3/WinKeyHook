#pragma once

#include "resource.h"
#include "keyhook.h"
#include <shellapi.h>
#include <vector>

//! �f�t�H���g�X���[�v����
#define DEFAULT_INTERVAL_SLEEP	50
#define BUFFLEN 8191

namespace MYAPP
{
#ifdef _DEBUG
	static LPCTSTR STR_APPNAME = _T("WinKeyHook.DEBUG");
#else
	static LPCTSTR STR_APPNAME = _T("WinKeyHook");
#endif
};


class CMainWnd : public IKeyHookListener
{
public:
	CMainWnd(HWND hWnd) :m_hWnd(hWnd), 
							m_bClose(FALSE), 
							m_hEdit(NULL), 
							m_dwModKeyState(0){}
	//! 
	BOOL Initialize();

	//! �^�X�N�g���C���b�Z�[�W
	static const ULONG m_uWMNotifyIcon;
	//! �^�X�N�g���C����
	static const ULONG m_uWMTaskTrayRestore;

	//! �^�X�N�g���C���b�Z�[�W
	LRESULT OnNotifyTaskBarIcon(WPARAM wParam, LPARAM lParam);

	//! �^�X�N�g���C�ǉ�
	BOOL AddNotifyIcon();
	//! �^�X�N�g���C�폜
	BOOL DeleteNotifyIcon();

	//! ���鐧��
	BOOL IsClose()const{ return m_bClose; }
	void SetClose(BOOL b = TRUE){ m_bClose = b; }
	void Close()const{ if (m_bClose){ ::PostMessage(GetSafeHwnd(), WM_CLOSE, 0, 0); } }

	void Show()const
	{
		ShowWindow(GetSafeHwnd(), SW_SHOW);
		ShowWindow(GetSafeHwnd(), SW_RESTORE);
	};
	void Hide()const
	{
		ShowWindow(GetSafeHwnd(), SW_HIDE);
	}
	void Iconic()const
	{
		ShowWindow(GetSafeHwnd(), SW_MINIMIZE);
	}

	//! ���O�ǉ�
	void AddLog(LPCTSTR str);

	//! ���b�Z�[�W�n���h��
	BOOL OnMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult);

	//! 
	HWND GetSafeHwnd()const{ return m_hWnd; };
protected:
	// ==================================================================
	// �L�[����̋@�\����
	//! ALT + F4 ���s
	void DoALTF4();

	//! grep ���s
	void DoGrep();

	//!  �G�� �N���b�v�{�[�h����
	void DoClipBoardHistory();

	//! �L�[�_�E���^�X�N�̎��s
	BOOL DoKeyDownTask(WORD* pKeyArray, int nSize);

	//! �L�[�A�b�v�^�X�N�̎��s
	BOOL DoKeyUpTask(WORD* pKeyArray, int nSize);

	//! �L�[�^�X�N�̎��s
	BOOL DoKeyTask(WORD* pKeyArray, int nSize, DWORD dwFlags=0);

	//! �_�E���A�b�v
	BOOL DoKeyDownUp(WORD wKey);

	//! �A�b�v
	BOOL DoKeyUp(WORD wKey);

	//! �\��t��
	void DoPaste();

	//! �R�s�[
	void DoCopy();

	//! �t�@�C�������w�肵�Ď��s
	void DoExec();

	//! Win + �����L�[
	void DoWinNumber(int nNumber);

	//! �}�E�X����
	void DoLButtonDownUp();
	void DoRButtonDownUp();
	void DoMButtonDownUp();
	void DoMouseMove(int x, int y);
	// ==================================================================


	//! �L�[�{�[�h�Ԋu�X���[�v
	static void IntervalSleep(int nSleep=DEFAULT_INTERVAL_SLEEP){::Sleep(nSleep);}

	//! ���O�G�f�B�b�g�̍쐬
	BOOL CreateLogEdit();

	//! �C���L�[�̃L�����Z��
	BOOL CancelModKey();

	BOOL IsWIN()const{ return (m_dwModKeyState & MODKEY_WIN) ? TRUE : FALSE; }
	BOOL IsCtrl()const{ return (m_dwModKeyState & MODKEY_CTRL) ? TRUE : FALSE; }
	BOOL IsShift()const{ return (m_dwModKeyState & MODKEY_SHIFT) ? TRUE : FALSE; }
	BOOL IsMenu()const{ return (m_dwModKeyState & MODKEY_MENU) ? TRUE : FALSE; }
	BOOL IsNoModKey()const {return (!IsCtrl() && !IsShift() && !IsMenu() && !IsWIN())?TRUE:FALSE; }

	HWND m_hWnd;
	HWND m_hEdit;
	//! NOTIFYICONDATA �̐錾
	NOTIFYICONDATA  m_cTaskTray;

	BOOL m_bClose;

	//! �C���L�[��ԊǗ�
	DWORD m_dwModKeyState;
	enum
	{
		MODKEY_LWIN		= 0x0001,
		MODKEY_RWIN		= 0x0002,
		MODKEY_LCTRL	= 0x0004,
		MODKEY_RCTRL	= 0x0008,
		MODKEY_LSHIFT	= 0x0010,
		MODKEY_RSHIFT	= 0x0020,
		MODKEY_LMENU	= 0x0040,
		MODKEY_RMENU	= 0x0080,

		MODKEY_WIN = MODKEY_LWIN | MODKEY_RWIN,
		MODKEY_CTRL = MODKEY_LCTRL | MODKEY_RCTRL,
		MODKEY_SHIFT = MODKEY_LSHIFT | MODKEY_RSHIFT,
		MODKEY_MENU = MODKEY_LMENU | MODKEY_RMENU,
	};


	BOOL m_bKeyDisp;
	enum
	{
		IDX_MENU_LOG=0,
	};
	//! ���j���[��ԍX�V
	void UpdateMenuState();

public:
	//! �L�[�{�[�h�C�x���g�n���h��
	virtual BOOL OnKeyboardEvent(DWORD dwMsg, KBDLLHOOKSTRUCT* pInfo);

};

//! �ʎ���
class CMainWndEx : public CMainWnd
{
public:
	CMainWndEx(HWND h) :CMainWnd(h){}
	//! �L�[�{�[�h�C�x���g�n���h��
	BOOL OnKeyboardEvent(DWORD dwMsg, KBDLLHOOKSTRUCT* pInfo);
};

