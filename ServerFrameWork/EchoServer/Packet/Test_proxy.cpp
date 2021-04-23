#pragma once

#include "Test_proxy.h"

namespace TestC2S
{
	void Proxy::LogInReq( HostID remote, DHNet::RmiContext rmiContext, const String &deviceID, const CDHTime &tmVaue, const std::vector<String> &vecValue )
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
	void Proxy::LogInAck( HostID remote, DHNet::RmiContext rmiContext, const String &deviceID, const CDHTime &tmValue, const std::vector<String> &vecValue )
	{
		auto msg = CMessage::Create();
		msg->SetID( Rmi_LogInAck );
		*msg << deviceID;
		*msg << tmValue;
		*msg << vecValue;
		RmiSend( remote, rmiContext, Rmi_LogInAck, msg );
	}
}

