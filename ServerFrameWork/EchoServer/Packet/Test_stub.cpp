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
				String deviceID; *msg >> deviceID;
				CDHTime tmVaue; *msg >> tmVaue;
				std::vector<int> vecValue; *msg >> vecValue;

				bool _ret = LogInReq( remote, deviceID, tmVaue, vecValue );
				if( _ret == false )
					::printf( "LogInReq function that a user did not create has been called.#####" );
			}
			break;
		case Rmi_CheckServerInfoReq:
			{

				bool _ret = CheckServerInfoReq( remote );
				if( _ret == false )
					::printf( "CheckServerInfoReq function that a user did not create has been called.#####" );
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
				String deviceID; *msg >> deviceID;
				CDHTime tmValue; *msg >> tmValue;
				std::vector<int> vecValue; *msg >> vecValue;

				bool _ret = LogInAck( remote, deviceID, tmValue, vecValue );
				if( _ret == false )
					::printf( "LogInAck function that a user did not create has been called.#####" );
			}
			break;
		case Rmi_CheckServerInfoAck:
			{
				int iError; *msg >> iError;
				EventInfo info; *msg >> info;

				bool _ret = CheckServerInfoAck( remote, iError, info );
				if( _ret == false )
					::printf( "CheckServerInfoAck function that a user did not create has been called.#####" );
			}
			break;
			default: return false;
		}
		return true;
	}
}

