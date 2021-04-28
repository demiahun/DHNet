#pragma once 

#include "Test_common.h"

namespace TestC2S
{
	class Stub : public DHNet::IRmiStub 
	{
	public:
		virtual bool LogInReq( HostID remote, const String &deviceID, const CDHTime &tmVaue, const std::vector<int> &vecValue )
		{ return false; }
#define DECRMI_TestC2S_LogInReq bool LogInReq( HostID remote, const String &deviceID, const CDHTime &tmVaue, const std::vector<int> &vecValue ) override
#define DEFRMI_TestC2S_LogInReq(DerivedClass) bool DerivedClass::LogInReq( HostID remote, const String &deviceID, const CDHTime &tmVaue, const std::vector<int> &vecValue )

		virtual bool ProcessReceivedMessage( HostID remote, DHNet::CMessage* msg ) override;
	};
}

namespace TestS2C
{
	class Stub : public DHNet::IRmiStub 
	{
	public:
		virtual bool LogInAck( HostID remote, const String &deviceID, const CDHTime &tmValue, const std::vector<int> &vecValue )
		{ return false; }
#define DECRMI_TestS2C_LogInAck bool LogInAck( HostID remote, const String &deviceID, const CDHTime &tmValue, const std::vector<int> &vecValue ) override
#define DEFRMI_TestS2C_LogInAck(DerivedClass) bool DerivedClass::LogInAck( HostID remote, const String &deviceID, const CDHTime &tmValue, const std::vector<int> &vecValue )

		virtual bool ProcessReceivedMessage( HostID remote, DHNet::CMessage* msg ) override;
	};
}

