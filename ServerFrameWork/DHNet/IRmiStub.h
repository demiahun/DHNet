#pragma once


#pragma pack(push,8)

namespace DHNet
{
	/*
	DIDL 컴파일러가 생성한 Stub 클래스의 베이스 클래스
	*/

	class IRmiStub
	{
	public:
		IRmiStub()
		{}
		virtual ~IRmiStub()
		{}
		/*
		이 함수를 구현하지 말 것.NIDL 컴파일러의 결과물이 override한다.
		*/
		virtual bool ProcessReceivedMessage( HostID remote, CMessage* msg ) = 0;
	};
}

#pragma pack(pop)