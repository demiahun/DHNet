#include "stdafx.h"
#include "EchoServer.h"


CEchoServer::CEchoServer()
{
	m_netServer = CDHNetServerPtr( new CDHNetServer() );
}


CEchoServer::~CEchoServer()
{}

void CEchoServer::OnTick()
{
}

void CEchoServer::OnClientJoin( HostID remote, StringW hostName )
{
	DHLOG_WARN( L"HostID(%I64d)", remote );
}

void CEchoServer::OnClientLeave( HostID remote, ErrorType errorType )
{
	DHLOG_WARN( L"HostID(%I64d)", remote );
}

void CEchoServer::Start()
{
	CStartServerParameter param;

	param.m_Port = 10000;
	param.m_OnTickCallbackIntervalMs = 1000;

	m_netServer->SetEventSink( this );
	m_netServer->AttachStub( this );
	m_netServer->AttachProxy( &m_proxy );

	ErrorType errorType = ErrorType_Ok;
	m_netServer->Start( param, errorType );

	if( errorType != ErrorType_Ok )
	{
		DHLOG_ERROR( L"fail : m_netServer->Start() : Error(%d)", errorType );
		return;
	}

	DHLOG_INFO( L"Server start~~.!" );
}

void CEchoServer::End()
{
	m_netServer->End();
}

DEFRMI_TestC2S_LogInReq( CEchoServer )
{
	std::wcout << "LogInReq : DeviceID : " << deviceID.GetString() << std::endl;
	std::wcout << "LogInReq : time : " << tmVaue.Format( L"%Y-%m-%d %H:%M:%S" ).GetString() << std::endl;
	std::cout << "LogInReq :";
	for( auto vit : vecValue )
	{
		std::cout << " " << vit;
	}
	std::cout << std::endl;

	//////////////////////////////////////////////////////////////////////////
	// LogInAck Send

	m_proxy.LogInAck( remote, RmiContext::FastEncryp, deviceID, tmVaue, vecValue );

	/*auto msgAck = CMessage::Create();
	*msgAck << deviceID;
	*msgAck << tmVaue;
	*msgAck << vecValue;

	msgAck->SetEncrypt( true );

	m_netServer->SendPacket( remote, TestS2C::Rmi_LogInAck, msgAck );*/

	return true;
}