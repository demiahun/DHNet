#pragma once

#include "CoreMinimal.h"
#include "Message.h"
#include "MessageMarshal.h"



namespace DHNet
{
	inline void CompressToMessage( CMessage *msg )
	{
		TArray<uint8> compressedData;

		Int32 CompressedSize = FCompression::CompressMemoryBound( ECompressionFlags::COMPRESS_ZLIB, msg->GetLength() - HEADSIZE );

		if( CompressedSize >= MAX_PACKET_SIZE )
		{
			UE_LOG( LogTemp, Error, TEXT( "overflow packet size : PacketID(%d), msgLength(%d), compressBound(%d)" ), msg->GetID(), msg->GetLength(), CompressedSize );
			return;
		}
		compressedData.SetNum( CompressedSize );

		if( FCompression::CompressMemory( ECompressionFlags::COMPRESS_ZLIB, compressedData.GetData(), CompressedSize, msg->m_buffer + HEADSIZE, msg->GetLength() - HEADSIZE ) == false )
		{
			UE_LOG( LogTemp, Error, TEXT( "failed CompressMemory : PacketID(%d), msgLength(%d), compressBound(%d)" ), msg->GetID(), msg->GetLength(), CompressedSize );
			return;
		}

		Int32 sizeOrg = msg->GetLength() - HEADSIZE;

		msg->m_Length = HEADSIZE;
		*msg << sizeOrg;	// 압축전 크기를 넣어서 받는 쪽에서 검사한다.(*msg << sizeOrg 처리시 msg->m_Length 가 int 만큼 증가한다)

		FMemory::Memcpy( msg->m_buffer + msg->m_Length, compressedData.GetData(), CompressedSize );
		msg->m_Length += CompressedSize;
		msg->WriteEnd();
	}

	inline void UncompressToMessage( CMessage *msg )
	{
		if( (msg->GetLength() - HEADSIZE) <= 0 )
			return;

		TArray<uint8> UncompressedData;

		Int32 sizeOrg = 0;
		*msg >> sizeOrg;

		UncompressedData.SetNum( sizeOrg );

		if( FCompression::UncompressMemory( ECompressionFlags::COMPRESS_ZLIB, UncompressedData.GetData(), sizeOrg, msg->m_buffer + HEADSIZE + sizeof(Int32), msg->GetLength() - HEADSIZE - sizeof(Int32) ) == false )
		{
			UE_LOG( LogTemp, Error, TEXT( "failed UncompressMemory : PacketID(%d), msgLength(%d), sizeOrg(%d)" ), msg->GetID(), msg->GetLength(), sizeOrg );
			return;
		}

		FMemory::Memcpy( msg->m_buffer + HEADSIZE, UncompressedData.GetData(), sizeOrg );
		msg->m_Length = (HEADSIZE + sizeOrg);
		msg->WriteEnd();

		msg->m_Length = HEADSIZE;
	}
}