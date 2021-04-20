#include "stdafx.h"
#include "KeyHookManager.h"
#include <algorithm>

using namespace MYKEYHOOK;

// 外部依存
extern HINSTANCE hInst;

std::auto_ptr<CKeyHookManager> CKeyHookManager::m_pInstance;
HHOOK CKeyHookManager::m_hHook = NULL;
/**
* インスタンス取得
*/
CKeyHookManager* CKeyHookManager::Instance()
{
	if (m_pInstance.get() == NULL)
	{
		m_pInstance.reset(new CKeyHookManager);
	}
	return m_pInstance.get();
}


/**
* コンストラクタ
*/
CKeyHookManager::CKeyHookManager() :m_pListener(NULL)
{
	SetKeyHook();
}

/**
* デストラクタ
*/
CKeyHookManager::~CKeyHookManager()
{
	UnKeyHook();
}

/**
* フックプロシジャ
*/
LRESULT CALLBACK CKeyHookManager::KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	try
	{
		if (nCode == HC_ACTION)
		{// アクションのみ
			if (CKeyHookManager::Instance()->OnKeyboardEvent((DWORD)wParam, (KBDLLHOOKSTRUCT*)lParam))
			{
				return 1;
			}
		}
	}
	catch (...)
	{
	}
	return ::CallNextHookEx(m_hHook, nCode, wParam, lParam);
}

/**
* キーボードフック
*/
BOOL CKeyHookManager::SetKeyHook()
{
	//フックをセット
	if (m_hHook)
	{
		return TRUE;
	}

	m_hHook = ::SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardProc, hInst, 0);
	if (!m_hHook)
	{
		std::wstring strErr;
		GetLastErrorString(GetLastError(),strErr);

		TCHAR tc[2048];
		_stprintf_s(tc,2048,_T("HOOK ERROR : %s"), strErr.c_str());
		AddLogMessage(tc);
		return FALSE;
	}
	return TRUE;
}


/**
* キーボードフック解除
*/
void CKeyHookManager::UnKeyHook()
{
	if (m_hHook)
	{
		if (UnhookWindowsHookEx(m_hHook))
		{
			m_hHook = NULL;
		}
	}
}

/**
* キー処理
*/
BOOL CKeyHookManager::OnKeyboardEvent(DWORD dwMsg, KBDLLHOOKSTRUCT* pInfo)
{
	try
	{
		if (m_pListener)
		{
			return m_pListener->OnKeyboardEvent(dwMsg, pInfo);
		}
	}
	catch (...)
	{
	}
	return FALSE;
}

/**
* ログ出力
*/
void CKeyHookManager::AddLogMessage(LPCTSTR str)
{
	::OutputDebugString(str);
	if (m_pListener)
	{
		m_pListener->AddLogMessage(str);
	}
}

/**
* エラー文字列取得
*/
BOOL CKeyHookManager::GetLastErrorString(DWORD dwErrorNo, std::wstring& strErr)
{
	LPVOID lpMsgBuf = NULL;

	if ((FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK,
		NULL,
		dwErrorNo,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		NULL)) == 0 || lpMsgBuf == NULL)
	{
		return FALSE;
	}

	strErr = (LPTSTR)lpMsgBuf;
	LocalFree(lpMsgBuf);

	return TRUE;

}


std::vector<CKeyData::KEYUNIT> CKeyData::m_vecKeyCodeSort;
std::vector<CKeyData::KEYUNIT> CKeyData::m_vecKeyNameSort;
std::vector<CKeyData::KEYUNIT> CKeyData::m_vecKeyAliasSort;

/**
* キーコードからキー取得
*/
CKeyData::KEYUNIT CKeyData::GetKeyByKeyCode(DWORD dwKey)
{
	Initialize();

	int nEnd = (int)m_vecKeyCodeSort.size();
	int nStart = 0;
	while (nStart<nEnd)
	{
		int nTarget = (nStart + nEnd) / 2;
		KEYUNIT cTemp = m_vecKeyCodeSort[nTarget];
		if (cTemp.dwKeyCode == dwKey)
		{
			return cTemp;
		}
		else
		{
			if (cTemp.dwKeyCode > dwKey)
			{
				nEnd = nTarget;
			}
			else
			{
				nStart = nTarget + 1;
			}
		}
	}
	return KEYUNIT(0, _T(""));
}

