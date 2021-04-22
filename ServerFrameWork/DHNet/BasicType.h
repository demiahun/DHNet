#pragma once

// 데이터 타입(C# 과 데이터 형식을 맞추어야 하는 데이터형 들)
typedef byte				Byte;
typedef __int16				Int16;
typedef unsigned __int16	UInt16;
typedef __int32				Int32;
typedef unsigned __int32	UInt32;
typedef __int64				Int64;
typedef unsigned __int64	UInt64;

// 클래스에 이걸 선언하면 복사 불가능한 상태가 된다.
// 실수로 assignment 구문을 넣는 것을 막게 하려면 유용하다. 
#define NO_COPYABLE(typeName) \
	private: \
		inline typeName& operator=(const typeName&) { return *this; } \
		inline typeName(const typeName&) {}

/*
서버 인스턴스 내에서 Socket(user) 식벽을 위한 키
*/
typedef Int64 HostID;
const HostID HostID_None = -1;

namespace DHNet
{
	const Int32 MAX_PACKET_SIZE = (1024 * 256);
	typedef Int32 STRING_LENGTH_DATA_TYPE;
	/*
	PacketSize, PacketID, SequenceNum, RmiContext flag
	*/
	const Int32 HEADSIZE = sizeof( Int32 ) + sizeof( Int32 ) + sizeof( Int32 ) + sizeof( Byte );
	const Int32 HEAD_POS_PACKET_SIZE = 0;
	const Int32 HEAD_POS_PACKET_ID = sizeof( Int32 );
	const Int32 HEAD_POS_SEQUENCE_NUM = sizeof( Int32 ) + sizeof( Int32 );
	const Int32 HEAD_POS_RMICONTEXT = sizeof( Int32 ) + sizeof( Int32 ) + sizeof( Int32 );

	const DWORD HEART_BIT_TIME = 3000;	// Milliseconds

	const Int32 PACKETID_HEART_BIT = -1;
	/*
	server -> client 접속시 알려줌
	*/
	const Int32 PACKETID_SC_HOSTID_INFO = -2;
	/*
	client -> server 첫 접속시 받았다고 회신
		Server PACKETID_SC_HOSTID_INFO -> clinet
		client PACKETID_CS_HOSTID_RECV -> Server
	client -> server 재접속 이면 HostID 를 보냄
		Server PACKETID_SC_HOSTID_INFO -> clinet
		client PACKETID_CS_HOSTID_RECONNECT -> Server
	*/
	const Int32 PACKETID_CS_HOSTID_RECV = -3;
	const Int32 PACKETID_CS_HOSTID_RECONNECT = -4;
	/*
	Server -> Client 재접속 결과 회신
	*/
	const Int32 PACKETID_SC_RECONNECT_SUCCESS = -5;
	const Int32 PACKETID_SC_RECONNECT_FAIL = -6;

	// 첫접속 완료 패킷(클라,서버 통신보안등 준비를 서로 마무리 했다는 신호)
	const Int32 PACKETID_SC_CONNECT_SUCCESS = -7;

    const Int32 PACKETID_SYSTEM_OnClientJoin = -100;
    const Int32 PACKETID_SYSTEM_OnClientLeave = -101;
    const Int32 PACKETID_SYSTEM_OnClientOnline = -102;
    const Int32 PACKETID_SYSTEM_OnClientOffline = -103;
    const Int32 PACKETID_SYSTEM_OnDelayHeartbit = -104;
}