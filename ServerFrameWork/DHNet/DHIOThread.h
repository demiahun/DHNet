#pragma once

#include "DHThread.h"
#include "DHIOObject.h"
#include "DHPtr.h"

/*
CSocket 의 Send,Recv 처리를 진행 해주는 쓰레드
	CSocket 이 생성되어 IO 를 위해 할달되는 IOCP 관리	
*/
namespace DHNet
{
	class CIOThread : public CDHThread, public CIOObject
	{
	public:
		CIOThread();
		virtual ~CIOThread(){}

		virtual bool Loop() override;

	public:
		/*
		I/O 포트, Thread 종료
		this::End() -> IOCP -> this::Loop 에서 종료처리
		*/
		void	End();

		void	SetIndex( UINT index );

	private:
		/*
		IOThread index
			DHNetServer 에서 IOThread 생성시 순차적으로 할당
		*/
		UINT	m_index = 0;
	};

	REFCOUNT( CIOThread );
}