/**
* 名前からキー取得
*/
CKeyData::KEYUNIT CKeyData::GetKeyByName(LPCTSTR strKeyName)
{
	Initialize();

	//! 
	int nEnd = (int)m_vecKeyNameSort.size();
	int nStart = 0;
	while (nStart<nEnd)
	{
		int nTarget = (nStart + nEnd) / 2;
		KEYUNIT cTemp = m_vecKeyNameSort[nTarget];
		if (_tccmp(cTemp.strKeyName.c_str(),strKeyName) == 0)
		{
			return cTemp;
		}
		else
		{
			if (_tccmp(cTemp.strKeyName.c_str(),strKeyName) >0)
			{
				nEnd = nTarget;
			}
			else
			{
				nStart = nTarget + 1;
			}
		}
	}
	return KEYUNIT(0, _T(""));
}

/**
* エイリアス検索
*/
int CKeyData::GetKeysByAlias(LPCTSTR strKeyAlias, CKeyData::KEYUNITS& vecFind)
{
	vecFind.clear();

	// Alias
	int nEnd = (int)m_vecKeyAliasSort.size();
	int nStart = 0;
	while (nStart<nEnd)
	{
		int nTarget = (nStart + nEnd) / 2;
		KEYUNIT cTemp = m_vecKeyAliasSort[nTarget];
		if (_tcscmp(cTemp.strAlias.c_str(),strKeyAlias) == 0)
		{
			break;
		}
		else
		{
			if (_tcscmp(cTemp.strAlias.c_str(), strKeyAlias) >0)
			{
				nEnd = nTarget;
			}
			else
			{
				nStart = nTarget + 1;
			}
		}
	}
	if (nEnd < nStart)
	{
		return 0;
	}

	int nCount = nEnd - nStart;
	KEYUNITS::iterator iter = m_vecKeyAliasSort.begin() + nStart;
	for (int i = 0; i<nCount; i++, iter++)
	{
		if (_tcscmp(iter->strAlias.c_str(),strKeyAlias) == 0)
		{
			vecFind.push_back(*iter);
		}
		else if (vecFind.size() > 0)
		{
			break;
		}
	}
	return (int)vecFind.size();
}

// 比較関数
BOOL compare_keyname(const CKeyData::KEYUNIT& left, const CKeyData::KEYUNIT& right)
{
	return _tcscmp(left.strKeyName.c_str(),right.strKeyName.c_str())<0;
}
BOOL compare_keycode(const CKeyData::KEYUNIT& left, const CKeyData::KEYUNIT& right)
{
	return left.dwKeyCode < right.dwKeyCode;
}
BOOL compare_keyalias(const CKeyData::KEYUNIT& left, const CKeyData::KEYUNIT& right)
{
	return _tcscmp(left.strAlias.c_str(),right.strAlias.c_str())<0;
}

