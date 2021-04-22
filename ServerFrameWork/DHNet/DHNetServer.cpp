#include "stdafx.h"
#include "DHNetServer.h"
#include "DHLock.h"
#include "DHLog.h"
#include "sysutil.h"
#include "DHTimer.h"
#include "IRmiStub.h"
#include "IRmiProxy.h"
#include "DHUtil.h"
#include "DHCrypto.h"
#include "MessageMarshal.h"

namespace DHNet
{
	CDHNetServer *GDHNetServer = nullptr;

	CDHNetServer::CDHNetServer()
	{
		GDHNetServer = this;

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
	}

	CDHNetServer::~CDHNetServer()
	{
		::WSACleanup();
	}

	void CDHNetServer::Start( CStartServerParameter &param, ErrorType &outError )
	{
		m_StartParameter = param;

		if( m_pNetServerEvent == nullptr )
		{
			outError = ErrorType_InvalidCallbackFunc;
			return;
		}

        // core 갯수는 지정하지 않으면 실제 cpu 갯수만큼만 사용하도록 수정.
		UINT coreCount = GetCoreNum();

		// Listen 시작
		String hostName;
		if( m_StartParameter.m_HostName.GetLength() == 0 )
		{
			m_StartParameter.m_HostName = DHNet::GetLocalIP();
			hostName = m_StartParameter.m_HostName;
		}
		if( m_StartParameter.m_HostName.GetLength() > 0 )
		{
			/*
			domain 지원.?
			DHNet::GetIPsbyHostname( L"www.google.com", OUT std::vector< String > vecIPs ) 로 지원할수 있음
			*/
			switch( m_StartParameter.m_HostName[0] )
			{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				{
					hostName = m_StartParameter.m_HostName;
				}
				break;
			default:
				{
					std::vector< String > vecIPs;
					DHNet::GetIPsbyHostname( m_StartParameter.m_HostName, vecIPs );
					if( vecIPs.size() == 0 )
					{
						outError = ErrorType_StartFailListen;
						return;
					}
					hostName = vecIPs[0];
				}
				break;
			}
		}
        m_Accepter.SetINetCoreEvent( this );
		if( m_Accepter.Start( hostName.GetLength() > 0 ? hostName.GetString() : nullptr, m_StartParameter.m_Port ) == false )
		{
			outError = ErrorType_StartFailListen;

			m_vecThreadIO.clear();
			m_vecThreadWork.clear();

			::WSACleanup();

			return;
		}

		// IOThread 준비
		UINT CountIOThread = param.m_ThreadCountIO == 0 ? coreCount : param.m_ThreadCountIO;
        DHLOG_FORCE_INFO( L"IOThread CountIOThread : %d", CountIOThread );
		for( UINT ui = 0; ui < CountIOThread; ++ui )
		{
			auto iocp = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );

			if( iocp == nullptr )
			{
				DHLOG_ERROR( L"Failed Create IOThread IOCP : %d", ui );
				continue;
			}

			CIOThreadPtr ioThread( new CIOThread() );
			ioThread->SetIndex( ui );
			ioThread->SetIOCP( iocp );

			m_vecThreadIO.push_back( ioThread );
		}
		
		// WorkThread 준비
		UINT CountWordThread = param.m_ThreadCountWork == 0 ? coreCount : param.m_ThreadCountWork;
        DHLOG_FORCE_INFO( L"WorkThread CountWordThread : %d", CountWordThread );
		for( UINT ui = 0; ui < CountWordThread; ++ui )
		{
			auto iocp = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );

			if( iocp == nullptr )
			{
				DHLOG_ERROR( L"Failed Create WorkThread IOCP : %d", ui );
				continue;
			}

			CWorkThreadPtr workThread( new CWorkThread() );
			workThread->SetIndex( ui );
			workThread->SetIOCP( iocp );
			workThread->SetEventSink( this );

