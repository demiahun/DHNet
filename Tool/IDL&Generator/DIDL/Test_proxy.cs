using System;
using System.Net;
using System.Collections.Generic;


using NGNet;
namespace TestC2S
{
	public class Proxy : NGNet.RmiProxy
	{
		public void LogInReq( RmiContext rmiContext, string deviceID, DateTime tmVaue, List<int> vecValue )
		{
			Message msg = new Message();
			UMessageMarshal.Write( msg, deviceID );
			UMessageMarshal.Write( msg, tmVaue );
			UMessageMarshal.Write( msg, vecValue );
			RmiSend( rmiContext, Common.Rmi_LogInReq, msg );
		}
		public void CheckServerInfoReq( RmiContext rmiContext )
		{
			Message msg = new Message();
			RmiSend( rmiContext, Common.Rmi_CheckServerInfoReq, msg );
		}
	}
}

namespace TestS2C
{
	public class Proxy : NGNet.RmiProxy
	{
		public void LogInAck( RmiContext rmiContext, string deviceID, DateTime tmValue, List<int> vecValue )
		{
			Message msg = new Message();
			UMessageMarshal.Write( msg, deviceID );
			UMessageMarshal.Write( msg, tmValue );
			UMessageMarshal.Write( msg, vecValue );
			RmiSend( rmiContext, Common.Rmi_LogInAck, msg );
		}
		public void CheckServerInfoAck( RmiContext rmiContext, int iError, EventInfo info )
		{
			Message msg = new Message();
			UMessageMarshal.Write( msg, iError );
			UMessageMarshal.Write( msg, info );
			RmiSend( rmiContext, Common.Rmi_CheckServerInfoAck, msg );
		}
	}
}

