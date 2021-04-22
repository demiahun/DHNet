#pragma once

#include "zip/zlib.h"
#include "MessageMarshal.h"
#include "DHLog.h"

namespace DHNet
{
	inline void CompressToMessage( CMessage *msg, Byte *bufferTemp )
	{
		if( (msg->GetLength() - HEADSIZE) <= 0 )
			return;

		DWORD size = compressBound( msg->GetLength() - HEADSIZE );

		if( size >= MAX_PACKET_SIZE )
		{
			DHLOG_ERROR( L"overflow packet size : msgLength(%d), compressBound(%d)", msg->GetLength(), size );
			return;
		}

		int ret = compress2( bufferTemp, &size, (Byte*)(msg->m_buffer + HEADSIZE), msg->GetLength() - HEADSIZE, 1 );

		if( ret != Z_OK )
		{
			DHLOG_ERROR( L"Zip compress fail : ret(%d), packetID(%d)", ret, msg->GetID() );
			return;
		}

		int sizeOrg = msg->GetLength() - HEADSIZE;

		msg->m_Length = HEADSIZE;
		*msg << sizeOrg;	// 압축전 크기를 넣어서 받는 쪽에서 검사한다.(*msg << sizeOrg 처리시 msg->m_Length 가 int 만큼 증가한다)

		::memcpy( msg->m_buffer + msg->m_Length, bufferTemp, size );
		msg->m_Length += size;
		msg->WriteEnd();
	}

	inline void UncompressToMessage( CMessage *msg, Byte *bufferTemp )
	{
		if( (msg->GetLength() - HEADSIZE) <= 0 )
			return;

		int sizeOrg = 0;
		*msg >> sizeOrg;

		DWORD sizeRet = sizeOrg;
		int ret = uncompress( bufferTemp, &sizeRet, (Byte*)(msg->m_buffer + HEADSIZE + sizeof(int)), (msg->GetLength() - HEADSIZE - sizeof(int)) );

		if( ret != Z_OK )
		{
			DHLOG_ERROR( L"Zip uncompress fail : ret(%d), packetID(%d)", ret, msg->GetID() );
			return;
		}

		if( sizeRet != sizeOrg )
		{
			DHLOG_ERROR( L"uncompress size fail : sizeRet(%d), sizeOrg(%d), packetID(%d)", sizeRet, sizeOrg, msg->GetID() );
			return;
		}

		::memcpy( msg->m_buffer + HEADSIZE, bufferTemp, sizeRet );
		msg->m_Length = (HEADSIZE + sizeRet);
		msg->WriteEnd();

		msg->m_Length = HEADSIZE;
	}
}

