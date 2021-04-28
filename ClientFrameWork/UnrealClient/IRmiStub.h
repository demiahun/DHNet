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
		이 함수를 구현하지 말 것.NIDL 컴파일러의 결과물이 override한다.
		*/
		virtual bool ProcessReceivedMessage( HostID remote, class CMessage* msg ){ return false; }
	};
}