#pragma once

// ������ Ÿ��(C# �� ������ ������ ���߾�� �ϴ� �������� ��)
typedef byte				Byte;
typedef __int16				Int16;
typedef unsigned __int16	UInt16;
typedef __int32				Int32;
typedef unsigned __int32	UInt32;
typedef __int64				Int64;
typedef unsigned __int64	UInt64;

// Ŭ������ �̰� �����ϸ� ���� �Ұ����� ���°� �ȴ�.
// �Ǽ��� assignment ������ �ִ� ���� ���� �Ϸ��� �����ϴ�. 
#define NO_COPYABLE(typeName) \
	private: \
		inline typeName& operator=(const typeName&) { return *this; } \
		inline typeName(const typeName&) {}

/*
���� �ν��Ͻ� ������ Socket(user) �ĺ��� ���� Ű
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
	server -> client ���ӽ� �˷���
	*/
	const Int32 PACKETID_SC_HOSTID_INFO = -2;
	/*
	client -> server ù ���ӽ� �޾Ҵٰ� ȸ��
		Server PACKETID_SC_HOSTID_INFO -> clinet
		client PACKETID_CS_HOSTID_RECV -> Server
	client -> server ������ �̸� HostID �� ����
		Server PACKETID_SC_HOSTID_INFO -> clinet
		client PACKETID_CS_HOSTID_RECONNECT -> Server
	*/
	const Int32 PACKETID_CS_HOSTID_RECV = -3;
	const Int32 PACKETID_CS_HOSTID_RECONNECT = -4;
	/*
	Server -> Client ������ ��� ȸ��
	*/
	const Int32 PACKETID_SC_RECONNECT_SUCCESS = -5;
	const Int32 PACKETID_SC_RECONNECT_FAIL = -6;

	// ù���� �Ϸ� ��Ŷ(Ŭ��,���� ��ź��ȵ� �غ� ���� ������ �ߴٴ� ��ȣ)
	const Int32 PACKETID_SC_CONNECT_SUCCESS = -7;

    const Int32 PACKETID_SYSTEM_OnClientJoin = -100;
    const Int32 PACKETID_SYSTEM_OnClientLeave = -101;
    const Int32 PACKETID_SYSTEM_OnClientOnline = -102;
    const Int32 PACKETID_SYSTEM_OnClientOffline = -103;
    const Int32 PACKETID_SYSTEM_OnDelayHeartbit = -104;
}