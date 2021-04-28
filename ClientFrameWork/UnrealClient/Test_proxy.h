#pragma once 

#include "Test_common.h"

namespace TestC2S
{
	class Proxy : public DHNet::IRmiProxy 
	{
	public:
		void LogInReq( HostID remote, RmiContext rmiContext, const FString &deviceID, const FDateTime &tmVaue, const TArray<int> &vecValue );
	};
}

namespace TestS2C
{
	class Proxy : public DHNet::IRmiProxy 
	{
	public:
		void LogInAck( HostID remote, RmiContext rmiContext, const FString &deviceID, const FDateTime &tmValue, const TArray<int> &vecValue );
	};
}

