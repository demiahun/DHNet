using System;
using System.Net;
using System.Collections.Generic;


using NGNet;
namespace TestC2S
{
	public class Stub : NGNet.RmiStub
	{
		public delegate bool LogInReqDelegate( string deviceID, DateTime tmVaue, List<int> vecValue );
		public LogInReqDelegate LogInReq = delegate ( string deviceID, DateTime tmVaue, List<int> vecValue )
		{
			return false;
		};
		public delegate bool CheckServerInfoReqDelegate(  );
		public CheckServerInfoReqDelegate CheckServerInfoReq = delegate (  )
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
			case Common.Rmi_CheckServerInfoReq:
				ProcessReceivedMessage_CheckServerInfoReq( msg );
				break;
			default: return false;
			}
			return true;
		}
		void ProcessReceivedMessage_LogInReq( Message msg )
		{
			string deviceID; UMessageMarshal.Read( msg, out deviceID );
			DateTime tmVaue; UMessageMarshal.Read( msg, out tmVaue );
			List<int> vecValue; UMessageMarshal.Read( msg, out vecValue );
			// Call this method.
			bool _ret = LogInReq( deviceID, tmVaue, vecValue );
			if( _ret == false )
				Console.WriteLine( "Error: RMI function that a user did not create has been called." );
		}
		void ProcessReceivedMessage_CheckServerInfoReq( Message msg )
		{
			// Call this method.
			bool _ret = CheckServerInfoReq(  );
			if( _ret == false )
				Console.WriteLine( "Error: RMI function that a user did not create has been called." );
		}
	}
}

namespace TestS2C
{
	public class Stub : NGNet.RmiStub
	{
		public delegate bool LogInAckDelegate( string deviceID, DateTime tmValue, List<int> vecValue );
		public LogInAckDelegate LogInAck = delegate ( string deviceID, DateTime tmValue, List<int> vecValue )
		{
			return false;
		};
		public delegate bool CheckServerInfoAckDelegate( int iError, EventInfo info );
		public CheckServerInfoAckDelegate CheckServerInfoAck = delegate ( int iError, EventInfo info )
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
			case Common.Rmi_CheckServerInfoAck:
				ProcessReceivedMessage_CheckServerInfoAck( msg );
				break;
			default: return false;
			}
			return true;
		}
		void ProcessReceivedMessage_LogInAck( Message msg )
		{
			string deviceID; UMessageMarshal.Read( msg, out deviceID );
			DateTime tmValue; UMessageMarshal.Read( msg, out tmValue );
			List<int> vecValue; UMessageMarshal.Read( msg, out vecValue );
			// Call this method.
			bool _ret = LogInAck( deviceID, tmValue, vecValue );
			if( _ret == false )
				Console.WriteLine( "Error: RMI function that a user did not create has been called." );
		}
		void ProcessReceivedMessage_CheckServerInfoAck( Message msg )
		{
			int iError; UMessageMarshal.Read( msg, out iError );
			EventInfo info; UMessageMarshal.Read( msg, out info );
			// Call this method.
			bool _ret = CheckServerInfoAck( iError, info );
			if( _ret == false )
				Console.WriteLine( "Error: RMI function that a user did not create has been called." );
		}
	}
}

