// KeyHook.cpp : DLL �A�v���P�[�V�����p�ɃG�N�X�|�[�g�����֐����`���܂��B
//

#include "stdafx.h"
#include <memory>
#include <vector>
#include <string>
#include "KeyHook.h"

using namespace KEYHOOK_DLL;

extern HINSTANCE g_hInstance;

class CKeyHookManager : public IKeyHookListener 
{
public :
	static CKeyHookManager* GetInstance(){
		if (m_pInstance.get() == NULL)
		{
			m_pInstance.reset(new CKeyHookManager);
		}
		return m_pInstance.get();
	}
	//! �f�X�g���N�^
	virtual ~CKeyHookManager(){
		UnKeyHook();
	}

	//! ���X�i�[�o�^
	void SetListener(IKeyHookListener* p)
	{ 
		m_pListener = p; 
	};

	void ResetKeyHook()
	{
		UnKeyHook();
		SetKeyHook();
	}
	//! �L�[�{�[�h�C�x���g
	virtual BOOL OnKeyboardEvent(DWORD dwMsg, KBDLLHOOKSTRUCT* pInfo){
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

	//! Log
	void AddLog(LPCTSTR str){
		::OutputDebugString(str);
		if (m_pListener)
		{
			m_pListener->AddLog(str);
		}
	}
private:
	CKeyHookManager() :m_pListener(NULL){SetKeyHook();} //!< �R���X�g���N�^

	//! KeyBoard Hook
	static LRESULT CALLBACK KeyboardProc(int p_nCode, WPARAM p_wParam, LPARAM p_lParam);

	//! Set Key Hook
	BOOL SetKeyHook(){
		//�t�b�N���Z�b�g
		if (m_hHook)
		{
			return TRUE;
		}

		m_hHook = ::SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardProc, g_hInstance, 0);
		if (!m_hHook)
		{
			std::wstring strErr;
			GetLastErrorString(GetLastError(),strErr);

			TCHAR tc[2048];
			_stprintf_s(tc,2048,_T("HOOK ERROR : %s"), strErr.c_str());
			AddLog(tc);
			return FALSE;
		}
		return TRUE;
	}
	void UnKeyHook(){
		if (m_hHook)
		{
			if (UnhookWindowsHookEx(m_hHook))
			{
				m_hHook = NULL;
			}
		}
	}

	static HHOOK m_hHook;								//!< �L�[�t�b�N
	static std::auto_ptr<CKeyHookManager> m_pInstance;	//!< �C���X�^���X

	IKeyHookListener* m_pListener;						//!< ���X�i�[

	static BOOL GetLastErrorString(DWORD dwErrorNo, std::wstring& strErr);
};

std::auto_ptr<CKeyHookManager> CKeyHookManager::m_pInstance;
HHOOK CKeyHookManager::m_hHook = NULL;

/**
* �t�b�N�v���V�W��
*/
LRESULT CALLBACK CKeyHookManager::KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	try
	{
		if (nCode == HC_ACTION)
		{// �A�N�V�����̂�
			if (CKeyHookManager::GetInstance()->OnKeyboardEvent((DWORD)wParam, (KBDLLHOOKSTRUCT*)lParam))
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
* �G���[������擾
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

//! ���X�i�[�o�^
void WINAPI SetKeyHookListener(IKeyHookListener* p)
{
	CKeyHookManager::GetInstance()->SetListener(p);
}

void WINAPI ResetKeyHook()
{
	CKeyHookManager::GetInstance()->ResetKeyHook();
}