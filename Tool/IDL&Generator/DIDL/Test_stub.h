#pragma once 

#include "Test_common.h"

namespace TestC2S
{
	class Stub : public NGNet::IRmiStub 
	{
	public:
		virtual bool LogInReq( HostID remote, const String &deviceID, const CNgTime &tmVaue, const std::vector<NGDungeonTime> &vecValue )
		{ return false; }
#define DECRMI_TestC2S_LogInReq bool LogInReq( HostID remote, const String &deviceID, const CNgTime &tmVaue, const std::vector<NGDungeonTime> &vecValue ) override
#define DEFRMI_TestC2S_LogInReq(DerivedClass) bool DerivedClass::LogInReq( HostID remote, const String &deviceID, const CNgTime &tmVaue, const std::vector<NGDungeonTime> &vecValue )

		virtual bool ProcessReceivedMessage( HostID remote, NGNet::CMessage* msg ) override;
	};
}

namespace TestS2C
{
	class Stub : public NGNet::IRmiStub 
	{
	public:
		virtual bool LogInAck( HostID remote, const String &deviceID, const CNgTime &tmValue, const std::vector<int> &vecValue )
		{ return false; }
#define DECRMI_TestS2C_LogInAck bool LogInAck( HostID remote, const String &deviceID, const CNgTime &tmValue, const std::vector<int> &vecValue ) override
#define DEFRMI_TestS2C_LogInAck(DerivedClass) bool DerivedClass::LogInAck( HostID remote, const String &deviceID, const CNgTime &tmValue, const std::vector<int> &vecValue )

		virtual bool ProcessReceivedMessage( HostID remote, NGNet::CMessage* msg ) override;
	};
}

