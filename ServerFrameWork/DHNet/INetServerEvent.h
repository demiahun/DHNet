#pragma once

#include "BasicType.h"
#include "DHEnums.h"
#include "DHString.h"
#include "DHSocket.h"

#pragma pack(push,8)

namespace DHNet
{
	class CMessage;

	class INetCoreEvent
	{
	public:
		virtual ~INetCoreEvent(){}

		/*
		message 핸들링 용으로 사용
		# message 직접 분석이 아니고 전달이나 다른 용도로 사용하면 될듯.?
		*/
		virtual void OnReceiveMessageProcess( HostID remote, CMessage* msg )
		{return;}

		/*
		message 도착하면 이함수를 호출하여 전달한다.
		# 패킷을 직접 분석하여 사용시 override 하여 사용하면 된다.
		*/
		virtual bool OnReceiveUserMessage( HostID remote, CMessage* msg )
		{return false;}

        /*
        소켓관리
        */
        virtual void AddSocket( CSocketPtr socket ){}
        virtual void RemoveSocket( HostID remote ){}
        virtual CSocketPtr GetSocket( HostID remote ){ return nullptr; }
	};
	/*
	구현할 서버의 클레스 에서 상속받아 사용되며
	OnClientJoin() 을 통해 유저클래스 생성 및 로직처리
	OnClinetLeave() 을 통해 유저 클래스 제거 및 정리
	*/
	class INetServerEvent : public INetCoreEvent
	{
	public:
		virtual ~INetServerEvent(){}

		/*
		일정 시간마다 콜백되는 함수
		CStartServerParameter::m_OnTickCallbackIntervalMs 을 설정하여 서버 시작시 넘겨 주어야 동작함
		*/
		virtual void OnTick(){}

		/**
		\~korean
		클라이언트가 연결을 들어오면 발생하는 이벤트
		- OnConnectionRequest에서 진입 허가를 한 클라이언트에 대해서만 콜백된다.
		\param clientInfo 연결이 들어온 클라이언트
		*/
		virtual void OnClientJoin( HostID remote, StringW hostName ) = 0;
		/*
		클라이언트가 연결을 해제하면 발생하는 이벤트입니다.
		*/
		virtual void OnClientLeave( HostID remote, ErrorType errorType ) = 0;
		/*
		클라이언트가 끊긴 뒤 재 접속 되었을 때 콜백 됩니다.
		*/
		virtual void OnClientOnline( HostID remote ){}
		/*
		클라이언트가 끊겼을 때 콜백 됩니다.
		해당 이벤트가 콜백이 된 뒤, 재 접속이 이루어지지 않으면
		OnClientLeave 가 콜백 됩니다.
		(필요없는 경우 구현하지 않아도 되기 때문에 정의 해둠)
		*/
		virtual void OnClientOffline( HostID remote ){}
        /*
        GetTickCount로 Heartbit Time 체크해서 
        */
        virtual void OnDelayHeartbit( HostID remote, DWORD _DealyMS ){}
	};
}

#pragma pack(pop)