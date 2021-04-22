#pragma once






class CEchoServer : public INetServerEvent, public TestC2S::Stub
{
public:
	CEchoServer();
	virtual ~CEchoServer();

public:
	virtual void OnTick() override;

	virtual void OnClientJoin( HostID remote, StringW hostName ) override;
	virtual void OnClientLeave( HostID remote, ErrorType errorType ) override;

public:
	void	Start();
	void	End();

public:
	CDHNetServerPtr
			m_netServer;
	TestS2C::Proxy
			m_proxy;

public:
	DECRMI_TestC2S_LogInReq;
};

REFCOUNT( CEchoServer );
