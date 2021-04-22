#pragma once

#include "DHThread.h"
#include "DHIOObject.h"
#include "DHPtr.h"
#include "INetServerEvent.h"

/*
CSocket �� Send,Recv ó���� ���� ���ִ� ������
	CSocket �� �����Ǿ� IO �� ���� �Ҵ޵Ǵ� IOCP ����
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
			�ܺ� �ٸ� ������ ���� ȣ��Ǿ� CWorkThread IOCP �� ���� �� CWorkThread::Loop ���� ���� �޾� ó��
		*/
		virtual void OnReceiveMessageProcess( HostID remote, CMessage* msg ) override;

	public:
		/*
		I/O ��Ʈ, Thread ����
		this::End() -> IOCP -> this::Loop ���� ����ó��
		*/
		void	End();

		void	SetIndex( UINT index );

		void	SetEventSink( INetCoreEvent *eventSink );

        LPWSAOVERLAPPED
                GetOverlappedPostRecv();

	private:
		/*
		IOThread index
		DHNetServer ���� IOThread ������ ���������� �Ҵ�
		*/
		UINT	m_index = 0;
		INetCoreEvent*
				m_pNetCoreEvent = nullptr;


		OVERLAPPED
				m_overlappedPostRecv;
	};
	REFCOUNT( CWorkThread );
}

