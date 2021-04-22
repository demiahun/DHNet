#include "stdafx.h"
#include "DHSocket.h"
#include "DHLog.h"
#include "DHWorkThread.h"
#include "DHTimer.h"
#include "MessageMarshal.h"
#include "DHCrypto.h"
#include "sysutil.h"
#include "ZipHelper.h"

namespace DHNet
{
	std::atomic< INT64 > CSocket::HostIDMaker = 0;

	CSocket::CSocket()
	{
		++HostIDMaker;
		m_HostID = HostIDMaker;

		for( int i = OLT_SEND; i < OLT_END; ++i )
		{
			::memset( &m_overlapped[i], 0, sizeof( OVERLAPPED ) );
		}

		m_tmHeartBitRefresh = CDHTime::GetCurrTime();
	}

    CSocket::CSocket( SOCKET_USE_TYPE _type )
    {
        SocketUseType = _type;

        if( SocketUseType == SUT_SERVER )
        {
            ++HostIDMaker;
            m_HostID = HostIDMaker;
        }

        for( int i = OLT_SEND; i < OLT_END; ++i )
        {
            ::memset( &m_overlapped[i], 0, sizeof( OVERLAPPED ) );
        }

        m_tmHeartBitRefresh = CDHTime::GetCurrTime();
    }

    CSocket::~CSocket()
	{
		_FreeAllSendQue();

		DHLOG_INFO( L"Socket 소멸자 : HostID(%I64d)", m_HostID );
	}

	void CSocket::OnIOCallback( bool success, DWORD transferred, LPOVERLAPPED lpoverlapped )
	{
		if( success == false )
		{
			if( lpoverlapped == &m_overlapped[OLT_RECV] || lpoverlapped == &m_overlapped[OLT_DoSend] )
			{
				Close();
			}

			return;
		}

		if( lpoverlapped == &m_overlapped[OLT_TIMER] )
		{
			if( m_bEnableHeartBit == false )
				return;

			if( IsConnected() )
			{
				// HeartBit 처리
				auto msg = CMessage::Create();
                auto gapTick = m_HeartBitRecvTick - m_HeartBitSendTick;
                *msg << gapTick;
				SendPacket( RmiContext::Reliable, PACKETID_HEART_BIT, msg );
                m_HeartBitSendTick = ::GetTickCount64();
			}

			auto tmCurr = CDHTime::GetCurrTime();
			CDHTimeSpan tmSpan( 0, 0, 1, 0, 0 );

			// 재접속 대기 시작이 지나면 종료하자
			if( tmCurr > (m_tmHeartBitRefresh + tmSpan) )
			{
				Close( true );
				return;
			}

            // Close(false) 를 통한 Socket 종료가 되어도 1분간 Tick 을 돌며 재접속을 기다린다.
            CallTickHeartBit();

			return;
		}

		/*
		Socket close message
		서버 종료시에도 이곳에서 종료 처리가 되도록 한다.
		*/
		if( lpoverlapped == (LPOVERLAPPED)0 )
		{
			Close( true );
			return;
		}

		if( lpoverlapped == &m_overlapped[OLT_DoSend] )
		{
			_DoSend();
			return;
		}

		if( lpoverlapped == &m_overlapped[OLT_SEND] )
		{
			_OnSendComplete( transferred );
			return;
		}

		if( lpoverlapped == &m_overlapped[OLT_RECV] )
		{
			// 클라이언트 종료 호출
			if( transferred == 0 )
			{
				DHLOG_INFO( L"client disconnect call : HostID(%I64d)", m_HostID );
				Close( true );
			}
			else
			{
				_OnRecvComplete( transferred );
			}

			return;
		}

		DHLOG_ERROR( L"OVERLAPPED UNKNOWN MODE.!" );
	}

	void CSocket::SetConnectionInfo( SOCKET socket, SOCKADDR_IN &sockaddr )
	{
		m_Socket = socket;
		m_sockaddr = sockaddr;

		Int32 iZero = 0;
		::setsockopt( m_Socket, SOL_SOCKET, SO_RCVBUF, (char*)&iZero, sizeof( iZero ) );
		::setsockopt( m_Socket, SOL_SOCKET, SO_SNDBUF, (char*)&iZero, sizeof( iZero ) );
	}

