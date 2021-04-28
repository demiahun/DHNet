// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Networking.h"
#include "IRmiHost.h"

/*
	Thread 를 통한 소켓 동작 메인 클래스
*/
namespace DHNet
{
	class FNetClient : public FRunnable, public IRmiHost
	{
		/* FRunnable 을 이용한 쓰레드 동작 주체 */
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
			접속 시동 및 성공 후 쓰레드 시작
		*/
		bool	Start( FString hostName, int32 port );
		void	End();

		void	FrameMove();	// 클라이언트 Tick() 에서 후출되어 Packet Recv 처리를 진행한다.

		void	AttachStub( class IRmiStub *stub );
		void	AttachProxy( class IRmiProxy *proxy );

		bool	IsConnected();

	private:
		/*
			FNetClient::Run() 에서 서버 패킷일 경우 호출하여 처리되는 함수
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