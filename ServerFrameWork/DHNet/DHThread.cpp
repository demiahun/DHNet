#include "stdafx.h"
#include "DHThread.h"
#include "DHLog.h"
#include <assert.h>

__declspec(thread)	int g_tlThreadID;

namespace DHNet
{
	CDHThread::CDHThread()
	{}
	
	CDHThread::CDHThread( StringW className )
	{
		m_className = className;
	}

	CDHThread::~CDHThread()
	{
		End();
	}

	bool CDHThread::Start()
	{
		if( m_hThread != nullptr )
			return false;

		m_hKillEvent = ::CreateEvent( NULL, false, false, NULL );
		if( m_hKillEvent == nullptr )
		{
			DHLOG_ERROR( L"fail CreateEvent" );
			return false;
		}

		m_hThread = _BEGINTHREADEX( NULL, 0, ThreadProc, this, 0, &m_dwThreadId );
		if( m_hThread == nullptr )
		{
			::CloseHandle( m_hKillEvent );
			m_hKillEvent = nullptr;
			DHLOG_ERROR( L"fail BeginThread : ChildName(%s)", m_className.GetString() );
			return false;
		}

		return true;
	}

	void CDHThread::End( DWORD timeOut )
	{
		if( m_hThread == nullptr )
			return;

		::SetEvent( m_hKillEvent );

		DWORD ret = ::WaitForSingleObject( m_hThread, timeOut );
		::CloseHandle( m_hThread );
		m_hThread = nullptr;

		if( m_hKillEvent != nullptr )
		{
			::CloseHandle( m_hKillEvent );
			m_hKillEvent = nullptr;
		}

		if( ret == WAIT_TIMEOUT )
		{
			DHLOG_ERROR( L"Thread �� ���� ������� ������ : ThreadID(%d), childName(%s)", m_dwThreadId, m_className.GetString() );
		}
	}

	DWORD CDHThread::GetThreadID()
	{
		return m_dwThreadId;
	}

    UInt64 CDHThread::GetRunCount()
    {
        return m_RunCount;
    }

    void CDHThread::_Run()
	{
		g_tlThreadID = InterlockedIncrement( &g_indexThreadID );

		DWORD ret;
		SetWorking( true );

		while( true )
		{
			ret = ::WaitForSingleObject( m_hKillEvent, 0 );		// 1 millisecond �������� ȣ��ȴ�.(���� �����忡�� ���� sleep() �� ȣ������ �ʵ���)

			if( ret == WAIT_OBJECT_0 )
				break;

			if( ret == WAIT_TIMEOUT )
			{
				if( Loop() == false )
					break;
			}

			if( IsWorking() == false )
				break;

            ++m_RunCount;

            if( m_RunCount == UINT64_MAX )
                m_RunCount = 0;
		}

		DHLOG_INFO( L"Thread ���� ���� : ThreadID(%d), childName(%s)", m_dwThreadId, m_className.GetString() );
	}

}
