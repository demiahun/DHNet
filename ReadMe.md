**DHNet**

------

- C++ Network library. Asynchronous. TCP GameServer.



**프로젝트 정보**

------

- C++ 비동기 네트워크 라이브러리
- 게임에서 사용 할 수 있는 TCP 기반의 socket server
- Unity3D 연동(ClientFramework/Unity3D)
- Unreal 연동(ClientFramework/Unreal)



**서버 사용법**

------

DHNet/ServerFramework/EchoServer/EchoServer.h.cpp 참조

Ex.)

`CDHNetServerPtr m_netServer;`

`m_netServer = CDHNetServerPtr( new CDHNetServer() );`

`CStartServerparameter param;`

`param.m_Port = 10000;`

`param.m_OnTickCallbackIntervalMs = 1000;`

`m_netServer->SetEventSink( this );`
`m_netServer->AttachStub( this );`
`m_netServer->AttachProxy( &m_proxy );`

`ErrorType errorType = ErrorType_Ok;`
`m_netServer->Start( param, errorType );`

`if( errorType != ErrorType_Ok )`
`{`
	`DHLOG_ERROR( L"fail : m_netServer->Start() : Error(%d)", errorType );`
	`return;`
`}`



**클라이언트 사용법** 

------

Unity3D, Unreal

`CNetClient netClient = new CNetClient();`

`netClient.AttachProxy( proxy );`

`netClient.AttachStub( stub );`



**stub(packet recv), proxy(packet send) 핸들링**

------

텍스트 문서로 .DIDL 파일을 만들어 패킷 보내기, 패킷 받을 함수를 정의

Ex.)

`// 기본형 데이터 : bool, int, Int16, UInt16, Int32, UInt32, Int64, UInt64`
`// 시간 데이터형 : 스크립트에선 Time(Cpp : CDHTime, UE Cpp : FDateTime, CS : DateTime)`
`// 스트링 : 스크립트에선 string(Cpp : String(NgStringW), UE Cpp : FString, CS : string)`
`// 컨테이너 : 스크립트에선 vector, map(CPP : std::vector, std::map, UE Cpp : TArray, TMap, CS : list, Dictionary)`

`packet TestC2S 1000`
`{`
	`LogInReq( string deviceID, Time tmVaue, vector<int> vecValue );`
`}`

`packet TestS2C 1200`
`{`
	`LogInAck( string deviceID Time tmValue, vector<int> vecValue );`
`}`

을 작성해 주고

DHNet/ServerFramework/DHNet/util/DIDL.exe 를 이용하여

Ex.) 

옵션 : -cs : C# 용 파일, -cpp : C++ 용 파일로 출력

DIDL.exe -cs [DIDL 파일 경로] ..\Unity3D\Assets\Scripts\Server\Packet\

으로 실행하면 

DIDL파일이름_common.cs

DIDL파일이름_proxy.cs

DIDL파일이름_stub.cs 

가 출력됩니다.



## 라이선스

- 소스코드는 상업적, 비상업적 어느 용도이든 자유롭게 사용 가능 합니다.

## License

- All source codes are free to use commercially also.