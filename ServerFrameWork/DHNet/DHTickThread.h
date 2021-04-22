#pragma once

#include "DHThread.h"
#include "INetServerEvent.h"

namespace DHNet
{
	class CTickThread : public CDHThread
	{
	public:
		CTickThread( INetServerEvent *pNetServerEvent, DWORD OnTickCallbackIntervalMs )
			: CDHThread( L"TickThread" )
		{
			m_pNetServerEvent = pNetServerEvent;
			m_OnTickCallbackIntervalMs = OnTickCallbackIntervalMs;
		}
		virtual ~CTickThread(){}

		virtual	bool Loop() override
		{
			if( m_pNetServerEvent == nullptr )
				return false;

			auto tickStart = ::GetTickCount64();

			m_pNetServerEvent->OnTick();

			auto tickInterval = ::GetTickCount64() - tickStart;

			auto tickSleep = m_OnTickCallbackIntervalMs > tickInterval ? (m_OnTickCallbackIntervalMs - tickInterval) : 1;

			::Sleep( static_cast<DWORD>(tickSleep) );

			return true;
		}

	public:
		void	End()
		{
			CDHThread::SetWorking( false );
			CDHThread::End();
		}

	private:
		INetServerEvent
				*m_pNetServerEvent = nullptr;
		UInt64	m_OnTickCallbackIntervalMs = 0;
	};
	REFCOUNT( CTickThread );
}
