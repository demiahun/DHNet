#include "EchoServer.h"





CEchoServer::CEchoServer()
{
	m_netClient = TSharedPtr< FNetClient >( new FNetClient() );
}

CEchoServer::~CEchoServer()
{

}

bool CEchoServer::Connect( FString _HostName, Int32 _Port )
{
	m_netClient->AttachStub( this );
	m_netClient->AttachProxy( &m_proxy );

	if( m_netClient->Start( _HostName, _Port ) == false )
	{
		UE_LOG( LogTemp, Error, TEXT( "failed CenterServer connect : HostName(%s), Port(%d)" ), *_HostName, _Port );
		return false;
	}

	HostName = _HostName;
	Port = _Port;

	return true;
}

void CEchoServer::Disconnect()
{
	m_netClient->End();
}

void CEchoServer::OnTick()
{
	m_netClient->FrameMove();
}

bool CEchoServer::IsConnected()
{
	return m_netClient->IsConnected();
}

DEFRMI_TestS2C_LogInAck( CEchoServer )
{
	// HostID remote, const String &deviceID, const CDHTime &tmValue, const std::vector<int> &vecValue
	
	UE_LOG( LogTemp, Warning, TEXT( "LogInAck.!" ) );

	for( auto &info : vecServerGroupInfo )
	{
		UE_LOG( LogTemp, Warning, TEXT( "NGServerGroupInfo" ) );

		UE_LOG( LogTemp, Warning, TEXT( "GroupID : %d" ), info.GroupID );
		UE_LOG( LogTemp, Warning, TEXT( "CenterServerIp : %s" ), *info.CenterServerIp );
		UE_LOG( LogTemp, Warning, TEXT( "CenterServerPort : %d" ), info.CenterServerPort );
		UE_LOG( LogTemp, Warning, TEXT( "ServerName : %s" ), *info.ServerName );
	}
	for( auto &info : vecAccountInfo )
	{
		UE_LOG( LogTemp, Warning, TEXT( "NGAccountInfo" ) );

		UE_LOG( LogTemp, Warning, TEXT( "DeviceID : %s" ), *info.DeviceID );
		UE_LOG( LogTemp, Warning, TEXT( "AccountID : %d" ), info.AccountID );
	}

	return true;
}