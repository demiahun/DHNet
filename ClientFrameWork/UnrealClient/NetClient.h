// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Networking.h"
#include "IRmiHost.h"

/*
	Thread �� ���� ���� ���� ���� Ŭ����
*/
namespace DHNet
{
	class FNetClient : public FRunnable, public IRmiHost
	{
		/* FRunnable �� �̿��� ������ ���� ��ü */
		FRunnableThread *Thread = nullptr;

	public:
		FNetClient();
		virtual ~FNetClient();

		/*
			FRunnable override
		*/
		virtual uint32	Run() override;

		/*
			IRmiHost override
		*/
		virtual void RmiSend( HostID remote, RmiContext rmiContext, Int32 packetID, class CMessage *msg ) override;

		/*
			���� �õ� �� ���� �� ������ ����
		*/
		bool	Start( FString hostName, int32 port );
		void	End();

		void	FrameMove();	// Ŭ���̾�Ʈ Tick() ���� ����Ǿ� Packet Recv ó���� �����Ѵ�.

		void	AttachStub( class IRmiStub *stub );
		void	AttachProxy( class IRmiProxy *proxy );

		bool	IsConnected();

	private:
		/*
			FNetClient::Run() ���� ���� ��Ŷ�� ��� ȣ���Ͽ� ó���Ǵ� �Լ�
		*/
		void	_OnReceiveMessage( class CMessage *msg );

	private:
		FSocket	*Socket = nullptr;
		FCriticalSection
				m_cs;
		HostID	m_HostID = 0;

		FString	HostName;
		int32	Port = 0;

		FThreadSafeBool
			bRun = false;
		FThreadSafeBool
			bVerify = false;
		FThreadSafeCounter
			SequenceNumSend = 0;

		/*
			TQueue is Thread safe
		*/
		TQueue< class CMessage* >
			queSend;
		TQueue< class CMessage* >
			queRecv;

		TArray< class IRmiStub* >
			vecStub;
		TArray< class IRmiProxy* >
			vecProxy;
	};
}