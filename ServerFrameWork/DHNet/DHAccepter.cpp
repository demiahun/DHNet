#include "stdafx.h"
#include "DHAccepter.h"
#include "DHLog.h"
#include "DHNetServer.h"

#if (_WIN32_WINNT >= 0x0600)
#include <WS2tcpip.h>	// �ּ����� OS : vista �̻� / Server 2008 �̻�
#pragma comment( lib, "Ws2_32.lib" )
#endif

namespace DHNet
{

	CAccepter::CAccepter()
		: CDHThread( L"Accepter" )
	{
	}

	CAccepter::~CAccepter()
	{

	}

	bool CAccepter::Loop()
	{
		SOCKET socket = INVALID_SOCKET;
		SOCKADDR_IN sinRemote;
		int size = sizeof( sinRemote );

		if( m_Socket == INVALID_SOCKET )	// listen �� ���� ���� üũ.
		{
			DHLOG_ERROR( L"m_Socket == INVALID_SOCKET" );
			return false;
		}

		socket = ::accept( m_Socket, (SOCKADDR*)&sinRemote, &size );
		if( socket == INVALID_SOCKET )
		{
			if( m_Socket != INVALID_SOCKET )
			{
				DHLOG_ERROR( L"fail : accept() .!" );
			}

			return true;
		}

		int sock_opt = 1;
		if( ::setsockopt( socket, IPPROTO_TCP, TCP_NODELAY, (char*)&sock_opt, sizeof( sock_opt ) ) == SOCKET_ERROR )
		{
			DHLOG_ERROR( L"fail : setsocketopt(%d)", WSAGetLastError() );
			return true;
		}

		/*
		Socket ����
			IOThread �� SendRecv IOCP ���ε�
			DHNetServer �� ������ �����Ͽ� WorkThread ���� DHNetServer �� ���� Send �� �� �ֵ��� ����.
		*/
		auto spSocket = CSocketPtr( new CSocket() );

		spSocket->SetConnectionInfo( socket, sinRemote );

        m_INetCoreEvent->AddSocket( spSocket );

		return true;
	}

	bool CAccepter::Start( const WCHAR *szIP, int iPort )
	{
		m_Socket = ::WSASocket( AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED );

		if( m_Socket == INVALID_SOCKET )
		{
			DHLOG_ERROR( L"m_Socket == INVALID_SOCKET" );
			return false;
		}

		char cOK = 1;
		int iRet;
		iRet = ::setsockopt( m_Socket, SOL_SOCKET, SO_REUSEADDR, &cOK, sizeof( char ) );

		SOCKADDR_IN in;  // Ư�� port�� binding�� �Ѵ�.
		::ZeroMemory( in.sin_zero, sizeof( char ) * (sizeof( in.sin_zero ) / sizeof( in.sin_zero[0] )) );
		in.sin_family = AF_INET;		
		in.sin_port = ::htons( iPort );

	#if (_WIN32_WINNT >= 0x0600)
		/*
		Domain ������ �Ʒ� ���� ����
		https://msdn.microsoft.com/ko-kr/library/windows/desktop/ms738524(v=vs.85).aspx
		*/
		if( szIP == nullptr )
		{
			in.sin_addr.s_addr = INADDR_ANY;
		}
		else
		{
			if( InetPtonW( AF_INET, szIP, &in.sin_addr ) != 1 )
			{
				DHLOG_ERROR( L"fail InetPtonW : Error(%d)", WSAGetLastError() );
				return false;
			}
		}		
	#else
		in.sin_addr.s_addr = INADDR_ANY;
	#endif

		if( ::bind( m_Socket, (SOCKADDR *)&in, sizeof( in ) ) == SOCKET_ERROR )
		{
			CLOSE_SOCKET( m_Socket );

			DHLOG_ERROR( L"fail : socket bind.!" );
			return false;
		} // if

		if( ::listen( m_Socket, SOMAXCONN ) == SOCKET_ERROR )
		{
			::shutdown( m_Socket, SD_BOTH );
			::closesocket( m_Socket );
			m_Socket = INVALID_SOCKET;

			DHLOG_ERROR( L"fail : listen start.!" );
			return false;
		} // if

		return CDHThread::Start();
	}

	void CAccepter::End()
	{
		CDHThread::SetWorking( false );

		if( m_Socket != INVALID_SOCKET )
		{
			CLOSE_SOCKET( m_Socket );
		}

		CDHThread::End();
	}

    void CAccepter::SetINetCoreEvent( INetCoreEvent *v )
    {
        m_INetCoreEvent = v;
    }

}
