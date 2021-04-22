#include "stdafx.h"
#include "DHIOThread.h"
#include "DHLog.h"
#include "DHSocket.h"


namespace DHNet
{

	CIOThread::CIOThread()
		: CDHThread( L"IOThread" )
	{
	}

	bool CIOThread::Loop()
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

		auto socket = (CSocket*)ulKey;
		socket->OnIOCallback( (bOK ? true : false), dwBytesTransfer, lpOverlapped );
		socket->DecRefCount();	// �Ϸ�� �۾��� ���� ����Ʈ ������ ����.

		return true;
	}

	void CIOThread::End()
	{
		// I/O ��Ʈ ����
		if( GetIOCP() != nullptr )
		{
			::PostQueuedCompletionStatus( GetIOCP(), 0, 0, 0 );
		}
	}

	void CIOThread::SetIndex( UINT index )
	{
		m_index = index;
	}

}