	bool CSocket::Connect( const char *szIP, USHORT port )
	{
		if( IsConnected() == true )
		{
			DHLOG_ERROR( L"Allerady connected. HostID(%I64d)", m_HostID );
			return true;
		}

        DHLOG_INFO( L"Socket Connect : HostID(%I64d), szIP(%s), port(%d)", m_HostID, StringA2W(szIP).GetString(), port );

		int ret;

		/*
		overlapped I/O 를 통해 send,recv 를 받도록 생성함
		*/
		m_Socket = ::WSASocket( AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED );

		if( m_Socket == INVALID_SOCKET )
		{
			DHLOG_ERROR( L"fail WSASocket create : error(%d)", ::WSAGetLastError() );
			return false;
		}

		int sock_opt = 1;
		if( ::setsockopt( m_Socket, IPPROTO_TCP, TCP_NODELAY, (char*)&sock_opt, sizeof( sock_opt ) ) == SOCKET_ERROR )
		{
			DHLOG_ERROR( L"fail setsockopt : error(%d)", ::WSAGetLastError() );
			return false;
		}

		m_sockaddr.sin_family = AF_INET;
		if( szIP )
		{
			m_sockaddr.sin_addr.s_addr = ::inet_addr( szIP );
		}

		if( port != 0 )
		{
			m_sockaddr.sin_port = ::htons( port );
		}

        /////////////////////////////////
        // 소켓의 성능 최적화를 위한 세팅 
        Int32 iZero = 0;
        ::setsockopt( m_Socket, SOL_SOCKET, SO_RCVBUF, (char*)&iZero, sizeof( iZero ) );
        ::setsockopt( m_Socket, SOL_SOCKET, SO_SNDBUF, (char*)&iZero, sizeof( iZero ) );

		ret = ::connect( m_Socket, (struct sockaddr*)&m_sockaddr, sizeof( m_sockaddr ) );

		if( SOCKET_ERROR == ret )
		{
			// 간혹 다른 컴에서 WSAENOBUFS란 Error를 유발할 경우가 존재한다.
			// TCP의 buffer space가 모자랄 경우인 것 같다.
			// client가 100개 이면 뒤에 5개 정도가 이 error를 발생시킨다.

			DHLOG_ERROR( L"fail connect : error(%d) - %s", ::WSAGetLastError(), GetMsgWSALastError() );

			CLOSE_SOCKET( m_Socket );
			return false;
		}

		m_SequenceNumSend = 0;
		m_SequenceNumRecv = 0;
		m_SequenceNumFailCount = 0;

		return true;
	}

	bool CSocket::IsConnected()
	{
		return (m_Socket != INVALID_SOCKET);
	}

	void CSocket::Disconnect()
	{
		IncRefCount();
		CIOObject::PostIOCompletion( 0, (ULONG_PTR)this, 0 );
	}

	void CSocket::Close( bool IsDisconectSuccess  )
	{
		SOCKET hSocket = ::InterlockedExchange( &m_Socket, INVALID_SOCKET );

		if( hSocket != INVALID_SOCKET )
		{
			CLOSE_SOCKET( hSocket );
		}

        DHLOG_INFO( L"Socket Close : HostID(%I64d), IsDisconectSuccess(%s)", m_HostID, IsDisconectSuccess ? L"true" : L"false" );

		IsVerify = false;

        _FreeAllSendQue();

		if( IsDisconectSuccess == false )
		{
            if( m_INetCoreEvent != nullptr && SocketUseType == SUT_SERVER )
            {
                auto smsg = CMessage::Create();
                smsg->m_HostID = m_HostID;
                smsg->SetID( PACKETID_SYSTEM_OnClientOffline );
                smsg->WriteEnd();
                m_INetCoreEvent->OnReceiveMessageProcess( m_HostID, smsg );
            }
			return;
		}

        if( m_INetCoreEvent != nullptr && SocketUseType == SUT_SERVER )
        {
            m_INetCoreEvent->RemoveSocket( m_HostID );

            auto smsg = CMessage::Create();
            smsg->m_HostID = m_HostID;
            smsg->SetID( PACKETID_SYSTEM_OnClientLeave );
            *smsg << (int)ErrorType::ErrorType_Ok;
            smsg->WriteEnd();
            m_INetCoreEvent->OnReceiveMessageProcess( m_HostID, smsg );
        }

		m_bEnableHeartBit = false;
	}

