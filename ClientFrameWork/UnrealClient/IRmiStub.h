#pragma once

#include "BasicType.h"


namespace DHNet
{
	class IRmiStub
	{
	public:
		IRmiStub(){}
		virtual ~IRmiStub(){}

		/*
		�� �Լ��� �������� �� ��.NIDL �����Ϸ��� ������� override�Ѵ�.
		*/
		virtual bool ProcessReceivedMessage( HostID remote, class CMessage* msg ){ return false; }
	};
}