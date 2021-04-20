#pragma once
#include <memory>

namespace MYMUTEX
{
	//! ミューテックス管理クラス
	class CMutex
	{
	public:
		CMutex(HANDLE h) :m_hMutext(h){};
		virtual ~CMutex()
		{
			if (m_hMutext)
			{
				::ReleaseMutex(m_hMutext);
				m_hMutext = NULL;
			}
		}

		//! ミューテックス生成
		static BOOL CreateMutex(LPCTSTR strName, std::auto_ptr<CMutex>& pMutex)
		{
			/* ミューテックスを作成する */
			HANDLE hMutex;
			hMutex = ::CreateMutex(NULL, TRUE, strName);

			if (!hMutex)
			{
				return FALSE;
			}
			/* もしGetLastError()が、ERROR_ALREADY_EXISTSなら、
			*  すでにアプリケーションが起動している */
			if (GetLastError() == ERROR_ALREADY_EXISTS)
			{
				/* アプリケーションの起動中止 */
				return FALSE;
			}
			pMutex.reset(new CMutex(hMutex));
			return TRUE;
		}

	private:
		//! 暗黙生成禁止
		CMutex();
		HANDLE m_hMutext;
	};

};