/**
* 初期化
*/
void CKeyData::Initialize()
{
	static BOOL bInit = TRUE;
	if (bInit)
	{
		bInit = FALSE;
		m_vecKeyCodeSort.clear();
		m_vecKeyNameSort.clear();
		m_vecKeyAliasSort.clear();

		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_LBUTTON, _T("LBUTTON")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_RBUTTON, _T("RBUTTON")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_CANCEL, _T("CANCEL"), _T("Break")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_MBUTTON, _T("MBUTTON")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_XBUTTON1, _T("XBUTTON1")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_XBUTTON2, _T("XBUTTON2")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_BACK, _T("BACK")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_TAB, _T("TAB")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_CLEAR, _T("CLEAR")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_RETURN, _T("RETURN")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_SHIFT, _T("SHIFT")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_CONTROL, _T("CONTROL")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_ALT, _T("ALT")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_PAUSE, _T("PAUSE"), _T("Pause")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_CAPITAL, _T("CAPITAL")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_KANA, _T("KANA"), _T("カタカナ/ひらがな/ローマ字"), TRUE));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_JUNJA, _T("JUNJA")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_FINAL, _T("FINAL")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_KANJI, _T("KANJI"), _T("全角/半角"), TRUE));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_ESCAPE, _T("ESCAPE"), _T("Escape")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_CONVERT, _T("CONVERT"), _T("変換")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_NONCONVERT, _T("NONCONVERT"), _T("無変換")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_ACCEPT, _T("ACCEPT")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_MODECHANGE, _T("MODECHANGE")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_SPACE, _T("SPACE"), _T("Space")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_PRIOR, _T("PRIOR"), _T("Prior")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_NEXT, _T("NEXT"), _T("Next")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_END, _T("END"), _T("End")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_HOME, _T("HOME"), _T("Home")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_LEFT, _T("LEFT"), _T("Left")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_UP, _T("UP"), _T("Up")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_RIGHT, _T("RIGHT"), _T("Right")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_DOWN, _T("DOWN"), _T("Down")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_SELECT, _T("SELECT")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_PRINT, _T("PRINT")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_EXECUTE, _T("EXECUTE")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_SNAPSHOT, _T("SNAPSHOT"), _T("PrintScreen")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_INSERT, _T("INSERT"), _T("Insert")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_DELETE, _T("DELETE")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_HELP, _T("HELP")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_0, _T("0")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_1, _T("1")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_2, _T("2")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_3, _T("3")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_4, _T("4")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_5, _T("5")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_6, _T("6")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_7, _T("7")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_8, _T("8")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_9, _T("9")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_A, _T("A")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_B, _T("B")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_C, _T("C")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_D, _T("D")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_E, _T("E")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_F, _T("F")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_G, _T("G")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_H, _T("H")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_I, _T("I")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_J, _T("J")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_K, _T("K")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_L, _T("L")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_M, _T("M")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_N, _T("N")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_O, _T("O")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_P, _T("P")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_Q, _T("Q")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_R, _T("R")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_S, _T("S")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_T, _T("T")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_U, _T("U")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_V, _T("V")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_W, _T("W")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_X, _T("X")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_Y, _T("Y")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_Z, _T("Z")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_LWIN, _T("LWIN"), _T("Win(左)")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_RWIN, _T("RWIN"), _T("Win(右)")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_APPS, _T("APPS"), _T("Application")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_SLEEP, _T("SLEEP")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_NUMPAD0, _T("NUMPAD0")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_NUMPAD1, _T("NUMPAD1")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_NUMPAD2, _T("NUMPAD2")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_NUMPAD3, _T("NUMPAD3")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_NUMPAD4, _T("NUMPAD4")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_NUMPAD5, _T("NUMPAD5")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_NUMPAD6, _T("NUMPAD6")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_NUMPAD7, _T("NUMPAD7")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_NUMPAD8, _T("NUMPAD8")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_NUMPAD9, _T("NUMPAD9")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_MULTIPLY, _T("MULTIPLY")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_ADD, _T("ADD")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_SEPARATOR, _T("SEPARATOR")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_SUBTRACT, _T("SUBTRACT")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_DECIMAL, _T("DECIMAL")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_DIVIDE, _T("DIVIDE")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_F1, _T("F1")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_F2, _T("F2")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_F3, _T("F3")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_F4, _T("F4")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_F5, _T("F5")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_F6, _T("F6")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_F7, _T("F7")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_F8, _T("F8")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_F9, _T("F9")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_F10, _T("F10")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_F11, _T("F11")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_F12, _T("F12")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_F13, _T("F13")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_F14, _T("F14")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_F15, _T("F15")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_F16, _T("F16")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_F17, _T("F17")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_F18, _T("F18")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_F19, _T("F19")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_F20, _T("F20")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_F21, _T("F21")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_F22, _T("F22")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_F23, _T("F23")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_F24, _T("F24")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_NUMLOCK, _T("NUMLOCK"), _T("NumLock")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_SCROLL, _T("SCROLLLOCK"), _T("ScrollLock")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_LSHIFT, _T("LSHIFT"), _T("Shift(左)")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_RSHIFT, _T("RSHIFT"), _T("Shift(右)")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_LCONTROL, _T("LCONTROL"), _T("Control(左)")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_RCONTROL, _T("RCONTROL"), _T("Control(右)")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_LMENU, _T("LMENU"), _T("Alt(左)")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_RMENU, _T("RMENU"), _T("Alt(右)")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_BROWSER_BACK, _T("BROWSER_BACK")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_BROWSER_FORWARD, _T("BROWSER_FORWARD")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_BROWSER_REFRESH, _T("BROWSER_REFRESH")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_BROWSER_STOP, _T("BROWSER_STOP")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_BROWSER_SEARCH, _T("BROWSER_SEARCH")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_BROWSER_FAVORITES, _T("BROWSER_FAVORITES")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_BROWSER_HOME, _T("BROWSER_HOME")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_VOLUME_MUTE, _T("VOLUME_MUTE")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_VOLUME_DOWN, _T("VOLUME_DOWN")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_VOLUME_UP, _T("VOLUME_UP")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_MEDIA_NEXT_TRACK, _T("MEDIA_NEXT_TRACK")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_MEDIA_PREV_TRACK, _T("MEDIA_PREV_TRACK")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_MEDIA_STOP, _T("MEDIA_STOP")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_MEDIA_PLAY_PAUSE, _T("MEDIA_PLAY_PAUSE")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_LAUNCH_MAIL, _T("LAUNCH_MAIL")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_LAUNCH_MEDIA_SELECT, _T("LAUNCH_MEDIA_SELECT")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_LAUNCH_APP1, _T("LAUNCH_APP1")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_LAUNCH_APP2, _T("LAUNCH_APP2")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_1, _T("OEM_1")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_PLUS, _T("OEM_PLUS")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_COMMA, _T("OEM_COMMA")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_MINUS, _T("OEM_MINUS")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_PERIOD, _T("OEM_PERIOD")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_2, _T("OEM_2")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_3, _T("OEM_3")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_4, _T("OEM_4")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_5, _T("OEM_5")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_6, _T("OEM_6")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_7, _T("OEM_7")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_8, _T("OEM_8")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_AX, _T("OEM_AX")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_102, _T("OEM_102")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_ICO_HELP, _T("ICO_HELP")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_ICO_00, _T("ICO_00")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_PROCESSKEY, _T("PROCESSKEY")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_ICO_CLEAR, _T("ICO_CLEAR")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_PACKET, _T("PACKET")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_RESET, _T("OEM_RESET")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_JUMP, _T("OEM_JUMP")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_PA1, _T("OEM_PA1")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_PA2, _T("OEM_PA2")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_PA3, _T("OEM_PA3")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_WSCTRL, _T("OEM_WSCTRL")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_CUSEL, _T("OEM_CUSEL")));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_ATTN, _T("OEM_ATTN"), _T("CapsLock"), TRUE));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_FINISH, _T("OEM_FINISH"), _T("カタカナ/ひらがな/ローマ字"), TRUE));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_COPY, _T("OEM_COPY"), _T("カタカナ/ひらがな/ローマ字"), TRUE));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_AUTO, _T("OEM_AUTO"), _T("半角"), TRUE));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_ENLW, _T("OEM_ENLW"), _T("全角"), TRUE));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_BACKTAB, _T("OEM_BACKTAB"), _T("カタカナ/ひらがな/ローマ字"), TRUE));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_ATTN, _T("ATTN"), _T("カタカナ/ひらがな/ローマ字"), TRUE));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_CRSEL, _T("CRSEL"), _T(""), TRUE));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_EXSEL, _T("EXSEL"), _T(""), TRUE));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_EREOF, _T("EREOF"), _T(""), TRUE));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_PLAY, _T("PLAY"), _T(""), TRUE));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_ZOOM, _T("ZOOM"), _T(""), TRUE));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_NONAME, _T("NONAME"), _T(""), TRUE));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_PA1, _T("PA1"), _T(""), TRUE));
		m_vecKeyCodeSort.push_back(KEYUNIT(VKD_OEM_CLEAR, _T("OEM_CLEAR"), _T(""), TRUE));

		m_vecKeyNameSort.insert(m_vecKeyNameSort.begin(), m_vecKeyCodeSort.begin(), m_vecKeyCodeSort.end());
		m_vecKeyAliasSort.insert(m_vecKeyAliasSort.begin(), m_vecKeyCodeSort.begin(), m_vecKeyCodeSort.end());

		std::sort(m_vecKeyCodeSort.begin(), m_vecKeyCodeSort.end(), compare_keycode);
		std::sort(m_vecKeyNameSort.begin(), m_vecKeyNameSort.end(), compare_keyname);
		std::sort(m_vecKeyAliasSort.begin(), m_vecKeyAliasSort.end(), compare_keyalias);

	}
}


