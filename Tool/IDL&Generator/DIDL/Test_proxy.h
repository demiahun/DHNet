#pragma once 

#include "Test_common.h"

namespace TestC2S
{
	class Proxy : public NGNet::IRmiProxy 
	{
	public:
		void LogInReq( HostID remote, RmiContext rmiContext, const String &deviceID, const CNgTime &tmVaue, const std::vector<NGDungeonTime> &vecValue );
	};
}

namespace TestS2C
{
	class Proxy : public NGNet::IRmiProxy 
	{
	public:
		void LogInAck( HostID remote, RmiContext rmiContext, const String &deviceID, const CNgTime &tmValue, const std::vector<int> &vecValue );
	};
}

