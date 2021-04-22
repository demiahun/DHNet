#pragma once

#include <process.h>
#include "DHString.h"
#include "BasicType.h"

#ifndef _BEGINTHREADEX 
typedef unsigned( __stdcall* PTHREAD_START )(void*);
#   define _BEGINTHREADEX( psa, \
	cbStack, \
	pfnStartAddr, \
	pvParam, \
	fdwCreate, \
	pdwThreadId) \
	((HANDLE)_beginthreadex( (void*) (psa), \
		(unsigned) (cbStack), \
		(PTHREAD_START) (pfnStartAddr), \
		(void*) (pvParam), \
		(unsigned) (fdwCreate), \
		(unsigned*) (pdwThreadId)))
#endif

/*
current thread id.(thread local storage)
# �����庰 ��������� ���� �����Ǵ� ���� Ÿ���Դϴ�.
# ������ ������ �������� �ε����� ���鶧 ���Ǿ� Thread ������ Ȱ��
*/
extern __declspec(thread)	int g_tlThreadID;
static long	g_indexThreadID = -1;

/*
class CTest : public DHNet.CNgThread
{
public:
	virtual bool	Loop() override
	{
		�����忡�� ������ �ڵ� �ۼ�
		return false; �� ������ ����
	}
}

CTest cTest;
cTest.Begin();	// ������ ���� ����
*/

namespace DHNet
{
	class CDHThread
	{
	public:
		CDHThread();
		CDHThread( StringW className );
		virtual ~CDHThread();

		virtual	bool Loop() = 0;

		/* 
		_BEGINTHREADEX ȣ��. Run()�� ȣ��Ǵ� ����.
		*/
		bool	Start();
		/*
		�������� ���Ḧ ��û�ϰ� �̸� ��ٷȴٰ� ������
		*/
		void	End( DWORD timeOut = 2000 );

		bool	IsWorking() const
		{
			return m_bWorking;
		}
		/*
		������ ��������� ȣ��
		*/
		void	SetWorking( bool working )
		{
			m_bWorking = working;
		}

		DWORD	GetThreadID();

        UInt64  GetRunCount();

	private:
		void	_Run();		

		static UINT WINAPI ThreadProc( LPVOID param )
		{
			CDHThread *thread = reinterpret_cast<CDHThread*>(param);

			thread->_Run();
			thread->SetWorking( false );

			return 0;
		}

	private:
		HANDLE		m_hKillEvent = nullptr;
		HANDLE      m_hThread = nullptr;
		bool		m_bWorking = false;
		DWORD		m_dwThreadId = 0;

		StringW		m_className;
        UInt64      m_RunCount = 0;
	};
}

