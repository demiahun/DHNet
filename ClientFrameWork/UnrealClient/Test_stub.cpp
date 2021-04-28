#pragma once

#include "Test_stub.h"

namespace TestC2S
{
	bool Stub::ProcessReceivedMessage( HostID remote, DHNet::CMessage* msg )
	{
		switch( msg->GetID() )
		{
		case Rmi_LogInReq:
			{
				FString deviceID; *msg >> deviceID;
				FDateTime tmVaue; *msg >> tmVaue;
				TArray<int> vecValue; *msg >> vecValue;

				bool _ret = LogInReq( remote, deviceID, tmVaue, vecValue );
				if( _ret == false )
					::printf( "LogInReq function that a user did not create has been called.#####" );
			}
			break;
			default: return false;
		}
		return true;
	}
}

namespace TestS2C
{
	bool Stub::ProcessReceivedMessage( HostID remote, DHNet::CMessage* msg )
	{
		switch( msg->GetID() )
		{
		case Rmi_LogInAck:
			{
				FString deviceID; *msg >> deviceID;
				FDateTime tmValue; *msg >> tmValue;
				TArray<int> vecValue; *msg >> vecValue;

				bool _ret = LogInAck( remote, deviceID, tmValue, vecValue );
				if( _ret == false )
					::printf( "LogInAck function that a user did not create has been called.#####" );
			}
			break;
			default: return false;
		}
		return true;
	}
}

