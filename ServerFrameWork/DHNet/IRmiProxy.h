#pragma once

#include "BasicType.h"
#include "DHEnums.h"


namespace DHNet
{
	class CMessage;
	class IRmiHost;

	/*
	NIDL �����Ϸ��� ������ Proxy Ŭ������ ���̽� Ŭ����
	*/

	class IRmiProxy
	{
	public:
		IRmiHost *m_core = nullptr;

	public:
		void RmiSend( HostID remote, RmiContext rmiContext, Int32 packetID, CMessage *msg );
	};
}