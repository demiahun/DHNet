using System;
using System.Collections.Generic;


using NGNet;
namespace TestC2S
{
	public class Common
	{
		// Message ID that replies to each RMI method.
		public const Int32 Rmi_LogInReq = 1000;
		public const Int32 Rmi_CheckServerInfoReq = 1001;
	}
}

namespace TestS2C
{
	public class Common
	{
		// Message ID that replies to each RMI method.
		public const Int32 Rmi_LogInAck = 1200;
		public const Int32 Rmi_CheckServerInfoAck = 1201;
	}
}

