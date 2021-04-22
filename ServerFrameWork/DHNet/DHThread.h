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
# 쓰레드별 저장공간이 따로 생성되는 변수 타입입니다.
# 쓰레드 생성시 순서보장 인덱스를 만들때 사용되어 Thread 구성시 활용
*/
extern __declspec(thread)	int g_tlThreadID;
static long	g_indexThreadID = -1;

/*
class CTest : public DHNet.CNgThread
{
public:
	virtual bool	Loop() override
	{
		쓰레드에서 동작할 코드 작성
		return false; 시 쓰레드 종료
	}
}

CTest cTest;
cTest.Begin();	// 쓰레드 동작 시작
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
		_BEGINTHREADEX 호출. Run()이 호출되는 시점.
		*/
		bool	Start();
		/*
		스레드의 종료를 요청하고 이를 기다렸다가 릴리즈
		*/
		void	End( DWORD timeOut = 2000 );

		bool	IsWorking() const
		{
			return m_bWorking;
		}
		/*
		스레드 정상종료시 호출
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