	void CSocket::CallTickHeartBit( DWORD dwHeartBitTime )
	{
		IncRefCount();

		auto param = GTimerParamPool.Alloc();
		param->iocp = GetIOCP();
		param->transfereed = 0;
		param->completionKey = (ULONG_PTR)this;
		param->lpOverlapped = &m_overlapped[OLT_TIMER];
        param->m_Time = dwHeartBitTime;

		GTimer.AddParam( param );
	}

	void CSocket::BindToIOCP()
	{
		if( ::CreateIoCompletionPort( (HANDLE)m_Socket, GetIOCP(), (ULONG_PTR)this, 0 ) == NULL )
		{
			DHLOG_ERROR( L"fail bind IOCP : error(%d)", ::WSAGetLastError() );
		}
	}

	bool CSocket::WaitRecv()
	{
		if( m_Socket == INVALID_SOCKET )
		{
			return false;
		}

		IncRefCount();	// 패킷을 받을 동안 소켓이 사라지지 않도록 강제로 refcount 를 올려두자

		DWORD dwRead = 0;
		DWORD dwFlag = 0;

		int ret = 0;
		WSABUF kBuf;
		kBuf.buf = &m_BufferRecv.m_buffer[m_BufferRecv.m_Length];
		kBuf.len = MAX_PACKET_SIZE - m_BufferRecv.m_Length;

		ret = ::WSARecv( m_Socket,					// socket
						 &kBuf,						// buffer pointer, size
						 1,                         // lpBuffers 배열에 있는 WSABUF 구조체의 수.
						 &dwRead,                   // I/O 작업이 끝나자마자 보내진 바이트 수
						 &dwFlag,                   // [in,out] Option Flag
						 &m_overlapped[OLT_RECV],   // void* 식별자임...
						 NULL );                    // Recv 완료시 호출될 함수 포인터

		// If no error occurs and the receive operation has completed immediately, WSARecv returns zero.
		if( ret == 0 )
		{
			return true;
		}

		int iError = ::GetLastError();

		if( iError == WSA_IO_PENDING )
			return true;

		if( iError != WSAECONNRESET )		//  An existing connection was forcibly closed by the remote host.
		{
			DHLOG_ERROR( L"fail WSARecv : error(%d)", iError );
		}

		Close();
		DecRefCount();

		return false;
	}

	String CSocket::GetIP()
	{
		auto ip = ::inet_ntoa( m_sockaddr.sin_addr );
		return StringA2W( ip );
	}

	int CSocket::GetPort()
	{
		auto port = ntohs( m_sockaddr.sin_port );
		return port;
	}
	
	void CSocket::SendPacket( RmiContext rmiContext, Int32 packetID, CMessage* msg )
	{
		msg->SetRmiContext( rmiContext );
		msg->SetID( packetID );
		msg->WriteEnd();

        size_t queSize = 0;
        {
            CSLocker lock( m_csSend );

            m_queSend.push( msg );

            queSize = m_queSend.size();
        }

		if( IsConnected() == false )
        {
            if( SocketUseType == SUT_CLIENT )
            {
                _FreeAllSendQue();
            }
            return;
        }

		// 1개이면 이전에 보내지고 있는게 없으니 IOThread 에서 보내라고 IOCP 알리자
		if( queSize == 1 )
		{
			IncRefCount();	// 보내지는 동안 Socket 삭제가 이루지 지지 않도록 RefCount 를 올리고 IOThread 에서 받으면 DecRefCount() 를 호출해줌

			CIOObject::PostIOCompletion( 0, (ULONG_PTR)this, &m_overlapped[OLT_DoSend] );
		}
	}

