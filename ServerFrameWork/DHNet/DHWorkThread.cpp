#include "stdafx.h"
#include "DHWorkThread.h"
#include "DHLog.h"
#include "Message.h"
#include "DHNetServer.h"


namespace DHNet
{

	CWorkThread::CWorkThread()
		: CDHThread( L"WorkThread" )
	{
		::memset( &m_overlappedPostRecv, 0, sizeof( OVERLAPPED ) );
	}

	bool CWorkThread::Loop()
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
				{
					DHLOG_INFO( L"Lan ��� �� ����̹� ������ ���� ���� : error(%d)", iError );
				}
				break;
			default:
				{
					DHLOG_INFO( L"ShutDown Socket : error(%d)", iError );
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

		if( lpOverlapped == &m_overlappedPostRecv )
		{
			auto msg = (CMessage*)ulKey;

			try
			{
				if( m_pNetCoreEvent != nullptr )
				{
					m_pNetCoreEvent->OnReceiveUserMessage( msg->m_HostID, msg );
				}
				else
				{
					DHLOG_ERROR( L"m_pNetCoreEvent nullptr.!" );
				}
			}
			catch( std::exception &e )
			{
				puts( e.what() );
			}

			GMessagePool.Free( msg );
		}

		return true;
	}

	void CWorkThread::OnReceiveMessageProcess( HostID remote, CMessage* msg )
	{
		CIOObject::PostIOCompletion( 0, (ULONG_PTR)msg, (LPWSAOVERLAPPED)&m_overlappedPostRecv );
	}

	void CWorkThread::End()
	{
		// I/O ��Ʈ ����
		if( GetIOCP() != nullptr )
		{
			::PostQueuedCompletionStatus( GetIOCP(), 0, 0, 0 );
		}
	}

	void CWorkThread::SetIndex( UINT index )
	{
		m_index = index;
	}

	void CWorkThread::SetEventSink( INetCoreEvent *eventSink )
	{
		m_pNetCoreEvent = eventSink;
	}

    LPWSAOVERLAPPED CWorkThread::GetOverlappedPostRecv()
    {
        return (LPWSAOVERLAPPED)&m_overlappedPostRecv;
    }

}
