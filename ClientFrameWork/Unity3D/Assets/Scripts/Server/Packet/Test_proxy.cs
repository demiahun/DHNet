using System;
using System.Net;
using System.Collections.Generic;


using DHNet;
namespace TestC2S
{
	public class Proxy : DHNet.RmiProxy
	{
		public void LogInReq( RmiContext rmiContext, string deviceID, DateTime tmVaue, List<string> vecValue )
		{
			Message msg = new Message();
			UMessageMarshal.Write( msg, deviceID );
			UMessageMarshal.Write( msg, tmVaue );
			UMessageMarshal.Write( msg, vecValue );
			RmiSend( rmiContext, Common.Rmi_LogInReq, msg );
		}
	}
}

namespace TestS2C
{
	public class Proxy : DHNet.RmiProxy
	{
		public void LogInAck( RmiContext rmiContext, string deviceID, DateTime tmValue, List<string> vecValue )
		{
			Message msg = new Message();
			UMessageMarshal.Write( msg, deviceID );
			UMessageMarshal.Write( msg, tmValue );
			UMessageMarshal.Write( msg, vecValue );
			RmiSend( rmiContext, Common.Rmi_LogInAck, msg );
		}
	}
}

