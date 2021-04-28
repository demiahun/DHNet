#pragma once

#include "CoreMinimal.h"
#include "NgIOBuffer.h"
#include "BasicType.h"

namespace DHNet
{
	class CMessage : public IOBuffer
	{
	public:
		CMessage()
		{
			Clear();
		}
		virtual ~CMessage()
		{}

		virtual void Clear() override;

		static CMessage* Create();

		Int32	GetLength();
		void	WriteEnd();

		void	SetID( Int32 id );
		Int32	GetID();

		/*
		Sequence Number
		*/
		void	SetSequenceNum( Int32 sn );
		Int32	GetSequenceNum();

		/*
		암호화를 통해 패킷을 전송 할건지 설정
		사용법 :
		Message msg = CMessage::Create();
		msg << data;
		msg << data0;
		..
		msg.SetEncrypted( true );	// SendPacket 전에 언제든 설정만 해주면 암호화 되어 전송된다.
		*/
		void	SetRmiContext( Byte v );
		Byte	GetRmiContext();

		/*
		데이터 셋팅이 다 이루어진 후 사용해야 정상 동작함
		: 셋팅 후여야 buffer 에 데이터 길이가 있기 때문
		*/
		void	CopyTo( CMessage *msg );

	public:
		void	Read( Byte &v );
		void	Read( Int16 &v );
		void	Read( UInt16 &v );
		void	Read( Int32 &v );
		void	Read( UInt32 &v );
		void	Read( Int64 &v );
		void	Read( UInt64 &v );
		void	Read( float &v );
		void	Read( double &v );
		void	Read( FString &v );

		void	Write( Byte &v );
		void	Write( const Byte &v );
		void	Write( Int16 &v );
		void	Write( const Int16 &v );
		void	Write( UInt16 &v );
		void	Write( const UInt16 &v );
		void	Write( Int32 &v );
		void	Write( const Int32 &v );
		void	Write( UInt32 &v );
		void	Write( const UInt32 &v );
		void	Write( Int64 &v );
		void	Write( const Int64 &v );
		void	Write( UInt64 &v );
		void	Write( const UInt64 &v );
		void	Write( float &v );
		void	Write( const float &v );
		void	Write( double &v );
		void	Write( const double &v );
		void	Write( const char* v );
		void	Write( const FString &v );

	private:
		template<typename T>
		void	_Read( T &v );
		void	_ReadString();
		template<typename T>
		void	_Write( T &v );

	private:
		char	BufferString[MAX_PACKET_SIZE] = {0,};
	};

	extern IOBufferPool<CMessage>	GMessagePool;
}

