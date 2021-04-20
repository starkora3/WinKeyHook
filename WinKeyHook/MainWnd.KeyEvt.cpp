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

//! キーボードイベントハンドラ
BOOL CMainWnd::OnKeyboardEvent(DWORD dwMsg, KBDLLHOOKSTRUCT* pInfo)
{
	BOOL bRet = FALSE;
#ifdef _DEBUG
	{
		TCHAR tc[1024];
		_stprintf_s(tc, 1024, _T("ModKeyState:%x, IsWIN:%ld"), m_dwModKeyState, IsWIN());
		::OutputDebugString(tc);
		AddLog(tc);
	}
#endif
	switch (dwMsg)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		switch (pInfo->vkCode)
		{// 修飾キー管理
		case KEY_CODE::VKD_LWIN:m_dwModKeyState |= MODKEY_LWIN; break;
		case KEY_CODE::VKD_RWIN:m_dwModKeyState |= MODKEY_RWIN; break;
		case KEY_CODE::VKD_LCONTROL:m_dwModKeyState |= MODKEY_LCTRL; break;
		case KEY_CODE::VKD_RCONTROL:m_dwModKeyState |= MODKEY_RCTRL; break;
		case KEY_CODE::VKD_LSHIFT:m_dwModKeyState |= MODKEY_LSHIFT; break;
		case KEY_CODE::VKD_RSHIFT:m_dwModKeyState |= MODKEY_RSHIFT; break;
		case KEY_CODE::VKD_LMENU:m_dwModKeyState |= MODKEY_LMENU; break;
		case KEY_CODE::VKD_RMENU:m_dwModKeyState |= MODKEY_RMENU; break;
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		switch (pInfo->vkCode)
		{// 修飾キー管理
		case KEY_CODE::VKD_LWIN:m_dwModKeyState &= ~MODKEY_LWIN; break;
		case KEY_CODE::VKD_RWIN:m_dwModKeyState &= ~MODKEY_RWIN; break;
		case KEY_CODE::VKD_LCONTROL:m_dwModKeyState &= ~MODKEY_LCTRL; break;
		case KEY_CODE::VKD_RCONTROL:m_dwModKeyState &= ~MODKEY_RCTRL; break;
		case KEY_CODE::VKD_LSHIFT:m_dwModKeyState &= ~MODKEY_LSHIFT; break;
		case KEY_CODE::VKD_RSHIFT:m_dwModKeyState &= ~MODKEY_RSHIFT; break;
		case KEY_CODE::VKD_LMENU:m_dwModKeyState &= ~MODKEY_LMENU; break;
		case KEY_CODE::VKD_RMENU:m_dwModKeyState &= ~MODKEY_RMENU; break;
		}
		break;
	}

	if(m_bKeyDisp)
	{
		std::wstring strMsg;
		switch (dwMsg)
		{
		case WM_KEYDOWN:strMsg = _T("KeyDown"); break;
		case WM_SYSKEYDOWN:strMsg = _T("SysKeyDown"); break;
		case WM_KEYUP:strMsg = _T("KeyUp"); break;
		case WM_SYSKEYUP:strMsg = _T("SysKeyUp"); break;
		}

		TCHAR strKeyName[1024];
		GetKeyName(pInfo->vkCode,strKeyName);

		TCHAR tc[2048];
		_stprintf_s(tc, 2048, _T("[%s] %s ,%x,%ld"), strMsg.c_str(), strKeyName,pInfo->vkCode, pInfo->time);
		AddLog(tc);
	}

	return bRet;
}

//! 修飾キーのキャンセル
BOOL CMainWnd::CancelModKey()
{
	BOOL bRet = TRUE;
	try
	{
		WORD pKey[8];
		int index = 0;

		if (m_dwModKeyState&MODKEY_LWIN)
		{
			pKey[index] = KEY_CODE::VKD_LWIN;
			index++;
		}
		if (m_dwModKeyState&MODKEY_RWIN)
		{
			pKey[index] = KEY_CODE::VKD_RWIN;
			index++;
		}
		if (m_dwModKeyState&MODKEY_LCTRL)
		{
			pKey[index] = KEY_CODE::VKD_LCONTROL;
			index++;
		}
		if (m_dwModKeyState&MODKEY_RCTRL)
		{
			pKey[index] = KEY_CODE::VKD_RCONTROL;
			index++;
		}
		if (m_dwModKeyState&MODKEY_LSHIFT)
		{
			pKey[index] = KEY_CODE::VKD_LSHIFT;
			index++;
		}
		if (m_dwModKeyState&MODKEY_RSHIFT)
		{
			pKey[index] = KEY_CODE::VKD_RSHIFT;
			index++;
		}
		if (m_dwModKeyState&MODKEY_LMENU)
		{
			pKey[index] = KEY_CODE::VKD_LMENU;
			index++;
		}
		if (m_dwModKeyState&MODKEY_RMENU)
		{
			pKey[index] = KEY_CODE::VKD_RMENU;
			index++;
		}

		if (index)
		{
			bRet = DoKeyUpTask(pKey, index);
		}
	}
	catch (...)
	{
		return FALSE;
	}
	return bRet;
}