	void CSocket::SwapToSendQue( std::vector< CMessage* > &vecMessage )
	{
		CSLocker lock( m_csSend );

        size_t uiCount = m_queSend.size();
		for( size_t ui = 0; ui < uiCount; ++ui )
		{
			vecMessage.push_back( m_queSend.front() );
			m_queSend.pop();
		}
	}

	void CSocket::SetINetCoreEvent( INetCoreEvent *v )
	{
		m_INetCoreEvent = v;
	}

	DHNet::CDHTime CSocket::GetLastHeartbitTime()
	{
		return m_tmHeartBitRefresh;
	}

    size_t CSocket::GetSendQueueSize()
    {
        size_t size = 0;
        {
            CSLocker lock( m_csSend );
            size = m_queSend.size();
        }        

        return size;
    }

	void CSocket::DoSend()
	{
		_DoSend();
	}

	void CSocket::_DoSend()
	{
		if( IsConnected() == false )
		{
            int queueSize = (int)GetSendQueueSize();
			DHLOG_ERROR( L"m_Socket == INVALID_SOCKET : HostID(%I64d), queueSize(%d)", m_HostID, queueSize );

            if( 1 == queueSize )
            {
                DHLOG_ERROR( L"IsConnected false and 1 queSize m_Socket == INVALID_SOCKET : HostID(%I64d)", m_HostID );
            }
            _FreeAllSendQue();
			return;
		}

		CMessage* msgSend = nullptr;
		{
			CSLocker lock( m_csSend );	// <- Lock
			if( m_queSend.size() > 0 )
			{
				msgSend = m_queSend.front();
			}
		}

		if( msgSend == nullptr )
		{
			DHLOG_ERROR( L"msgSend == nullptr : HostID(%I64d)", m_HostID );
			return;
		}

		switch( msgSend->GetRmiContext() )
		{
		case RmiContext::ReliableCompress:
			{
				CompressToMessage( msgSend, (Byte*)m_bufferZip );
			}
			break;
		case RmiContext::FastEncryp:
			{
				XOREncrypt( msgSend->m_buffer + HEADSIZE, msgSend->m_Length - HEADSIZE );
			}
			break;
		case RmiContext::FastEncrypCompress:
			{
				XOREncrypt( msgSend->m_buffer + HEADSIZE, msgSend->m_Length - HEADSIZE );

				CompressToMessage( msgSend, (Byte*)m_bufferZip );
			}
			break;
		}

		++m_SequenceNumSend;
		msgSend->SetSequenceNum( m_SequenceNumSend );

		/*
		WSASend() 를 통해 send 가 완료되어 OnSendComplete 가 호출될때까지 살아 있도록 RefCount 를 올린다.
		*/
		IncRefCount();

		int ret = 0;
		{
			DWORD dwWrite = 0;
			WSABUF kBuf;
			kBuf.len = msgSend->m_Length;
			kBuf.buf = msgSend->m_buffer;

			ret = ::WSASend( m_Socket,
							 &kBuf,						// 버퍼 포인터
							 1,							// WSABUF의 갯수
							 &dwWrite,					// [out] 즉시 전송된 량
							 0,							// Flag
							 &m_overlapped[OLT_SEND],	// OVERLAPPED
							 NULL );
		}

		int iError = ::WSAGetLastError();

		if( ret == SOCKET_ERROR && iError != ERROR_IO_PENDING )    // 현재 pending 중..
		{
			if( iError != WSAECONNRESET )		// 원격에서 끊어진 경우가 아닌것. 특별하게 취급한다.
			{
				if( WSAECONNABORTED == iError )
				{
					DHLOG_ERROR( L"WSASend Failed - 타임아웃이나 다른 에러 상황으로 인해 연결이 중지됨" );
				}
				else
				{
					DHLOG_ERROR( L"WSASend Failed. LastError(%d)", iError );
				}
			}

			// send 실패했으니, 소켓을 닫아주자.
			Close();			

			// Error Code - 10054 (연결이 원격 호스트에 의해 재 설정된 경우)
			if( WSAECONNRESET == iError )
			{
				DHLOG_ERROR( L"!Send 실패 Socket 종료 : 연결이 원격 호스트에 의해 재 설정되었습니다." );
			}
			else
			{
				DHLOG_ERROR( L"Send 실패 Socket 종료 LastError(%d)", iError );
			}

			DecRefCount();
		}
	}

