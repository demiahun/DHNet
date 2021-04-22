#pragma once

#include "BasicType.h"
#include "DHPtr.h"
#include "DHIOObject.h"
#include "Message.h"
#include "DHString.h"
#include "DHLock.h"
#include "DHTime.h"
#include "DHEnums.h"

#include <queue>

/*
Packet 비동기 방식
	WorkThread(User) -> SendPacket(IncRefCount()) -> IOCP
		WorkThread 에서 SendPacket 이 호출 -> IOCP 를 통해 IOThread 로 넘어와
	IOThread -> IOCP Check -> OnIOCallback -> DoSend(WSASend, IncRefCount()) -> IOCP
		DoSend 를 통해 WSASend 가 호출되어 패킷을 전달하고
	IOThread -> IOCP Check -> OnIOCallback -> OnSendComplete
		Send 완료시 OnSendComplete(IOCP를 통해서) 가 호출된다.
*/
namespace DHNet
{
	class INetCoreEvent;

	enum SOCKET_USE_TYPE
	{
		SUT_NONE,
		SUT_SERVER,
		SUT_CLIENT,
	};

	class CSocket : public CIOObject, public RefObject
	{
		static std::atomic< INT64 > HostIDMaker;
        HostID  m_TempHostID = HostID_None;
	public:
		HostID	m_HostID = HostID_None;
		SOCKET_USE_TYPE
				SocketUseType = SUT_SERVER;
		std::atomic< bool >
                IsVerify = false;

	public:
		CSocket();
        CSocket( SOCKET_USE_TYPE _type );
		virtual ~CSocket();

		/*
		IOCP 를 통해 recv 이벤트 발생시 호출된다. 
			IOThread::Loop() 에서 호출
		*/
		virtual void OnIOCallback( bool success, DWORD transferred, LPOVERLAPPED lpoverlapped ) override;

	public:
		/*
		외부에서 Socket 을 받아와 사용시 호출하여 사용됨
		Ex.) Accept 등을 통해 접속 후 IO 만 사용되어야 할때
		*/
		void	SetConnectionInfo( SOCKET socket, SOCKADDR_IN &sockaddr );

		bool	Connect( const char *szIP, USHORT port );
		bool	IsConnected();
		/*
		접속 종료처리
		IOCP 를 통해 같은 쓰레드 내에서 Close() 이 호출되어 접속 종료 처리가 되도록 한다.
		this::Disconnect() -> IOCP -> IOThread -> this::OnIOCallback() -> this::Close()
		*/
		void	Disconnect();
		/*
		Socket 종료처리
		Close( true ); 로 호출되면 HeartBit 및 모든 동작이 멈추고 삭제된다.
		Close( false ); 호 호출되면 HeartBit 를 통해 1분간 재접속을 대기 했다가
			1분 이내 재접속 되면 Socket 갱신을 통한 재 정상동작
			1분 이내 재접속 못하면 삭제됨(OnIOCallback::HeartBit 를 통해 Close(true); 통해)
		*/
		void	Close( bool IsDisconectSuccess = false );
		/*
		HeartBit packet 을 주고 받기 시작
		접속이 완료 된 후 호출 되어야 동작함
		*/
		void	CallTickHeartBit( DWORD dwHeartBitTime = HEART_BIT_TIME );

		/*
		socket 을 IOCP 에 바인딩하여 send,recv 등의 이벤트를 받을수 있도록 함
		*/
		void	BindToIOCP();
		/*
		packet recv 대기 요청
			IOCP 에 recv 바인딩하여 IOCP 를 통해 packet 을 recv 받도록 하자
		*/
		bool	WaitRecv();

	public:
		String	GetIP();
		int		GetPort();

	public:
		void	SendPacket( RmiContext rmiContext, Int32 packetID, CMessage *msg );

		void	SwapToSendQue( std::vector< CMessage* > &vecMessage );

		void	SetINetCoreEvent( INetCoreEvent *v );

		CDHTime	GetLastHeartbitTime();

        size_t  GetSendQueueSize();

		void	DoSend();
	private:
		/*
		IOCP 를 통해 호출되어 IO 처리 하는 함수들
		*/		
		void	_DoSend();
		void	_OnSendComplete( DWORD transfered );
		void	_OnRecvComplete( DWORD transfered );

		void	_OnReceiveMessage( CMessage *msg );
		void	_OnReceiveMessageServer( CMessage *msg );
		void	_OnReceiveMessageClient( CMessage *msg );

		void	_FreeAllSendQue();

	private:
		enum OVERLAPPED_TYPE
		{
			OLT_SEND = 0,
			OLT_DoSend,
			OLT_RECV,
			OLT_TIMER,

			OLT_END,
		};
		OVERLAPPED
				m_overlapped[OLT_END];
		SOCKET	m_Socket = INVALID_SOCKET;
		SOCKADDR_IN         
				m_sockaddr;	// 소켓 주소 구조체.

		CriticalSection
				m_csSend;

		IOBuffer
				m_BufferRecv;
		std::queue< CMessage* >
				m_queSend;

		/*
		HeartBit 마지막 갱신시간
		*/
		CDHTime	m_tmHeartBitRefresh;
		bool	m_bEnableHeartBit = true;
        UInt64  m_HeartBitSendTick = 0;
        UInt64  m_HeartBitRecvTick = 0;

		/*
		TCP 패킷의 순서 번호
		: 이를 활용하여 중복패킷 콜백시 체크하여 접속을 끈어 버린다.
		*/
		Int32	m_SequenceNumSend = 0;
		Int32	m_SequenceNumRecv = 0;
		Int32	m_SequenceNumFailCount = 0;

        /*
        마지막으로 수신한 PacketID Keep
        Sequence Number 체크에서 로그 데이터 기록
        */
        Int32   m_LastRecvPacketID = 0;

		char	m_bufferZip[MAX_PACKET_SIZE] = {0,};

        /*
        클라이언트 용으로 사용되는 용도
        */
        INetCoreEvent*
                m_INetCoreEvent = nullptr;  // 메세지 핸들러

		NO_COPYABLE( CSocket );
	};

	REFOBJECTCOUNT( CSocket );
}

// SD_BOTH flag로 shutdown하는 경우, shutdown 없이 바로 closesocket 호출하는 것과 동일하다.
// 송/수신 시점에 있는 데이터를 손실할 가능성이 있다.
#define CLOSE_SOCKET( socket ) \
	if( socket != INVALID_SOCKET ) \
	{ \
	::shutdown( socket, SD_BOTH ); \
	::closesocket( socket ); \
	socket = INVALID_SOCKET; \
	} 

