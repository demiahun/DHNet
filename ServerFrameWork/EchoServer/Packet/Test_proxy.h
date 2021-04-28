#pragma once 

#include "Test_common.h"

namespace TestC2S
{
	class Proxy : public DHNet::IRmiProxy 
	{
	public:
		void LogInReq( HostID remote, RmiContext rmiContext, const String &deviceID, const CDHTime &tmVaue, const std::vector<int> &vecValue );
	};
}

namespace TestS2C
{
	class Proxy : public DHNet::IRmiProxy 
	{
	public:
		void LogInAck( HostID remote, RmiContext rmiContext, const String &deviceID, const CDHTime &tmValue, const std::vector<int> &vecValue );
	};
}