	void CSocket::_OnSendComplete( DWORD transfered )
	{
		CMessage *msg = nullptr;
		bool isRelaySend = false;
		{
			CSLocker lock( m_csSend );
			if( m_queSend.size() > 0 )
			{
				msg = m_queSend.front();
				m_queSend.pop();
			}
			if( m_queSend.size() > 0 )
			{
				isRelaySend = true;
			}
		}

		if( msg == nullptr )
		{
			DHLOG_INFO( L"Send 완료된 패킷이 없음.! HostID(%I64d)", m_HostID );
			return;
		}

		GMessagePool.Free( msg );

		// 보낸 packet size 같은가.?
		if( msg->m_Length != transfered )
		{
			//Close( true );
			DHLOG_ERROR( L"msg->m_Length(%d) != transfered(%d) : iError(%d)", msg->m_Length, transfered, ::GetLastError() );
			return;
		}

		// 더 보내야 할 Packet 이 있으면 Send 계속 진행
		if( isRelaySend )
		{
			_DoSend();
		}
	}

	void CSocket::_OnRecvComplete( DWORD transfered )
	{
		m_BufferRecv.m_Length += transfered;

		// 패킷을 전달 할 인터페이스
		INetCoreEvent *pNetEvent = nullptr;
		

		/*
		패킷이 나누어져서 오거나
		2개 이상의 패킷이 하나로 올수 있어서 체크한다(네이글 알골리즘)
		*/
		Int32 packetLen = 0;
		while( m_BufferRecv.m_Length >= HEADSIZE )
		{
			// 패킷 사이즈
			::memcpy_s( &packetLen, sizeof( packetLen ), m_BufferRecv.m_buffer, sizeof( packetLen ) );

			// 버퍼에 받아진 데이터로 완저한 packet 이 가능한 경우 WorkThread 로 전달
			if( m_BufferRecv.m_Length >= packetLen )
			{
				if( packetLen < HEADSIZE )
				{
					DHLOG_ERROR( L"=== Invalid packetLen to Socket Close : HostID(%I64d), packetLen(%d)", m_HostID, packetLen );
					Disconnect();
					return;
				}

				// CMessage 를 이용한 WorkdThread 전달
				auto msgSend = CMessage::Create();

				// HostID 를 같이 전달하여 해당 유저에게 패킷을 전달하자 
				msgSend->m_HostID = m_HostID;

				// PacketData 를 WorkThread 로 전달
				::memcpy_s( msgSend->m_buffer, MAX_PACKET_SIZE, m_BufferRecv.m_buffer, packetLen );

				// 읽어들인 크기를 빼자
				m_BufferRecv.m_Length -= packetLen;

				switch( msgSend->GetRmiContext() )
				{
				case RmiContext::ReliableCompress:
					{
						UncompressToMessage( msgSend, (Byte*)m_bufferZip );
					}
					break;
				case RmiContext::FastEncryp:
					{
						XORDecrypt( msgSend->m_buffer + HEADSIZE, packetLen - HEADSIZE );
					}
					break;
				case RmiContext::FastEncrypCompress:
					{
						UncompressToMessage( msgSend, (Byte*)m_bufferZip );

						XORDecrypt( msgSend->m_buffer + HEADSIZE, msgSend->GetLength() - HEADSIZE );
					}
					break;
				}

                Int32 msgID = msgSend->GetID();

				// Sequence Number 체크
				if( msgSend->GetSequenceNum() > m_SequenceNumRecv )
				{
					m_SequenceNumRecv = msgSend->GetSequenceNum();

					m_SequenceNumFailCount = 0;
                    m_LastRecvPacketID = msgID;
				}
				else
				{
					DHLOG_ERROR( L"SequenceNum check fail : HostID(%I64d), failCount(%d), LastRecvPacketID(%d), NowRecvPacketID(%d)", m_HostID, m_SequenceNumFailCount, m_LastRecvPacketID, msgID );

                    m_LastRecvPacketID = msgID;

					++m_SequenceNumFailCount;
					if( m_SequenceNumFailCount >= 10 )
					{
						// 같은 패킷이 10개 이상 오면 접속을 끈어 버리자
						DHLOG_ERROR( L"SequenceNum fail Disconnect : HostID(%I64d), failCount(%d)", m_HostID, m_SequenceNumFailCount );
						Disconnect();
						return;
					}
				}

				if( msgSend->GetID() < 0 )
				{
					_OnReceiveMessage( msgSend );

					// 전달한 부분을 빼고 데이터가 남았다면 앞부분으로 당김
					if( m_BufferRecv.m_Length > 0 )
					{
						::memmove( m_BufferRecv.m_buffer, m_BufferRecv.m_buffer + packetLen, m_BufferRecv.m_Length );
					}

					continue;
				}

				// 유저 패킷 전달
				{
                    m_INetCoreEvent->OnReceiveMessageProcess( m_HostID, msgSend );
				}

				// 전달한 부분을 빼고 데이터가 남았다면 앞부분으로 당김
				if( m_BufferRecv.m_Length > 0 )
				{
					::memmove( m_BufferRecv.m_buffer, m_BufferRecv.m_buffer + packetLen, m_BufferRecv.m_Length );
				}
			}
			else
			{
				// 온전한 패킷을 구성하기에 충분한 데이터가 없음. loop를 빠져나간다.
				break;
			}
		}

		WaitRecv();
	}

