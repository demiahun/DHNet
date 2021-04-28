#pragma once





namespace DHNet
{
	enum ErrorType
	{
		ErrorType_None = -1,

		ErrorType_Ok = 0,
		/*
		연결 실패.
		*/
		ErrorType_TCPConnectFailure,
		/*
		StartParam 에 콜백 함수 할당을 하지 않음
		*/
		ErrorType_InvalidCallbackFunc,
		/*
		Accept 시작 실패.
		*/
		ErrorType_StartFailListen,
		/*
		재접속 연결 실패
		*/
		ErrorType_ReconnectFail,
		/*
		이미 연결되어 있음
		*/
		ErrorType_AlreadyConnected,
		/*
		상대측 호스트에서 연결을 해제했음
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