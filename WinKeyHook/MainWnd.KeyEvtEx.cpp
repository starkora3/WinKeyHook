/**
* キーボードイベント処理
*/
#include "stdafx.h"
#include "MainWnd.h"
#include "KeyHook.h"

using namespace MYAPP;
using namespace KEYHOOK_DLL;

extern HINSTANCE hInst;
extern std::auto_ptr<CMainWnd> g_pMain;


/**
* キーボードイベント
*/
BOOL CMainWndEx::OnKeyboardEvent(DWORD dwMsg, KBDLLHOOKSTRUCT* pInfo)
{
	BOOL bRet = FALSE;

	switch (dwMsg)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		switch (pInfo->vkCode)
		{
		case KEY_CODE::VKD_Q:
			if (IsCtrl())
			{// Ctrl + Q	
				// 連続利用するためには、
				// Ctrlキーの状態を復元する必要がある
				// 状態復元用
				WORD pCtrlKey[2];
				int nCtrlCount = 1;
				if ((m_dwModKeyState & MODKEY_CTRL) == MODKEY_CTRL)
				{
					pCtrlKey[0] = KEY_CODE::VKD_LCONTROL;
					pCtrlKey[1] = KEY_CODE::VKD_RCONTROL;
					nCtrlCount = 2;
				}
				else if (m_dwModKeyState & MODKEY_LCTRL)
				{
					pCtrlKey[0] = KEY_CODE::VKD_LCONTROL;
				}
				else
				{
					pCtrlKey[0] = KEY_CODE::VKD_RCONTROL;
				}

				// Alt + F4 に Ctrl キーが影響するので、
				// Ctrl + Q の押下状態を解除する。
				WORD wKey = KEY_CODE::VKD_Q;
				DoKeyUpTask(&wKey, 1);

				//! 修飾キーのキャンセル
				CancelModKey();

				// Alt + F4
				DoALTF4();

				// キーイベントを横取りする
				bRet = TRUE;
			}
			break;
		case KEY_CODE::VKD_OEM_ATTN:// CapsLock
			if (!IsWIN() && !IsShift() && !IsMenu())
			{// CapsLock と Ctrl + CapsLock をフックする
				BOOL bCtrl = FALSE;
				if (IsCtrl())
				{
					bCtrl = TRUE;
				}

				//! 修飾キーのキャンセル
				CancelModKey();

				if (bCtrl)
				{
					//! Shift x 2
					DoKeyDownUp(KEY_CODE::VKD_LSHIFT);
					DoKeyDownUp(KEY_CODE::VKD_LSHIFT);
				}
				else
				{
					//! ALT x 2
					DoKeyDownUp(KEY_CODE::VKD_LMENU);
					DoKeyDownUp(KEY_CODE::VKD_LMENU);
				}

				// キーイベントを横取りする
				bRet = TRUE;
			}
			break;
		/*case KEY_CODE::VKD_A:
			if (IsWIN())
			{// WIN + A 
				// キャンセル処理
				WORD pKeyUp[] = { KEY_CODE::VKD_A };
				DoKeyUpTask(pKeyUp, 1);

				//! 修飾キーのキャンセル
				CancelModKey();

				// ここに処理を記述する
				AddLog(_T("Windows キーのフックイベント"));

				// キーイベントを横取りする
				bRet = TRUE;
			}
			break;*/
		}
		break;
	}

	if (__super::OnKeyboardEvent(dwMsg, pInfo))
	{
		bRet = TRUE;
	}
	return bRet;
}
