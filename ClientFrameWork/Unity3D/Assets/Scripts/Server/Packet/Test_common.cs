using System;
using System.Collections.Generic;


using DHNet;
namespace TestC2S
{
	public class Common
	{
		// Message ID that replies to each RMI method.
		public const Int32 Rmi_LogInReq = 1000;
	}
}

namespace TestS2C
{
	public class Common
	{
		// Message ID that replies to each RMI method.
		public const Int32 Rmi_LogInAck = 1200;
	}
}

