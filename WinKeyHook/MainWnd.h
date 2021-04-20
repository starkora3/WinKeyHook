#pragma once

#include "resource.h"
#include "keyhook.h"
#include <shellapi.h>
#include <vector>

//! デフォルトスリープ時間
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

	//! タスクトレイメッセージ
	static const ULONG m_uWMNotifyIcon;
	//! タスクトレイ復元
	static const ULONG m_uWMTaskTrayRestore;

	//! タスクトレイメッセージ
	LRESULT OnNotifyTaskBarIcon(WPARAM wParam, LPARAM lParam);

	//! タスクトレイ追加
	BOOL AddNotifyIcon();
	//! タスクトレイ削除
	BOOL DeleteNotifyIcon();

	//! 閉じる制御
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

	//! ログ追加
	void AddLog(LPCTSTR str);

	//! メッセージハンドラ
	BOOL OnMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult);

	//! 
	HWND GetSafeHwnd()const{ return m_hWnd; };
protected:
	// ==================================================================
	// キー操作の機能実装
	//! ALT + F4 実行
	void DoALTF4();

	//! grep 実行
	void DoGrep();

	//!  秀丸 クリップボード履歴
	void DoClipBoardHistory();

	//! キーダウンタスクの実行
	BOOL DoKeyDownTask(WORD* pKeyArray, int nSize);

	//! キーアップタスクの実行
	BOOL DoKeyUpTask(WORD* pKeyArray, int nSize);

	//! キータスクの実行
	BOOL DoKeyTask(WORD* pKeyArray, int nSize, DWORD dwFlags=0);

	//! ダウンアップ
	BOOL DoKeyDownUp(WORD wKey);

	//! アップ
	BOOL DoKeyUp(WORD wKey);

	//! 貼り付け
	void DoPaste();

	//! コピー
	void DoCopy();

	//! ファイル名を指定して実行
	void DoExec();

	//! Win + 数字キー
	void DoWinNumber(int nNumber);

	//! マウス操作
	void DoLButtonDownUp();
	void DoRButtonDownUp();
	void DoMButtonDownUp();
	void DoMouseMove(int x, int y);
	// ==================================================================


	//! キーボード間隔スリープ
	static void IntervalSleep(int nSleep=DEFAULT_INTERVAL_SLEEP){::Sleep(nSleep);}

	//! ログエディットの作成
	BOOL CreateLogEdit();

	//! 修飾キーのキャンセル
	BOOL CancelModKey();

	BOOL IsWIN()const{ return (m_dwModKeyState & MODKEY_WIN) ? TRUE : FALSE; }
	BOOL IsCtrl()const{ return (m_dwModKeyState & MODKEY_CTRL) ? TRUE : FALSE; }
	BOOL IsShift()const{ return (m_dwModKeyState & MODKEY_SHIFT) ? TRUE : FALSE; }
	BOOL IsMenu()const{ return (m_dwModKeyState & MODKEY_MENU) ? TRUE : FALSE; }
	BOOL IsNoModKey()const {return (!IsCtrl() && !IsShift() && !IsMenu() && !IsWIN())?TRUE:FALSE; }

	HWND m_hWnd;
	HWND m_hEdit;
	//! NOTIFYICONDATA の宣言
	NOTIFYICONDATA  m_cTaskTray;

	BOOL m_bClose;

	//! 修飾キー状態管理
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
	//! メニュー状態更新
	void UpdateMenuState();

public:
	//! キーボードイベントハンドラ
	virtual BOOL OnKeyboardEvent(DWORD dwMsg, KBDLLHOOKSTRUCT* pInfo);

};

//! 個別実装
class CMainWndEx : public CMainWnd
{
public:
	CMainWndEx(HWND h) :CMainWnd(h){}
	//! キーボードイベントハンドラ
	BOOL OnKeyboardEvent(DWORD dwMsg, KBDLLHOOKSTRUCT* pInfo);
};

