/**
 * キー操作機能の実装
 */
#include "stdafx.h"
#include "MainWnd.h"
#include "KeyHook.h"

using namespace MYAPP;
using namespace KEYHOOK_DLL;

extern HINSTANCE hInst;
extern std::auto_ptr<CMainWnd> g_pMain;


/**
 * ALT + F4 実行
 */
void CMainWnd::DoALTF4()
{
	try
	{
		WORD pKeyDown[] = { VK_MENU, VK_F4 };
		WORD pKeyUp[] = { VK_F4, VK_MENU};

		DoKeyDownTask(pKeyDown, 2);

		DoKeyUpTask(pKeyUp, 2);

		// キーボード操作
		INPUT pInput[4];
		::ZeroMemory(pInput, sizeof(INPUT)* 4);

		AddLog(_T("ALT + F4 を実行しました。"));
	}
	catch (...)
	{
		AddLog(_T("ALT + F4 の実行エラー"));
	}
}

/**
* grep 実行
*/
void CMainWnd::DoGrep()
{
	// Ctrl + Alt + H -> G -> Ctrl + V
	try
	{
		// キーボード操作

		{// Ctrl + Alt + H 
			WORD pKeyDown[] = { VK_CONTROL, VK_MENU, KEY_CODE::VKD_H };
			WORD pKeyUp[] = { KEY_CODE::VKD_H, VK_MENU, VK_CONTROL };

			DoKeyDownTask(pKeyDown, 3);
			DoKeyUpTask(pKeyUp, 3);
		}

		// 間隔をあける
		IntervalSleep();

		DoKeyDownUp(KEY_CODE::VKD_A);

		// 間隔をあける
		IntervalSleep();

		DoKeyDownUp(KEY_CODE::VKD_G);


		AddLog(_T("grep を実行しました。"));
	}
	catch (...)
	{
		AddLog(_T("grep の実行エラー"));
	}
}

//! クリップボード履歴
void CMainWnd::DoClipBoardHistory()
{
	// Ctrl + Alt + H -> H
	try
	{
		// キーボード操作

		WORD pKeyDown[] = { VK_CONTROL, VK_MENU, KEY_CODE::VKD_H };
		WORD pKeyUp[] = { KEY_CODE::VKD_H, VK_MENU, VK_CONTROL };

		DoKeyDownTask(pKeyDown, 3);
		DoKeyUpTask(pKeyUp, 3);

		IntervalSleep();

		DoKeyDownUp(KEY_CODE::VKD_H);

		AddLog(_T("クリップボード履歴 を実行しました。"));
	}
	catch (...)
	{
		AddLog(_T("クリップボード履歴 の実行エラー"));
	}
}

/**
 * キーダウンタスク
 */
BOOL CMainWnd::DoKeyDownTask(WORD* pKeyArray, int nSize)
{
	return DoKeyTask(pKeyArray, nSize);
}

/**
* キーアップタスク
*/
BOOL CMainWnd::DoKeyUpTask(WORD* pKeyArray, int nSize)
{
	return DoKeyTask(pKeyArray, nSize, KEYEVENTF_KEYUP);
}
/**
 * キータスク
 */
BOOL CMainWnd::DoKeyTask(WORD* pKeyArray, int nSize, DWORD dwFlags)
{
	if (nSize == 0)
	{
		return TRUE;
	}
	BOOL bRet = TRUE;
	// キーボード操作
	INPUT* pInput = NULL;
	const int nTotalSize = sizeof(INPUT)*nSize;
	try
	{
		pInput = (INPUT*)new BYTE[nTotalSize];
		if (pInput==NULL)
		{
			throw GetLastError();
		}
		::ZeroMemory(pInput, nTotalSize);

		for (int i = 0; i < nSize; i++)
		{
			//キーセット
			pInput[i].type = INPUT_KEYBOARD;
			pInput[i].ki.dwFlags = dwFlags;
			pInput[i].ki.wVk = pKeyArray[i];
		}
		UINT uRet = ::SendInput(nSize, pInput, sizeof(INPUT));
		if (uRet != (UINT)nSize)
		{// 全ては実行できなかった
			bRet = FALSE;
		}

		delete [] pInput;

		pInput = NULL;
	}
	catch (...)
	{
		if (pInput)
		{
			delete[] pInput;
		}
		bRet = FALSE;
	}
	return TRUE;
}

/**
 *
 */
