#include "stdafx.h"
#include "NetClient.h"
#include "DHNetServer.h"
#include "IRmiStub.h"
#include "IRmiProxy.h"
#include "sysutil.h"


namespace DHNet
{
	CNetClient::CNetClient()
	: CDHThread( L"NetClient" )
	{
		// socket init
		{
			WORD        wVer = MAKEWORD( 2, 2 );
			WSADATA     wsaData;

			if( ::WSAStartup( wVer, &wsaData ) != 0 )
			{
				::WSACleanup();
				return;
			}

			if( wsaData.wVersion != MAKEWORD( 2, 2 ) )
			{
				::WSACleanup();
				return;
			}
		}

		CSocketPtr spSocket( new CSocket( SUT_CLIENT ) );
		m_netSocket = spSocket;
		m_netSocket->SetINetCoreEvent( this );
	}


	CNetClient::~CNetClient()
	{
		Disconnect();
		::WSACleanup();
	}

	bool CNetClient::Loop()
	{
		LPOVERLAPPED lpOverlapped = NULL;
		DWORD dwBytesTransfer = 0;
        ULONG_PTR ulKey = 0;

		// I/O 의 결과를 얻어옵니다.
		BOOL bOK = ::GetQueuedCompletionStatus( m_netSocket->GetIOCP(), &dwBytesTransfer, &ulKey, &lpOverlapped, INFINITE );
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
					DHLOG_ERROR( L"Lan 장비 및 드라이버 문제로 소켓 종료 : error(%d)", iError );
				}
				break;
			default:
				{
					DHLOG_ERROR( L"ShutDown Socket : error(%d)", iError );
				}
				break;
			}
		}

		// 종료용 I/O Completion packet인지 확인
		if( ulKey == 0 && dwBytesTransfer == 0 && lpOverlapped == NULL )
		{
			if( m_netSocket->GetIOCP() != nullptr )
			{
				::CloseHandle( m_netSocket->GetIOCP() );
				m_netSocket->SetIOCP( nullptr );
			}

			DHLOG_INFO( L"Signal ShutDown IOCP.!" );
			return false;
		}

		auto socket = (CSocket*)ulKey;
		socket->OnIOCallback( (bOK ? true : false), dwBytesTransfer, lpOverlapped );
		socket->DecRefCount();	// 완료된 작업에 대한 스마트 포인터 감소.

		return true;
	}

	void CNetClient::RmiSend( HostID remote, RmiContext rmiContext, Int32 packetID, CMessage *msg )
	{
		if( m_netSocket == nullptr )
		{
			DHLOG_ERROR( L"m_netSocket == nullptr" );
			return;
		}

		m_netSocket->SendPacket( rmiContext, packetID, msg );
	}

	bool CNetClient::OnReceiveUserMessage( HostID remote, CMessage* msg )
	{
		for( auto rmi : m_vecStub )
		{
			bool ret = rmi->ProcessReceivedMessage( remote, msg );
			if( ret )
			{
				return true;;
			}
		}

		DHLOG_ERROR( L"function that a user did not create has been called." );

		return true;
	}

	void CNetClient::OnReceiveMessageProcess( HostID remote, CMessage* msg )
	{
		UInt32 index = ++m_indexWorkThreadQue;
		if( index == UINT_MAX )
		{
			m_indexWorkThreadQue = 1;
			index = 1;
		}
		index = index % m_vecThreadWork.size();
		m_vecThreadWork[index]->OnReceiveMessageProcess( remote, msg );
	}

	bool CNetClient::Connect( String hostName, int port, UInt32 _RecvThreadSize /* = 4 */ )
	{
		if( IsConnected() )
		{
			DHLOG_ERROR( L"already connected.!" );
			return false;
		}

		if( m_netSocket->GetIOCP() == nullptr )
		{
			auto iocp = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );
			m_netSocket->SetIOCP( iocp );
		}
		else
		{
			m_netSocket->Close();
		}

		bool ret = m_netSocket->Connect( StringW2A( hostName ).GetString(), port );

		if( ret == false )
		{
			DHLOG_ERROR( L"fail Connect( %s, %d )", hostName.GetString(), port );
			return ret;
		}
		
		m_netSocket->BindToIOCP();

		m_netSocket->WaitRecv();

		CDHThread::Start();

		if( m_vecThreadWork.size() == 0 )
		{
			for( UInt32 ui = 0; ui < _RecvThreadSize; ++ui )
			{
				auto iocp = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );

				CWorkThreadPtr workThread( new CWorkThread() );
				workThread->SetIndex( ui );
				workThread->SetIOCP( iocp );
				workThread->SetEventSink( this );
				workThread->Start();

				m_vecThreadWork.push_back( workThread );
			}
		}

		return true;
	}

	bool CNetClient::IsConnected()
	{
		return (m_netSocket->IsConnected() && m_netSocket->IsVerify);
	}

	void CNetClient::Disconnect()
	{
		// I/O 포트 종료 를 통해 소켓 종료까지 같이 처리한다.
		if( m_netSocket != nullptr )
		{
			m_netSocket->Disconnect();
		}

		for( auto &work : m_vecThreadWork )
		{
			work->End();
		}
		m_vecThreadWork.resize( 0 );
	}

	void CNetClient::AttachStub( IRmiStub *stub )
	{
		m_vecStub.push_back( stub );
	}

	void CNetClient::AttachProxy( IRmiProxy *proxy )
	{
		proxy->m_core = this;
		m_vecProxy.push_back( proxy );
	}

	void CNetClient::SocketClose( bool IsDisconectSuccess /*= false */ )
	{
		if( m_netSocket == nullptr )
			return;
		m_netSocket->Close( IsDisconectSuccess );
	}

	HostID CNetClient::GetHostID()
	{
		return m_netSocket->m_HostID;
	}
}