	void CSocket::_OnReceiveMessage( CMessage *msg )
	{
		switch( SocketUseType )
		{
		case SUT_SERVER:
			_OnReceiveMessageServer( msg );
			break;
		case SUT_CLIENT:
			_OnReceiveMessageClient( msg );
			break;
		}

		GMessagePool.Free( msg );
	} 

	void CSocket::_OnReceiveMessageServer( CMessage *msg )
	{
		switch( msg->GetID() )
		{
		case PACKETID_HEART_BIT:
			{
				m_tmHeartBitRefresh = CDHTime::GetCurrTime();
                m_HeartBitRecvTick = ::GetTickCount64();
                auto gapTick = m_HeartBitRecvTick - m_HeartBitSendTick;
                // HeartBit 3초 이상에 대하여 로그 추가하도록 수정.( 정상적으로 플레이 가능한 상황에서도 1000 ms이상은 한번씩 나온다.
                if( gapTick >= 3000 )
                {
                    auto smsg = CMessage::Create();
                    smsg->m_HostID = m_HostID;
                    smsg->SetID( PACKETID_SYSTEM_OnDelayHeartbit );
                    *smsg << gapTick;
                    smsg->WriteEnd();
                    m_INetCoreEvent->OnReceiveMessageProcess( m_HostID, smsg );
                }
			}
			break;
		case PACKETID_CS_HOSTID_RECV:
			{
				IsVerify = true;

				// 클라 정상 접속 하여 WorkThread 에 접속 사실을 알리자
                if( m_INetCoreEvent != nullptr )
                {
                    auto smsg = CMessage::Create();
                    smsg->m_HostID = m_HostID;
                    smsg->SetID( PACKETID_SYSTEM_OnClientJoin );
                    *smsg << GetIP();
                    smsg->WriteEnd();
                    m_INetCoreEvent->OnReceiveMessageProcess( m_HostID, smsg );
                }
				DHLOG_INFO( L"recv PACKETID_CS_HOSTID_RECV : HostID(%I64d), IP(%s)", m_HostID, GetIP().GetString() );

				{
					// 클라에게 통신준비 완료 됐다고 알리자
					auto msg = CMessage::Create();
					SendPacket( RmiContext::Reliable, PACKETID_SC_CONNECT_SUCCESS, msg );
				}
			}
			break;
		case PACKETID_CS_HOSTID_RECONNECT:
			{
				// 재접속 Socket 교체
				HostID ownerID;
				*msg >> ownerID;

                if( m_INetCoreEvent == nullptr )
                {
                    DHLOG_ERROR( L"m_INetCoreEvent == nullptr : remote(%I64d)", m_HostID );
                    break;
                }

                auto spOldSocket = m_INetCoreEvent->GetSocket( ownerID );
                if( spOldSocket == nullptr )
                {
                	// 클라에 리코넥션 머지 작업이 실패했다고 알리고 클라에서 접속을 종료 하도록 하자
                	auto msgSend = CMessage::Create();
                	SendPacket( RmiContext::Reliable, PACKETID_SC_RECONNECT_FAIL, msgSend );

                	DHLOG_WARN( L"재접속 할 Socket 이 없음 m_HostID(%I64d), ownerID(%I64d)", m_HostID, ownerID );
                	break;
                }

                m_INetCoreEvent->RemoveSocket( ownerID );
                m_INetCoreEvent->RemoveSocket( m_HostID );

                spOldSocket->m_HostID = m_HostID;
                m_HostID = ownerID;

                m_INetCoreEvent->AddSocket( spOldSocket );
                m_INetCoreEvent->AddSocket( this );

                // 머지 작업을 하자 HostID, sendPacket

                std::vector< CMessage* > vecMessage;
                spOldSocket->SwapToSendQue( vecMessage );

                IsVerify = true;

                // 클라에게 머지 성공했다고 알리자
                auto msgSend = CMessage::Create();
                SendPacket( RmiContext::Reliable, PACKETID_SC_RECONNECT_SUCCESS, msgSend );

                for( auto vit : vecMessage )
                {
                	SendPacket( (RmiContext)vit->GetRmiContext(), vit->GetID(), vit );
                }

                spOldSocket->Disconnect();

                auto smsg = CMessage::Create();
                smsg->m_HostID = m_HostID;
                smsg->SetID( PACKETID_SYSTEM_OnClientOnline );
                smsg->WriteEnd();
                m_INetCoreEvent->OnReceiveMessageProcess( m_HostID, smsg );
			}
			break;
		}
	}

