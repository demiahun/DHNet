#pragma once

#include <vector>
#include "DHLock.h"
#include "DHIOObject.h"
#include "DHThread.h"
#include "DHIOBuffer.h"

#pragma pack(push,8)

/*
IOCP 를 이용한 타이머 호출
*/

namespace DHNet
{
	class TimerParameter
	{
	public:
		TimerParameter(){}
		~TimerParameter(){}

		void	Clear();

		void	SetParameter( DWORD time, HANDLE _iocp, DWORD _transfereed, ULONG_PTR _completionKey, LPOVERLAPPED _lpOverlapped );

	public:
		UInt32	m_index = 0;

		DWORD	m_Time = 0;	// 설정된 시간(Milliseconds)뒤에 IOCP 를 통해 이벤트 전달

		HANDLE	iocp = nullptr;
		DWORD	transfereed = 0;
		ULONG_PTR
				completionKey = 0;
		LPOVERLAPPED
				lpOverlapped = nullptr;
	};
	extern IOBufferPool<TimerParameter> GTimerParamPool;

	class CDHTimer : public CIOObject, public CDHThread
	{
	public:
		CDHTimer();
		virtual ~CDHTimer();

		virtual bool Loop() override;

	public:
		bool	Start( DWORD tmTick );
		void	End();

	private:
		DWORD	m_tmTick = 10;

	public:
		void	AddParam( TimerParameter *param );

	private:
		enum OVERLAPPED_TYPE
		{
			OLT_ADDPARAM = 0,
			OLT_TICK,

			OLT_END,
		};
		OVERLAPPED
				m_overlapped[OLT_END];
		std::vector< TimerParameter* >
				m_vecTimerParam;
	};

	extern CDHTimer	GTimer;
}

#pragma pack(pop)