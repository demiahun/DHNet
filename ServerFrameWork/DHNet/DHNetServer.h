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
���� �ν��Ͻ� 1���� CDHNetServer �� 2�� �̻� ��� �Ҽ� ���� ������
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

		WorkThread �� ���� ���� �޼��� ó��
		# �Ϻ� ���� �ܿ��� Stub ������ ������ �������� ������
		# ���ٸ� INetServerEvent �� ���� ������.
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
		Socket map �� ���ݼ��� read �� ���� packet ����̿��� RWLock �� ���
		*/
		RWLock	m_srwSocket;
		CAtlMap< HostID, CSocketPtr >
				m_mapSocket;

	public:
		HANDLE	GetIOCPHandleByIOThread( HostID remote );
		CWorkThreadPtr
				GetWorkThread( UINT indexCount );
		/*
		WorkThread �� ������� �������ش�.
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