	void CSocket::_OnReceiveMessageClient( CMessage *msg )
	{
		switch( msg->GetID() )
		{
		case PACKETID_HEART_BIT:
			{
				auto msgSend = CMessage::Create();
				msg->CopyTo( msgSend );
				SendPacket( RmiContext::Reliable, PACKETID_HEART_BIT, msgSend );
				m_HeartBitSendTick = ::GetTickCount64();
			}
			break;
		case PACKETID_SC_HOSTID_INFO:
			{
				*msg >> m_HostID;
				UInt32 xorKey0, xorKey1, xorKey2;
				*msg >> xorKey0;
				*msg >> xorKey1;
				*msg >> xorKey2;

				DHLOG_INFO( L"HostID(%I64d), xor0(%d), xor1(%d), xor2(%d)", m_HostID, xorKey0, xorKey1, xorKey2 );

				auto msgSend = CMessage::Create();
				SendPacket( RmiContext::Reliable, PACKETID_CS_HOSTID_RECV, msgSend );
			}
			break;
		case PACKETID_SC_CONNECT_SUCCESS:
			{
				IsVerify = true;
			}
			break;
		default:
			{
				DHLOG_ERROR( L"구현 예정 중.!" );
			}
			break;
		}
	}

	void CSocket::_FreeAllSendQue()
	{
        std::queue< CMessage* > qValue;
        {
            CSLocker lock( m_csSend );
            m_queSend.swap( qValue );
        }

        GMessagePool.Free( qValue );
	}

}