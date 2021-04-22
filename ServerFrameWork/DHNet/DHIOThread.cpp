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

		// I/O 의 결과를 얻어옵니다.
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
					DHLOG_INFO( L"Lan 장비 및 드라이버 문제로 소켓 종료 : error(%d)", iError );
				}
				break;
			default:
				{
					DHLOG_INFO( L"ShutDown Socket : error(%d)", iError );
				}
				break;
			}
		}

		// 종료용 I/O Completion packet인지 확인
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
		socket->DecRefCount();	// 완료된 작업에 대한 스마트 포인터 감소.

		return true;
	}

	void CIOThread::End()
	{
		// I/O 포트 종료
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