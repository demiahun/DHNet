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
		message �ڵ鸵 ������ ���
		# message ���� �м��� �ƴϰ� �����̳� �ٸ� �뵵�� ����ϸ� �ɵ�.?
		*/
		virtual void OnReceiveMessageProcess( HostID remote, CMessage* msg )
		{return;}

		/*
		message �����ϸ� ���Լ��� ȣ���Ͽ� �����Ѵ�.
		# ��Ŷ�� ���� �м��Ͽ� ���� override �Ͽ� ����ϸ� �ȴ�.
		*/
		virtual bool OnReceiveUserMessage( HostID remote, CMessage* msg )
		{return false;}

        /*
        ���ϰ���
        */
        virtual void AddSocket( CSocketPtr socket ){}
        virtual void RemoveSocket( HostID remote ){}
        virtual CSocketPtr GetSocket( HostID remote ){ return nullptr; }
	};
	/*
	������ ������ Ŭ���� ���� ��ӹ޾� ���Ǹ�
	OnClientJoin() �� ���� ����Ŭ���� ���� �� ����ó��
	OnClinetLeave() �� ���� ���� Ŭ���� ���� �� ����
	*/
	class INetServerEvent : public INetCoreEvent
	{
	public:
		virtual ~INetServerEvent(){}

		/*
		���� �ð����� �ݹ�Ǵ� �Լ�
		CStartServerParameter::m_OnTickCallbackIntervalMs �� �����Ͽ� ���� ���۽� �Ѱ� �־�� ������
		*/
		virtual void OnTick(){}

		/**
		\~korean
		Ŭ���̾�Ʈ�� ������ ������ �߻��ϴ� �̺�Ʈ
		- OnConnectionRequest���� ���� �㰡�� �� Ŭ���̾�Ʈ�� ���ؼ��� �ݹ�ȴ�.
		\param clientInfo ������ ���� Ŭ���̾�Ʈ
		*/
		virtual void OnClientJoin( HostID remote, StringW hostName ) = 0;
		/*
		Ŭ���̾�Ʈ�� ������ �����ϸ� �߻��ϴ� �̺�Ʈ�Դϴ�.
		*/
		virtual void OnClientLeave( HostID remote, ErrorType errorType ) = 0;
		/*
		Ŭ���̾�Ʈ�� ���� �� �� ���� �Ǿ��� �� �ݹ� �˴ϴ�.
		*/
		virtual void OnClientOnline( HostID remote ){}
		/*
		Ŭ���̾�Ʈ�� ������ �� �ݹ� �˴ϴ�.
		�ش� �̺�Ʈ�� �ݹ��� �� ��, �� ������ �̷������ ������
		OnClientLeave �� �ݹ� �˴ϴ�.
		(�ʿ���� ��� �������� �ʾƵ� �Ǳ� ������ ���� �ص�)
		*/
		virtual void OnClientOffline( HostID remote ){}
        /*
        GetTickCount�� Heartbit Time üũ�ؼ� 
        */
        virtual void OnDelayHeartbit( HostID remote, DWORD _DealyMS ){}
	};
}

#pragma pack(pop)