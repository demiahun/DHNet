#pragma once

#include "BasicType.h"
#include "NgEnum.h"


namespace DHNet
{
	class IRmiProxy
	{
	public:
		IRmiProxy(){}
		virtual ~IRmiProxy(){}

		class IRmiHost *m_core = nullptr;

		void RmiSend( HostID remote, RmiContext rmiContext, Int32 packetID, class CMessage *msg );
	};
}