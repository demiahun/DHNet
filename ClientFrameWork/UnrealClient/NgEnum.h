#pragma once





namespace DHNet
{
	enum ErrorType
	{
		ErrorType_None = -1,

		ErrorType_Ok = 0,
		/*
		���� ����.
		*/
		ErrorType_TCPConnectFailure,
		/*
		StartParam �� �ݹ� �Լ� �Ҵ��� ���� ����
		*/
		ErrorType_InvalidCallbackFunc,
		/*
		Accept ���� ����.
		*/
		ErrorType_StartFailListen,
		/*
		������ ���� ����
		*/
		ErrorType_ReconnectFail,
		/*
		�̹� ����Ǿ� ����
		*/
		ErrorType_AlreadyConnected,
		/*
		����� ȣ��Ʈ���� ������ ��������
		*/
		ErrorType_DisconnectFromRemote,
	};

	enum RmiContext
	{
		Reliable,
		FastEncryp,

		ReliableCompress,
		FastEncrypCompress,
	};
}