BOOL CMainWnd::DoKeyDownUp(WORD wKey)
{
	try
	{
		const int nSize = 2;
		INPUT pInput[2];
		::ZeroMemory(pInput, sizeof(INPUT)* 2);
		int index = 0;

		pInput[index].type = INPUT_KEYBOARD;
		pInput[index].ki.wVk = wKey;
		index++;

		pInput[index].type = INPUT_KEYBOARD;
		pInput[index].ki.wVk = wKey;
		pInput[index].ki.dwFlags = KEYEVENTF_KEYUP;

		UINT uRet = ::SendInput(nSize, pInput, sizeof(INPUT));
		if (uRet != (UINT)nSize)
		{// 全ては実行できなかった
			return FALSE;
		}
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}

/**
*
*/
BOOL CMainWnd::DoKeyUp(WORD wKey)
{
	try
	{
		const int nSize = 1;
		INPUT pInput[1];
		::ZeroMemory(pInput, sizeof(INPUT)* 1);
		int index = 0;

		pInput[index].type = INPUT_KEYBOARD;
		pInput[index].ki.wVk = wKey;
		pInput[index].ki.dwFlags = KEYEVENTF_KEYUP;
		index++;

		UINT uRet = ::SendInput(nSize, pInput, sizeof(INPUT));
		if (uRet != (UINT)nSize)
		{// 全ては実行できなかった
			return FALSE;
		}
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}

//! 貼り付け
void CMainWnd::DoPaste()
{
	try
	{
		// Ctrl + V
		WORD pKeyDown[] = { VK_CONTROL, KEY_CODE::VKD_V };
		WORD pKeyUp[] = { KEY_CODE::VKD_V, VK_CONTROL };

		DoKeyDownTask(pKeyDown, 2);
		DoKeyUpTask(pKeyUp, 2);

		AddLog(_T("貼り付け を実行しました。"));
	}
	catch(...)
	{
		AddLog(_T("貼り付け の実行エラー"));
	}
}

//! コピー
void CMainWnd::DoCopy()
{
	try
	{
		// Ctrl + C
		WORD pKeyDown[] = { VK_CONTROL, KEY_CODE::VKD_C };
		WORD pKeyUp[] = { KEY_CODE::VKD_C, VK_CONTROL };

		DoKeyDownTask(pKeyDown, 2);
		DoKeyUpTask(pKeyUp, 2);

		AddLog(_T("コピー を実行しました。"));
	}
	catch(...)
	{
		AddLog(_T("コピー の実行エラー"));
	}
}

//! ファイル名を指定して実行
void CMainWnd::DoExec()
{
	try
	{
		// WIN + R
		WORD pKeyDown[] = { KEY_CODE::VKD_LWIN, KEY_CODE::VKD_R };
		WORD pKeyUp[] = { KEY_CODE::VKD_R, KEY_CODE::VKD_LWIN };

		DoKeyDownTask(pKeyDown, 2);
		DoKeyUpTask(pKeyUp, 2);

		AddLog(_T("ファイル名を指定して実行 を実行しました	。"));
	}
	catch(...)
	{
		AddLog(_T("ファイル名を指定して実行 の実行エラー"));
	}
}

//! Win + 数字キー
void CMainWnd::DoWinNumber(int nNumber)
{
	WORD wNum = KEY_CODE::VKD_1;
	switch(nNumber)
	{
	case 0:wNum = KEY_CODE::VKD_0;break;
	case 1:wNum = KEY_CODE::VKD_1;break;
	case 2:wNum = KEY_CODE::VKD_2;break;
	case 3:wNum = KEY_CODE::VKD_3;break;
	case 4:wNum = KEY_CODE::VKD_4;break;
	case 5:wNum = KEY_CODE::VKD_5;break;
	case 6:wNum = KEY_CODE::VKD_6;break;
	case 7:wNum = KEY_CODE::VKD_7;break;
	case 8:wNum = KEY_CODE::VKD_8;break;
	case 9:wNum = KEY_CODE::VKD_9;break;
	}

	WORD pKey[2];
	pKey[0] = KEY_CODE::VKD_LWIN;
	pKey[1] = wNum;
	DoKeyDownTask(pKey,2);

	pKey[1] = KEY_CODE::VKD_LWIN;
	pKey[0] = wNum;
	DoKeyUpTask(pKey,2);

}


//! マウス操作
void CMainWnd::DoLButtonDownUp()
{
	// 
	INPUT pInput[6];
	::ZeroMemory(pInput, sizeof(INPUT)* 6);
	int nCount = 0;

	pInput[nCount].type = INPUT_MOUSE;
	pInput[nCount].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	nCount++;

	pInput[nCount].type = INPUT_MOUSE;
	pInput[nCount].mi.dwFlags = MOUSEEVENTF_LEFTUP;
	nCount++;

	::SendInput(nCount, pInput, sizeof(INPUT));
}

//! マウス操作
void CMainWnd::DoRButtonDownUp()
{
	// 
	INPUT pInput[6];
	::ZeroMemory(pInput, sizeof(INPUT)* 6);
	int nCount = 0;

	pInput[nCount].type = INPUT_MOUSE;
	pInput[nCount].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	nCount++;

	pInput[nCount].type = INPUT_MOUSE;
	pInput[nCount].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	nCount++;

	::SendInput(nCount, pInput, sizeof(INPUT));
}

//! マウス操作
void CMainWnd::DoMButtonDownUp()
{
	// 
	INPUT pInput[6];
	::ZeroMemory(pInput, sizeof(INPUT)* 6);
	int nCount = 0;

	pInput[nCount].type = INPUT_MOUSE;
	pInput[nCount].mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
	nCount++;

	pInput[nCount].type = INPUT_MOUSE;
	pInput[nCount].mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
	nCount++;

	::SendInput(nCount, pInput, sizeof(INPUT));
}

//! マウス操作
void CMainWnd::DoMouseMove(int x, int y)
{
	// 
	INPUT pInput[6];
	::ZeroMemory(pInput, sizeof(INPUT)* 6);
	int nCount = 0;

	pInput[nCount].type = INPUT_MOUSE;
	pInput[nCount].mi.dwFlags = MOUSEEVENTF_MOVE;

//	int nWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
//	int nHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	pInput[nCount].mi.dx = x;// *0xFFFF / nWidth;
	pInput[nCount].mi.dy = y;// *0xFFFF / nHeight;
	nCount++;

#ifdef _DEBUG
/*	TCHAR str[256];
	_stprintf_s(str, 256, _T("\nScreen(%d,%d),Move(%d,%d)\n"), nWidth, nHeight, pInput[0].mi.dx, pInput[0].mi.dy);
	::OutputDebugString(str);*/
#endif

	::SendInput(nCount, pInput, sizeof(INPUT));
}