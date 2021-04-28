#include "Message.h"



namespace DHNet
{
	IOBufferPool<CMessage>	GMessagePool;

	void CMessage::Clear()
	{
		IOBuffer::Clear();
		m_Length = HEADSIZE;	// 데이터는 Head 이후 부터라 시작값을 넣어줌
	}

	DHNet::CMessage* CMessage::Create()
	{
		return GMessagePool.Alloc();
	}

	Int32 CMessage::GetLength()
	{
		Int32 value = 0;
		FMemory::Memcpy( &value, m_buffer + HEAD_POS_PACKET_SIZE, sizeof( Int32 ) );
		return value;
	}
	void CMessage::WriteEnd()
	{
		FMemory::Memcpy( m_buffer + HEAD_POS_PACKET_SIZE, &m_Length, sizeof( Int32 ) );
	}

	void CMessage::SetID( Int32 id )
	{
		FMemory::Memcpy( m_buffer + HEAD_POS_PACKET_ID, &id, sizeof( Int32 ) );
	}
	Int32 CMessage::GetID()
	{
		Int32 value = 0;
		FMemory::Memcpy( &value, m_buffer + HEAD_POS_PACKET_ID, sizeof( Int32 ) );
		return value;
	}

	void CMessage::SetSequenceNum( Int32 sn )
	{
		FMemory::Memcpy( m_buffer + HEAD_POS_SEQUENCE_NUM, &sn, sizeof( Int32 ) );
	}
	Int32 CMessage::GetSequenceNum()
	{
		Int32 value = 0;
		FMemory::Memcpy( &value, m_buffer + HEAD_POS_SEQUENCE_NUM, sizeof( Int32 ) );
		return value;
	}

	void CMessage::SetRmiContext( Byte v )
	{
		FMemory::Memcpy( m_buffer + HEAD_POS_RMICONTEXT, &v, sizeof( Byte ) );
	}

	Byte CMessage::GetRmiContext()
	{
		Byte value = 0;
		FMemory::Memcpy( &value, m_buffer + HEAD_POS_RMICONTEXT, sizeof( Byte ) );

		return value;
	}

	void CMessage::CopyTo( CMessage *msg )
	{
		msg->m_Length = GetLength();
		FMemory::Memcpy( msg->m_buffer, m_buffer, MAX_PACKET_SIZE );
	}

	void CMessage::Read( Byte &v )
	{
		_Read( v );
	}

	void CMessage::Read( Int16 &v )
	{
		_Read( v );
	}

	void CMessage::Read( UInt16 &v )
	{
		_Read( v );
	}

	void CMessage::Read( Int32 &v )
	{
		_Read( v );
	}

	void CMessage::Read( UInt32 &v )
	{
		_Read( v );
	}

	void CMessage::Read( Int64 &v )
	{
		_Read( v );
	}

	void CMessage::Read( UInt64 &v )
	{
		_Read( v );
	}

	void CMessage::Read( float &v )
	{
		_Read( v );
	}

	void CMessage::Read( double &v )
	{
		_Read( v );
	}

	void CMessage::Read( FString &v )
	{
		_ReadString();
		v = UTF8_TO_TCHAR( BufferString );
	}


	void CMessage::Write( Byte &v )
	{
		_Write( v );
	}
	void CMessage::Write( const Byte &v )
	{
		_Write( v );
	}

	void CMessage::Write( Int16 &v )
	{
		_Write( v );
	}
	void CMessage::Write( const Int16 &v )
	{
		_Write( v );
	}

	void CMessage::Write( UInt16 &v )
	{
		_Write( v );
	}
	void CMessage::Write( const UInt16 &v )
	{
		_Write( v );
	}

	void CMessage::Write( Int32 &v )
	{
		_Write( v );
	}
	void CMessage::Write( const Int32 &v )
	{
		_Write( v );
	}

	void CMessage::Write( UInt32 &v )
	{
		_Write( v );
	}
	void CMessage::Write( const UInt32 &v )
	{
		_Write( v );
	}

	void CMessage::Write( Int64 &v )
	{
		_Write( v );
	}
	void CMessage::Write( const Int64 &v )
	{
		_Write( v );
	}

	void CMessage::Write( UInt64 &v )
	{
		_Write( v );
	}
	void CMessage::Write( const UInt64 &v )
	{
		_Write( v );
	}

	void CMessage::Write( float &v )
	{
		_Write( v );
	}
	void CMessage::Write( const float &v )
	{
		_Write( v );
	}

	void CMessage::Write( double &v )
	{
		_Write( v );
	}
	void CMessage::Write( const double &v )
	{
		_Write( v );
	}

	void CMessage::Write( const char* v )
	{
		if( v == nullptr )
			return;

		STRING_LENGTH_DATA_TYPE len = FPlatformString::Strlen( v );//::strlen( v );
		if( len < 0 )
			len = 0;

		if( (m_Length + len + sizeof( STRING_LENGTH_DATA_TYPE )) > MAX_PACKET_SIZE )
		{
			//NGLOG_ERROR( L"over MAX_PACKET_SIZE : packetID(%d)", GetID() );
			return;
		}

		Write( len );
		if( len > 0 )
		{
			FMemory::Memcpy( m_buffer + m_Length, v, len );
		}
		m_Length += len;
	}

	void CMessage::Write( const FString &v )
	{
		auto value = TCHAR_TO_UTF8(*v);
		Write( value );
	}


	template<typename T>
	void CMessage::_Read( T &v )
	{
		if( (m_Length + sizeof( T )) > MAX_PACKET_SIZE )
		{
			//const char *szT = typeid(T).name();
			//NGLOG_ERROR( L"read over MAX_PACKET_SIZE : packetID(%d), T(%s)", GetID(), DHNet::StringA2W( szT ).GetString() );
			return;
		}

		FMemory::Memcpy( &v, m_buffer + m_Length, sizeof( T ) );
		m_Length += sizeof( T );
	}

	void CMessage::_ReadString()
	{
		STRING_LENGTH_DATA_TYPE len = 0;
		_Read( len );
		if( len < 0 )
		{
			//NGLOG_ERROR( L"incorrect string len : packetID(%d), Len(%d)", GetID(), len );
			return;
		}

		if( m_Length + len > MAX_PACKET_SIZE )
		{
			//NGLOG_ERROR( L"string read over MAX_PACKET_SIZE : packetID(%d)", GetID() );
			return;
		}

		FMemory::Memzero( BufferString, MAX_PACKET_SIZE );
		FMemory::Memcpy( BufferString, m_buffer + m_Length, len );
		m_Length += len;
	}

	template<typename T>
	void CMessage::_Write( T &v )
	{
		if( (m_Length + sizeof( T )) > MAX_PACKET_SIZE )
		{
			//const char *szT = typeid(T).name();
			//NGLOG_ERROR( L"over MAX_PACKET_SIZE : packetID(%d), T(%s)", GetID(), DHNet::StringA2W( szT ).GetString() );
			return;
		}

		FMemory::Memcpy( m_buffer + m_Length, &v, sizeof( T ) );
		m_Length += sizeof( T );
	}
}