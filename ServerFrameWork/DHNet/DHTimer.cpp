#include "stdafx.h"
#include "DHTimer.h"
#include "DHLog.h"
#include "DHUtil.h"


namespace DHNet
{
	IOBufferPool<TimerParameter> GTimerParamPool;

	void TimerParameter::Clear()
	{
		m_Time = 0;
		iocp = nullptr;
		transfereed = 0;
		completionKey = 0;
		lpOverlapped = nullptr;
	}

	void TimerParameter::SetParameter( DWORD time, HANDLE _iocp, DWORD _transfereed, ULONG_PTR _completionKey, LPOVERLAPPED _lpOverlapped )
	{
		m_Time = time;
		iocp = _iocp;
		transfereed = _transfereed;
		completionKey = _completionKey;
		lpOverlapped = _lpOverlapped;
	}

	CDHTimer	GTimer;

	CDHTimer::CDHTimer()
		: CDHThread( L"CNgTimer" )
	{
	}

	CDHTimer::~CDHTimer()
	{
	}

	bool CDHTimer::Loop()
	{
		LPOVERLAPPED lpOverlapped = NULL;
		DWORD dwBytesTransfer = 0;
        ULONG_PTR ulKey = 0;

		// I/O �� ����� ���ɴϴ�.
		BOOL bOK = ::GetQueuedCompletionStatus( GetIOCP(), &dwBytesTransfer, &ulKey, &lpOverlapped, INFINITE );
		if( bOK == FALSE )
		{
			int iError = WSAGetLastError();

			if( lpOverlapped == NULL && iError == WAIT_TIMEOUT )
				return true;

			switch( iError )
			{
			case 64:
			case 121:
			default:
				{
					DHLOG_ERROR( L"ShutDown IOCP : error(%d)", iError );
				}
				break;
			}
		}

		// ����� I/O Completion packet���� Ȯ��
		if( ulKey == 0 && dwBytesTransfer == 0 && lpOverlapped == NULL )
		{
			if( GetIOCP() != nullptr )
			{
				::CloseHandle( GetIOCP() );
				SetIOCP( nullptr );
			}

			DHLOG_INFO( L"Signal ShutDown IOCP.!" );
			return false;
		}

		if( lpOverlapped == &m_overlapped[OLT_ADDPARAM] )
		{
			auto param = (TimerParameter*)ulKey;
			m_vecTimerParam.push_back( param );
		}

		if( lpOverlapped == &m_overlapped[OLT_TICK] )
		{
			// Ÿ�̸� ȣ��
			DWORD dwCurr = ::GetTickCount();
			std::vector< TimerParameter* > vecUpdateParam;

			auto vit = m_vecTimerParam.begin();
			while( vit != m_vecTimerParam.end() )
			{
				auto param = *vit;
				if( dwCurr < param->m_Time )
				{
					++vit;
					continue;
				}

				vecUpdateParam.push_back( param );

				vit = DHNet::EraseUnorderly( m_vecTimerParam, vit );
			}

			// �ð��� �� param �� �ð��� �ٵǾ��ٰ� IOCP �� ���� �˸�
			for( auto param : vecUpdateParam )
			{
				if( ::PostQueuedCompletionStatus( param->iocp, param->transfereed, param->completionKey, param->lpOverlapped ) == FALSE )
				{
					DHLOG_ERROR( L"�ý��� �޸� ���� �� ť�� �� ���� iocp queueing ����.!" );
				}
				GTimerParamPool.Free( param );
			}

			// Tick Ÿ�� ��� �� �� ȣ��
			if( CDHThread::IsWorking() )
			{
				::Sleep( m_tmTick );

				CIOObject::PostIOCompletion( 0, (ULONG_PTR)this, (LPOVERLAPPED)&m_overlapped[OLT_TICK] );
			}		
		}

		return true;
	}

	bool CDHTimer::Start( DWORD tmTick )
	{
		m_tmTick = tmTick;

		CIOObject::PostIOCompletion( 0, (ULONG_PTR)this, (LPOVERLAPPED)&m_overlapped[OLT_TICK] );

		return CDHThread::Start();
	}

	void CDHTimer::End()
	{
		// I/O ��Ʈ ����
		if( GetIOCP() != nullptr )
		{
			::PostQueuedCompletionStatus( GetIOCP(), 0, 0, 0 );
		}
	}

	void CDHTimer::AddParam( TimerParameter *param )
	{
		param->m_Time += ::GetTickCount();
		CIOObject::PostIOCompletion( 0, (ULONG_PTR)param, (LPOVERLAPPED)&m_overlapped[OLT_ADDPARAM] );
	}

}
