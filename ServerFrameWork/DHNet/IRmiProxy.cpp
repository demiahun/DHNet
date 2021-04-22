#include "stdafx.h"
#include "IRmiProxy.h"
#include "IRmiHost.h"
#include "DHNetServer.h"


namespace DHNet
{

	void IRmiProxy::RmiSend( HostID remote, RmiContext rmiContext, Int32 packetID, CMessage *msg )
	{
		if( m_core == nullptr )
		{
			DHLOG_ERROR( L"m_core == nullptr : remote(%I64d), packetID(%d)", remote, packetID );
			return;
		}
		m_core->RmiSend( remote, rmiContext, packetID, msg );
	}

}