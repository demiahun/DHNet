#include "IRmiProxy.h"
#include "IRmiHost.h"




namespace DHNet
{
	void DHNet::IRmiProxy::RmiSend( HostID remote, RmiContext rmiContext, Int32 packetID, class CMessage *msg )
	{
		if( m_core == nullptr )
			return;
		m_core->RmiSend( remote, rmiContext, packetID, msg );
	}
}
