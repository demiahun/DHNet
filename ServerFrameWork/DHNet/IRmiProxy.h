#pragma once

#include "BasicType.h"
#include "DHEnums.h"


namespace DHNet
{
	class CMessage;
	class IRmiHost;

	/*
	NIDL 컴파일러가 생성한 Proxy 클래스의 베이스 클래스
	*/

	class IRmiProxy
	{
	public:
		IRmiHost *m_core = nullptr;

	public:
		void RmiSend( HostID remote, RmiContext rmiContext, Int32 packetID, CMessage *msg );
	};
}