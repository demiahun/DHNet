#pragma once


#include "CoreMinimal.h"
#include "CenterServer_stub.h"
#include "CenterServer_proxy.h"
#include "NetClient.h"

using namespace DHNet;

class CEchoServer : public TestS2C::Stub
{
public:
	CEchoServer();
	virtual ~CEchoServer();

	bool	Connect( FString _HostName, Int32 _Port );
	void	Disconnect();

	void	OnTick();

	bool	IsConnected();

public:
	TSharedPtr< FNetClient >
			m_netClient;
	CenterServerC2S::Proxy
			m_proxy;

	FString	HostName;
	Int32	Port = 0;

public:
	DECRMI_TestC2S_LogInAck;
};