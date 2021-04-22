#pragma once

#include <atlcoll.h>
#include "BasicType.h"
#include "DHEnums.h"
#include "DHSocket.h"
#include "INetServerEvent.h"
#include "DHAccepter.h"
#include "DHIOThread.h"
#include "DHWorkThread.h"
#include "DHTickThread.h"
#include "DHPtr.h"
#include "DHStartServerParameter.h"
#include "IRmiHost.h"

/*
서버 인스턴스 1개에 CDHNetServer 를 2개 이상 사용 할수 없는 구조임
*/

namespace DHNet
{
	class IRmiStub;
	class IRmiProxy;

	class CDHNetServer : public IRmiHost, public INetCoreEvent
	{
	public:
		CDHNetServer();
		virtual ~CDHNetServer();

		void	Start( CStartServerParameter &param, ErrorType &outError );
		void	End();

		void	SetEventSink( INetServerEvent *eventSink );
		void	AttachStub( IRmiStub *stub );
		void	AttachProxy( IRmiProxy *proxy );

		/*
		INetCoreEvent override

		WorkThread 를 통해 유저 메세지 처리
		# 하부 서버 단에서 Stub 설정이 있으면 그쪽으로 보내고
		# 없다면 INetServerEvent 를 통해 보낸다.
		*/
        virtual void OnReceiveMessageProcess( HostID remote, CMessage* msg ) override;
		virtual bool OnReceiveUserMessage( HostID remote, CMessage* msg ) override;
        virtual void AddSocket( CSocketPtr socket ) override;
        virtual void RemoveSocket( HostID remote ) override;
        virtual CSocketPtr GetSocket( HostID remote ) override;

		String	GetHostName();
		int		GetPort();

	private:
		CStartServerParameter
				m_StartParameter;
		INetServerEvent*
				m_pNetServerEvent = nullptr;
		std::vector< IRmiStub* >
				m_vecStub;
		std::vector< IRmiProxy* >
				m_vecProxy;

	public:		
		//void	SwapSocket( IN HostID newHost, IN HostID oldHost );		
        void    GetSockets( OUT std::vector< CSocketPtr > &vecSockets );
		Int32	GetSocketCount();

		virtual void RmiSend( HostID remote, RmiContext rmiContext, Int32 packetID, CMessage *msg ) override;

	private:
		/*
		Socket map 의 과반수는 read 를 통한 packet 통신이여서 RWLock 을 사용
		*/
		RWLock	m_srwSocket;
		CAtlMap< HostID, CSocketPtr >
				m_mapSocket;

	public:
		HANDLE	GetIOCPHandleByIOThread( HostID remote );
		CWorkThreadPtr
				GetWorkThread( UINT indexCount );
		/*
		WorkThread 를 순서대로 리턴해준다.
		*/
		CWorkThreadPtr
				GetWorkThreadQueue();

        void    GetIOThreadRunCount( OUT std::vector< UInt64 > &vecCount );
        void    GetWorkThreadRunCount( OUT std::vector< UInt64 > &vecCount );

	private:
		CAccepter
				m_Accepter;
		std::vector< CIOThreadPtr >
				m_vecThreadIO;
		std::vector< CWorkThreadPtr >
				m_vecThreadWork;
		
		CTickThreadPtr
				m_TickThread;

		bool	m_IsStart = false;
		std::atomic< UInt32 >
				m_indexWorkThreadQue;
	};

	REFCOUNT( CDHNetServer );

	extern CDHNetServer *GDHNetServer;
}