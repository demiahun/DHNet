#include "stdafx.h"
#include "DHIOObject.h"
#include "DHLog.h"



namespace DHNet
{

	void CIOObject::PostIOCompletion( DWORD transferred, ULONG_PTR lpCompletionKey, LPOVERLAPPED lpoverlapped )
	{
		if( ::PostQueuedCompletionStatus( GetIOCP(), transferred, lpCompletionKey, lpoverlapped ) == FALSE )
		{
			DHLOG_ERROR( L"�ý��� �޸� ���� �� ť�� �� ���� iocp queueing ����.!" );
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