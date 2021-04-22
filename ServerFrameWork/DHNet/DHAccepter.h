#pragma once

#include "DHThread.h"

namespace DHNet
{
    class INetCoreEvent;

	class CAccepter : public CDHThread
	{
	public:
		CAccepter();
		virtual ~CAccepter();

		virtual bool Loop() override;

		bool	Start( const WCHAR *szIP, int iPort );
		void	End();

        void	SetINetCoreEvent( INetCoreEvent *v );

	private:
		/*
		lesten socket
		*/
		SOCKET	m_Socket = INVALID_SOCKET;

        INetCoreEvent
                *m_INetCoreEvent = nullptr;
	};
}


