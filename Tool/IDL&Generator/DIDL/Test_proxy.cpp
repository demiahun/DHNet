#pragma once

#include "Test_proxy.h"

namespace TestC2S
{
	void Proxy::LogInReq( HostID remote, NGNet::RmiContext rmiContext, const String &deviceID, const CNgTime &tmVaue, const std::vector<NGDungeonTime> &vecValue )
	{
		auto msg = CMessage::Create();
		msg->SetID( Rmi_LogInReq );
		*msg << deviceID;
		*msg << tmVaue;
		*msg << vecValue;
		RmiSend( remote, rmiContext, Rmi_LogInReq, msg );
	}
}

namespace TestS2C
{
	void Proxy::LogInAck( HostID remote, NGNet::RmiContext rmiContext, const String &deviceID, const CNgTime &tmValue, const std::vector<int> &vecValue )
	{
		auto msg = CMessage::Create();
		msg->SetID( Rmi_LogInAck );
		*msg << deviceID;
		*msg << tmValue;
		*msg << vecValue;
		RmiSend( remote, rmiContext, Rmi_LogInAck, msg );
	}
}

