#pragma once
#include <memory>

namespace MYMUTEX
{
	//! �~���[�e�b�N�X�Ǘ��N���X
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

		//! �~���[�e�b�N�X����
		static BOOL CreateMutex(LPCTSTR strName, std::auto_ptr<CMutex>& pMutex)
		{
			/* �~���[�e�b�N�X���쐬���� */
			HANDLE hMutex;
			hMutex = ::CreateMutex(NULL, TRUE, strName);

			if (!hMutex)
			{
				return FALSE;
			}
			/* ����GetLastError()���AERROR_ALREADY_EXISTS�Ȃ�A
			*  ���łɃA�v���P�[�V�������N�����Ă��� */
			if (GetLastError() == ERROR_ALREADY_EXISTS)
			{
				/* �A�v���P�[�V�����̋N�����~ */
				return FALSE;
			}
			pMutex.reset(new CMutex(hMutex));
			return TRUE;
		}

	private:
		//! �Öِ����֎~
		CMutex();
		HANDLE m_hMutext;
	};

};