			m_vecThreadWork.push_back( workThread );
		}

		// Timer 준비
		{
			auto iocp = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );
			GTimer.SetIOCP( iocp );
		}

		// Timer 시작
		GTimer.Start( 1 );

		// IO,Work thread 시작
		for( UINT ui = 0; ui < CountIOThread; ++ui )
		{
			m_vecThreadIO[ui]->Start();
		}
		::Sleep( 100 );
		for( UINT ui = 0; ui < CountWordThread; ++ui )
		{
			m_vecThreadWork[ui]->Start();
		}
		::Sleep( 100 );

		// OnTick 설정이 있다면 Thread 를 활성화 하자
		if( m_StartParameter.m_OnTickCallbackIntervalMs > 0 )
		{
			m_TickThread = CTickThreadPtr( new CTickThread( m_pNetServerEvent, m_StartParameter.m_OnTickCallbackIntervalMs ) );

			m_TickThread->Start();
		}

		m_IsStart = true;
	}

	void CDHNetServer::End()
	{
		if( m_IsStart == false )
			return;

		// Socket Accept 종료
		m_Accepter.End();

		// Socket 을 모두 종료 처리 하자
		{
			SReadLocker lock( m_srwSocket );

			auto pos = m_mapSocket.GetStartPosition();
			while( pos )
			{
				auto mit = m_mapSocket.GetNext( pos );
				mit->m_value->Disconnect();
			}

			DHLOG_INFO( L"Socket disconnect start : size(%d)", m_mapSocket.GetCount() );
		}
		/*
		모든 소켓이 종료처리 되면 m_mapSocket 은 사이즈가 0이 된다.
		Socket::Disconnect() -> IOCP -> Socket::OnIOCallback() -> Socket::Close() -> this::RemoveSocket
		*/
		{
			while( true )
			{
				::Sleep( 100 );

				SReadLocker lock( m_srwSocket );

				if( m_mapSocket.GetCount() == 0 )
				{
					DHLOG_INFO( L"Socket disconnect end.!" );
					break;
				}
			}
		}

		::Sleep( 1000 );

		if( m_TickThread != nullptr )
		{
			m_TickThread->End();
		}

		for( auto vit : m_vecThreadIO )
		{
			vit->End();
		}
		for( auto vit : m_vecThreadWork )
		{
			vit->End();
		}

		GTimer.End();

		// IOCP thread 의 정상 종료 상태를 1분간 기다려 보자
		while( true )
		{
			::Sleep( 100 );

			bool IsAllEnd = true;
			for( auto vit : m_vecThreadIO )
			{
				if( vit->IsWorking() == false )
					continue;

				IsAllEnd = false;
				break;
			}

			if( IsAllEnd == false )
				continue;

			for( auto vit : m_vecThreadWork )
			{
				if( vit->IsWorking() == false )
					continue;

				IsAllEnd = false;
				break;
			}

			if( IsAllEnd == false )
				continue;

			if( GTimer.IsWorking() )
				continue;

			break;
		}
	}

	void CDHNetServer::SetEventSink( INetServerEvent *eventSink )
	{
		if( eventSink == nullptr )
			return;

		m_pNetServerEvent = eventSink;
	}

	void CDHNetServer::AttachStub( IRmiStub *stub )
	{
		if( stub == nullptr )
			return;

		m_vecStub.push_back( stub );
	}

	void CDHNetServer::AttachProxy( IRmiProxy *proxy )
	{
		if( proxy == nullptr )
			return;

		proxy->m_core = this;
		m_vecProxy.push_back( proxy );
	}

    void CDHNetServer::OnReceiveMessageProcess( HostID remote, CMessage* msg )
    {
        auto workThread = GetWorkThreadQueue();
        workThread->OnReceiveMessageProcess( remote, msg );
    }

    bool CDHNetServer::OnReceiveUserMessage( HostID remote, CMessage* msg )
	{
        if( msg->GetID() < 0 )
        {
            switch( msg->GetID() )
            {
            case PACKETID_SYSTEM_OnClientJoin:
                {
                    String ip;
                    *msg >> ip;
                    m_pNetServerEvent->OnClientJoin( remote, ip );
                    return true;
                }
                break;
            case PACKETID_SYSTEM_OnClientLeave:
                {
                    int Error = 0;
                    *msg >> Error;
                    m_pNetServerEvent->OnClientLeave( remote, (ErrorType)Error );
                    return true;
                }
                break;
            case PACKETID_SYSTEM_OnClientOnline:
                {
                    m_pNetServerEvent->OnClientOnline( remote );
                    return true;
                }
                break;
            case PACKETID_SYSTEM_OnClientOffline:
                {
                    m_pNetServerEvent->OnClientOffline( remote );
                    return true;
                }
                break;
            case PACKETID_SYSTEM_OnDelayHeartbit:
                {
                    UInt64 tick = 0;
                    *msg >> tick;
                    m_pNetServerEvent->OnDelayHeartbit( remote, (DWORD)tick );
                    return true;
                }
                break;
            default:
                {
                    DHLOG_ERROR( L"function that a user did not create has been called(???). : ID(%d)", msg->GetID() );
                }
                return false;
            }
        }

		for( auto rmi : m_vecStub )
		{
			bool ret = rmi->ProcessReceivedMessage( remote, msg );
			if( ret )
				return true;;
		}

		if( m_pNetServerEvent->OnReceiveUserMessage( remote, msg ) )
			return true;

		DHLOG_ERROR( L"function that a user did not create has been called. : ID(%d)", msg->GetID() );

		return false;
	}

    void CDHNetServer::AddSocket( CSocketPtr socket )
    {
		{
			SWriteLocker rwlock(m_srwSocket);

			if (m_mapSocket.Lookup(socket->m_HostID) != nullptr)
			{
				DHLOG_ERROR(L"fail add socket(Has socket) : HostID(%I64d)", socket->m_HostID);
				return;
			}

			m_mapSocket[socket->m_HostID] = socket;

			DHLOG_INFO(L"AddSocket : HostID(%I64d)", socket->m_HostID);
		}

        socket->SocketUseType = SUT_SERVER;
        socket->SetINetCoreEvent( this );
        socket->SetIOCP( GetIOCPHandleByIOThread( socket->m_HostID ) );
        socket->BindToIOCP();
        socket->WaitRecv();
        socket->CallTickHeartBit( 0 );

        auto msg = CMessage::Create();
        *msg << socket->m_HostID;
        *msg << XOR_KEY0;
        *msg << XOR_KEY1;
        *msg << XOR_KEY2;
        socket->SendPacket( RmiContext::Reliable, PACKETID_SC_HOSTID_INFO, msg );
    }

    void CDHNetServer::RemoveSocket( HostID remote )
    {
        if( remote == -1 )
            return;

        SWriteLocker rwlock( m_srwSocket );

        auto mit = m_mapSocket.Lookup( remote );
        if( mit == nullptr )
        {
            DHLOG_ERROR( L"fail find socket : HostID(%I64d)", remote );
            return;
        }

        m_mapSocket.RemoveKey( remote );

        DHLOG_INFO( L"RemoveSocket : HostID(%I64d)", remote );
    }

    DHNet::CSocketPtr CDHNetServer::GetSocket( HostID remote )
    {
        CSocketPtr spSocket = nullptr;
        {
            SReadLocker rwLock( m_srwSocket );

            auto mit = m_mapSocket.Lookup( remote );
            if( mit == nullptr )
            {
                return nullptr;
            }

            spSocket = mit->m_value;
        }

        return spSocket;
    }

	DHNet::String CDHNetServer::GetHostName()
	{
		return m_StartParameter.m_HostName;
	}

	int CDHNetServer::GetPort()
	{
		return m_StartParameter.m_Port;
	}

	//void CDHNetServer::SwapSocket( IN HostID newHost, IN HostID oldHost )
	//{
	//	SWriteLocker rwlock( m_srwSocket );

	//	auto mitNew = m_mapSocket.Lookup( newHost );
	//	auto mitOld = m_mapSocket.Lookup( oldHost );

	//	if( mitNew == nullptr || mitOld == nullptr )
	//	{
	//		DHLOG_ERROR( L"fail Find new(%I64d) or old(%I64d) socket", newHost, oldHost );
	//		return;
	//	}

	//	auto spNewSocket = mitNew->m_value;
	//	auto spOldSocket = mitOld->m_value;
	//	m_mapSocket[oldHost] = spNewSocket;
	//	m_mapSocket[newHost] = spOldSocket;
	//	spNewSocket->m_HostID = oldHost;
	//	spOldSocket->m_HostID = newHost;

	//	DHLOG_INFO( L"Success Swap socket : newHost(%I64d), oldHost(%I64d)", newHost, oldHost );
	//}

    void CDHNetServer::GetSockets( OUT std::vector< CSocketPtr > &vecSockets )
    {
        SReadLocker rwLock( m_srwSocket );

        auto pos = m_mapSocket.GetStartPosition();
        while( pos )
        {
            auto mit = m_mapSocket.GetNext( pos );
            vecSockets.push_back( mit->m_value );
        }
    }

    Int32 CDHNetServer::GetSocketCount()
	{
		Int32 count = 0;
		{
			SReadLocker rwLock( m_srwSocket );

			count = (int)m_mapSocket.GetCount();
		}		

		return count;
	}

	void CDHNetServer::RmiSend( HostID remote, RmiContext rmiContext, Int32 packetID, CMessage *msg )
	{
        CSocketPtr socket = nullptr;
        {
            SReadLocker rwlock( m_srwSocket );

            auto mit = m_mapSocket.Lookup( remote );
            if( mit == nullptr )
            {
                DHLOG_INFO( L"fail find socket : HostID(%I64d)", remote );
            }
            else
            {
                socket = mit->m_value;
            }
        }

        if( socket == nullptr && msg != nullptr )
        {
            GMessagePool.Free( msg );
            return;
        }

		socket->SendPacket( rmiContext, packetID, msg );
	}

	HANDLE CDHNetServer::GetIOCPHandleByIOThread( HostID remote )
	{
		auto index = (UINT)(remote % m_vecThreadIO.size());
		return m_vecThreadIO[index]->GetIOCP();
	}

	DHNet::CWorkThreadPtr CDHNetServer::GetWorkThread( UINT indexCount )
	{
		auto index = indexCount % m_vecThreadWork.size();
		return m_vecThreadWork[index];
	}

	DHNet::CWorkThreadPtr CDHNetServer::GetWorkThreadQueue()
	{
		UInt32 index = ++m_indexWorkThreadQue;
		if( index == UINT_MAX )
		{
			m_indexWorkThreadQue = 1;
			index = 1;
		}
		index = index % m_vecThreadWork.size();
		return m_vecThreadWork[index];
	}

    void CDHNetServer::GetIOThreadRunCount( OUT std::vector< UInt64 > &vecCount )
    {
        for( auto &thread : m_vecThreadIO )
        {
            vecCount.push_back( thread->GetRunCount() );
        }
    }

    void CDHNetServer::GetWorkThreadRunCount( OUT std::vector< UInt64 > &vecCount )
    {
        for( auto &thread : m_vecThreadWork )
        {
            vecCount.push_back( thread->GetRunCount() );
        }
    }

}