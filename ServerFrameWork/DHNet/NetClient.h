#pragma once

#include "IRmiHost.h"
#include "DHSocket.h"
#include "DHThread.h"
#include "INetServerEvent.h"
#include "DHWorkThread.h"

namespace DHNet
{
	class IRmiStub;
	class IRmiProxy;

	class CNetClient : public IRmiHost, public CDHThread, public INetCoreEvent
	{
	public:
		CNetClient();
		virtual ~CNetClient();

		/*
		CDHThread override
		*/
		virtual	bool Loop() override;
		/*
		IRmiHost override
		*/
		virtual void RmiSend( HostID remote, RmiContext rmiContext, Int32 packetID, CMessage *msg ) override;

		/*
		INetCoreEvent override
		*/
		virtual bool OnReceiveUserMessage( HostID remote, CMessage* msg ) override;
		virtual void OnReceiveMessageProcess( HostID remote, CMessage* msg ) override;

	public:
		bool	Connect( String hostName, int port, UInt32 _RecvThreadSize = 4 );

		bool	IsConnected();
		void	Disconnect();

		void	AttachStub( IRmiStub *stub );
		void	AttachProxy( IRmiProxy *proxy );

		void	SocketClose( bool IsDisconectSuccess = false );

		HostID	GetHostID();

	private:
		CSocketPtr 
				m_netSocket;
		std::vector< IRmiStub* >
				m_vecStub;
		std::vector< IRmiProxy* >
				m_vecProxy;
		std::vector< CWorkThreadPtr >
				m_vecThreadWork;
		std::atomic< UInt32 >
				m_indexWorkThreadQue = 0;
	};

	REFCOUNT( CNetClient );
}

