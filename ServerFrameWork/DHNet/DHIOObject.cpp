#include "stdafx.h"
#include "DHIOObject.h"
#include "DHLog.h"



namespace DHNet
{

	void CIOObject::PostIOCompletion( DWORD transferred, ULONG_PTR lpCompletionKey, LPOVERLAPPED lpoverlapped )
	{
		if( ::PostQueuedCompletionStatus( GetIOCP(), transferred, lpCompletionKey, lpoverlapped ) == FALSE )
		{
			DHLOG_ERROR( L"시스템 메모리 부족 및 큐가 다 차서 iocp queueing 실패.!" );
		}
	}

	void CIOObject::SetIOCP( HANDLE iocp )
	{
		m_iocp = iocp;
	}

	HANDLE CIOObject::GetIOCP()
	{
		return m_iocp;
	}
}