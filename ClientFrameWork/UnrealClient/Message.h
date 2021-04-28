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
		��ȣȭ�� ���� ��Ŷ�� ���� �Ұ��� ����
		���� :
		Message msg = CMessage::Create();
		msg << data;
		msg << data0;
		..
		msg.SetEncrypted( true );	// SendPacket ���� ������ ������ ���ָ� ��ȣȭ �Ǿ� ���۵ȴ�.
		*/
		void	SetRmiContext( Byte v );
		Byte	GetRmiContext();

		/*
		������ ������ �� �̷���� �� ����ؾ� ���� ������
		: ���� �Ŀ��� buffer �� ������ ���̰� �ֱ� ����
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

