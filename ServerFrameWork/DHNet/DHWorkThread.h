#pragma once

#include "DHThread.h"
#include "DHIOObject.h"
#include "DHPtr.h"
#include "INetServerEvent.h"

/*
CSocket 의 Send,Recv 처리를 진행 해주는 쓰레드
	CSocket 이 생성되어 IO 를 위해 할달되는 IOCP 관리
*/
namespace DHNet
{
	class CMessage;

	class CWorkThread : public CDHThread, public CIOObject, public INetCoreEvent
	{
	public:
		CWorkThread();
		virtual ~CWorkThread(){}

		/*
		CNgThread override
		*/
		virtual bool Loop() override;
		/*
		INetCoreEvent

		OnReceiveMessageProcess
			외부 다른 쓰레드 에서 호출되어 CWorkThread IOCP 에 전달 후 CWorkThread::Loop 에서 전달 받아 처리
		*/
		virtual void OnReceiveMessageProcess( HostID remote, CMessage* msg ) override;

	public:
		/*
		I/O 포트, Thread 종료
		this::End() -> IOCP -> this::Loop 에서 종료처리
		*/
		void	End();

		void	SetIndex( UINT index );

		void	SetEventSink( INetCoreEvent *eventSink );

        LPWSAOVERLAPPED
                GetOverlappedPostRecv();

	private:
		/*
		IOThread index
		DHNetServer 에서 IOThread 생성시 순차적으로 할당
		*/
		UINT	m_index = 0;
		INetCoreEvent*
				m_pNetCoreEvent = nullptr;


		OVERLAPPED
				m_overlappedPostRecv;
	};
	REFCOUNT( CWorkThread );
}

