#pragma once

#include "BasicType.h"
#include "DHEnums.h"



namespace DHNet
{
	class CMessage;

	class IRmiHost
	{
	public:
		virtual void RmiSend( HostID remote, RmiContext rmiContext, Int32 packetID, CMessage *msg )
		{}
	};
}