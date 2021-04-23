using System;
using System.Net;
using System.Collections.Generic;


using DHNet;
namespace TestC2S
{
	public class Stub : DHNet.RmiStub
	{
		public delegate bool LogInReqDelegate( string deviceID, DateTime tmVaue, List<string> vecValue );
		public LogInReqDelegate LogInReq = delegate ( string deviceID, DateTime tmVaue, List<string> vecValue )
		{
			return false;
		};
		public override bool ProcessReceivedMessage( Message msg )
		{
			switch( msg.ID )
			{
			case Common.Rmi_LogInReq:
				ProcessReceivedMessage_LogInReq( msg );
				break;
			default: return false;
			}
			return true;
		}
		void ProcessReceivedMessage_LogInReq( Message msg )
		{
			string deviceID; UMessageMarshal.Read( msg, out deviceID );
			DateTime tmVaue; UMessageMarshal.Read( msg, out tmVaue );
			List<string> vecValue; UMessageMarshal.Read( msg, out vecValue );
			// Call this method.
			bool _ret = LogInReq( deviceID, tmVaue, vecValue );
			if( _ret == false )
				Console.WriteLine( "Error: RMI function that a user did not create has been called." );
		}
	}
}

namespace TestS2C
{
	public class Stub : DHNet.RmiStub
	{
		public delegate bool LogInAckDelegate( string deviceID, DateTime tmValue, List<string> vecValue );
		public LogInAckDelegate LogInAck = delegate ( string deviceID, DateTime tmValue, List<string> vecValue )
		{
			return false;
		};
		public override bool ProcessReceivedMessage( Message msg )
		{
			switch( msg.ID )
			{
			case Common.Rmi_LogInAck:
				ProcessReceivedMessage_LogInAck( msg );
				break;
			default: return false;
			}
			return true;
		}
		void ProcessReceivedMessage_LogInAck( Message msg )
		{
			string deviceID; UMessageMarshal.Read( msg, out deviceID );
			DateTime tmValue; UMessageMarshal.Read( msg, out tmValue );
			List<string> vecValue; UMessageMarshal.Read( msg, out vecValue );
			// Call this method.
			bool _ret = LogInAck( deviceID, tmValue, vecValue );
			if( _ret == false )
				Console.WriteLine( "Error: RMI function that a user did not create has been called." );
		}
	}
}

