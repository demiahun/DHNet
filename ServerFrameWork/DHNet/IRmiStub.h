#pragma once


#pragma pack(push,8)

namespace DHNet
{
	/*
	DIDL �����Ϸ��� ������ Stub Ŭ������ ���̽� Ŭ����
	*/

	class IRmiStub
	{
	public:
		IRmiStub()
		{}
		virtual ~IRmiStub()
		{}
		/*
		�� �Լ��� �������� �� ��.NIDL �����Ϸ��� ������� override�Ѵ�.
		*/
		virtual bool ProcessReceivedMessage( HostID remote, CMessage* msg ) = 0;
	};
}

#pragma pack(pop)