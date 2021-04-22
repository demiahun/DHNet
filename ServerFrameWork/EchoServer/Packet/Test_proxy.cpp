#pragma once

#include "Test_proxy.h"

namespace TestC2S
{
	void Proxy::LogInReq( HostID remote, DHNet::RmiContext rmiContext, const String &deviceID, const CDHTime &tmVaue, const std::vector<int> &vecValue )
	{
		auto msg = CMessage::Create();
		msg->SetID( Rmi_LogInReq );
		*msg << deviceID;
		*msg << tmVaue;
		*msg << vecValue;
		RmiSend( remote, rmiContext, Rmi_LogInReq, msg );
	}
	void Proxy::CheckServerInfoReq( HostID remote, DHNet::RmiContext rmiContext )
	{
		auto msg = CMessage::Create();
		msg->SetID( Rmi_CheckServerInfoReq );
		RmiSend( remote, rmiContext, Rmi_CheckServerInfoReq, msg );
	}
}

namespace TestS2C
{
	void Proxy::LogInAck( HostID remote, DHNet::RmiContext rmiContext, const String &deviceID, const CDHTime &tmValue, const std::vector<int> &vecValue )
	{
		auto msg = CMessage::Create();
		msg->SetID( Rmi_LogInAck );
		*msg << deviceID;
		*msg << tmValue;
		*msg << vecValue;
		RmiSend( remote, rmiContext, Rmi_LogInAck, msg );
	}
	void Proxy::CheckServerInfoAck( HostID remote, DHNet::RmiContext rmiContext, const int &iError, const EventInfo &info )
	{
		auto msg = CMessage::Create();
		msg->SetID( Rmi_CheckServerInfoAck );
		*msg << iError;
		*msg << info;
		RmiSend( remote, rmiContext, Rmi_CheckServerInfoAck, msg );
	}
}

