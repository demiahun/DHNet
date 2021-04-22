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
Packet �񵿱� ���
	WorkThread(User) -> SendPacket(IncRefCount()) -> IOCP
		WorkThread ���� SendPacket �� ȣ�� -> IOCP �� ���� IOThread �� �Ѿ��
	IOThread -> IOCP Check -> OnIOCallback -> DoSend(WSASend, IncRefCount()) -> IOCP
		DoSend �� ���� WSASend �� ȣ��Ǿ� ��Ŷ�� �����ϰ�
	IOThread -> IOCP Check -> OnIOCallback -> OnSendComplete
		Send �Ϸ�� OnSendComplete(IOCP�� ���ؼ�) �� ȣ��ȴ�.
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
		IOCP �� ���� recv �̺�Ʈ �߻��� ȣ��ȴ�. 
			IOThread::Loop() ���� ȣ��
		*/
		virtual void OnIOCallback( bool success, DWORD transferred, LPOVERLAPPED lpoverlapped ) override;

	public:
		/*
		�ܺο��� Socket �� �޾ƿ� ���� ȣ���Ͽ� ����
		Ex.) Accept ���� ���� ���� �� IO �� ���Ǿ�� �Ҷ�
		*/
		void	SetConnectionInfo( SOCKET socket, SOCKADDR_IN &sockaddr );

		bool	Connect( const char *szIP, USHORT port );
		bool	IsConnected();
		/*
		���� ����ó��
		IOCP �� ���� ���� ������ ������ Close() �� ȣ��Ǿ� ���� ���� ó���� �ǵ��� �Ѵ�.
		this::Disconnect() -> IOCP -> IOThread -> this::OnIOCallback() -> this::Close()
		*/
		void	Disconnect();
		/*
		Socket ����ó��
		Close( true ); �� ȣ��Ǹ� HeartBit �� ��� ������ ���߰� �����ȴ�.
		Close( false ); ȣ ȣ��Ǹ� HeartBit �� ���� 1�а� �������� ��� �ߴٰ�
			1�� �̳� ������ �Ǹ� Socket ������ ���� �� ������
			1�� �̳� ������ ���ϸ� ������(OnIOCallback::HeartBit �� ���� Close(true); ����)
		*/
		void	Close( bool IsDisconectSuccess = false );
		/*
		HeartBit packet �� �ְ� �ޱ� ����
		������ �Ϸ� �� �� ȣ�� �Ǿ�� ������
		*/
		void	CallTickHeartBit( DWORD dwHeartBitTime = HEART_BIT_TIME );

		/*
		socket �� IOCP �� ���ε��Ͽ� send,recv ���� �̺�Ʈ�� ������ �ֵ��� ��
		*/
		void	BindToIOCP();
		/*
		packet recv ��� ��û
			IOCP �� recv ���ε��Ͽ� IOCP �� ���� packet �� recv �޵��� ����
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
		IOCP �� ���� ȣ��Ǿ� IO ó�� �ϴ� �Լ���
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
				m_sockaddr;	// ���� �ּ� ����ü.

		CriticalSection
				m_csSend;

		IOBuffer
				m_BufferRecv;
		std::queue< CMessage* >
				m_queSend;

		/*
		HeartBit ������ ���Žð�
		*/
		CDHTime	m_tmHeartBitRefresh;
		bool	m_bEnableHeartBit = true;
        UInt64  m_HeartBitSendTick = 0;
        UInt64  m_HeartBitRecvTick = 0;

		/*
		TCP ��Ŷ�� ���� ��ȣ
		: �̸� Ȱ���Ͽ� �ߺ���Ŷ �ݹ�� üũ�Ͽ� ������ ���� ������.
		*/
		Int32	m_SequenceNumSend = 0;
		Int32	m_SequenceNumRecv = 0;
		Int32	m_SequenceNumFailCount = 0;

        /*
        ���������� ������ PacketID Keep
        Sequence Number üũ���� �α� ������ ���
        */
        Int32   m_LastRecvPacketID = 0;

		char	m_bufferZip[MAX_PACKET_SIZE] = {0,};

        /*
        Ŭ���̾�Ʈ ������ ���Ǵ� �뵵
        */
        INetCoreEvent*
                m_INetCoreEvent = nullptr;  // �޼��� �ڵ鷯

		NO_COPYABLE( CSocket );
	};

	REFOBJECTCOUNT( CSocket );
}

// SD_BOTH flag�� shutdown�ϴ� ���, shutdown ���� �ٷ� closesocket ȣ���ϴ� �Ͱ� �����ϴ�.
// ��/���� ������ �ִ� �����͸� �ս��� ���ɼ��� �ִ�.
#define CLOSE_SOCKET( socket ) \
	if( socket != INVALID_SOCKET ) \
	{ \
	::shutdown( socket, SD_BOTH ); \
	::closesocket( socket ); \
	socket = INVALID_SOCKET; \
	} 

