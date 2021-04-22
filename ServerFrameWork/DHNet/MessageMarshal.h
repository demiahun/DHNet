#pragma once

#include "Message.h"
#include "DHTime.h"
#include <map>
#include <atltime.h>

#pragma pack(push,8)

namespace DHNet
{
	inline CMessage& operator >> ( CMessage &a, bool &b )
	{
		Byte value = 0;
		a.Read( value );
		b = value > 0 ? true : false;
		return a;
	}
	inline CMessage& operator<<( CMessage &a, bool &b )
	{
		Byte value = b ? 1 : 0;
		a.Write( value );
		return a;
	}
	inline CMessage& operator<<( CMessage &a, const bool &b )
	{
		Byte value = b ? 1 : 0;
		a.Write( value );
		return a;
	}

	inline CMessage& operator >> ( CMessage &a, Int16 &b )
	{
		a.Read( b );
		return a;
	}
	inline CMessage& operator<<( CMessage &a, Int16 &b )
	{
		a.Write( b );
		return a;
	}
	inline CMessage& operator<<( CMessage &a, const Int16 &b )
	{
		a.Write( b );
		return a;
	}
	inline CMessage& operator >> ( CMessage &a, UInt16 &b )
	{
		a.Read( b );
		return a;
	}
	inline CMessage& operator<<( CMessage &a, UInt16 &b )
	{
		a.Write( b );
		return a;
	}
	inline CMessage& operator<<( CMessage &a, const UInt16 &b )
	{
		a.Write( b );
		return a;
	}
	inline CMessage& operator >> ( CMessage &a, Int32 &b )
	{
		a.Read( b );
		return a;
	}
	inline CMessage& operator<<( CMessage &a, Int32 &b )
	{
		a.Write( b );
		return a;
	}
	inline CMessage& operator<<( CMessage &a, const Int32 &b )
	{
		a.Write( b );
		return a;
	}
	inline CMessage& operator >> ( CMessage &a, UInt32 &b )
	{
		a.Read( b );
		return a;
	}
	inline CMessage& operator<<( CMessage &a, UInt32 &b )
	{
		a.Write( b );
		return a;
	}
	inline CMessage& operator<<( CMessage &a, const UInt32 &b )
	{
		a.Write( b );
		return a;
	}
	inline CMessage& operator >> ( CMessage &a, Int64 &b )
	{
		a.Read( b );
		return a;
	}
	inline CMessage& operator<<( CMessage &a, Int64 &b )
	{
		a.Write( b );
		return a;
	}
	inline CMessage& operator<<( CMessage &a, const Int64 &b )
	{
		a.Write( b );
		return a;
	}
	inline CMessage& operator >> ( CMessage &a, UInt64 &b )
	{
		a.Read( b );
		return a;
	}
	inline CMessage& operator<<( CMessage &a, UInt64 &b )
	{
		a.Write( b );
		return a;
	}
	inline CMessage& operator<<( CMessage &a, const UInt64 &b )
	{
		a.Write( b );
		return a;
	}
	inline CMessage& operator >> ( CMessage &a, float &b )
	{
		a.Read( b );
		return a;
	}
	inline CMessage& operator<<( CMessage &a, float &b )
	{
		a.Write( b );
		return a;
	}
	inline CMessage& operator<<( CMessage &a, const float &b )
	{
		a.Write( b );
		return a;
	}
	inline CMessage& operator >> ( CMessage &a, double &b )
	{
		a.Read( b );
		return a;
	}
	inline CMessage& operator<<( CMessage &a, double &b )
	{
		a.Write( b );
		return a;
	}
	inline CMessage& operator<<( CMessage &a, const double &b )
	{
		a.Write( b );
		return a;
	}
	inline CMessage& operator >> ( CMessage &a, StringA &b )
	{
		a.Read( b );
		return a;
	}
	inline CMessage& operator<<( CMessage &a, StringA &b )
	{
		a.Write( b );
		return a;
	}
	inline CMessage& operator<<( CMessage &a, const StringA &b )
	{
		a.Write( b );
		return a;
	}
	inline CMessage& operator >> ( CMessage &a, StringW &b )
	{
		a.Read( b );
		return a;
	}
	inline CMessage& operator<<( CMessage &a, StringW &b )
	{
		a.Write( b );
		return a;
	}
	inline CMessage& operator<<( CMessage &a, const StringW &b )
	{
		a.Write( b );
		return a;
	}
	inline CMessage& operator >> ( CMessage &a, std::string &b )
	{
		a.Read( b );
		return a;
	}
	inline CMessage& operator<<( CMessage &a, std::wstring &b )
	{
		a.Write( b );
		return a;
	}
	inline CMessage& operator<<( CMessage &a, const std::wstring &b )
	{
		a.Write( b );
		return a;
	}
	inline CMessage& operator >> ( CMessage &a, CDHTime &b )
	{
		Int16 year = 0;
		Int16 month = 0;
		Int16 day = 0;
		Int16 hour = 0;
		Int16 min = 0;
		Int16 sec = 0;
		a >> year ;
		a >> month;
		a >> day;
		a >> hour;
		a >> min;
		a >> sec;

		b.SetDateTime( year, month, day, hour, min, sec, 0 );

		return a;
	}
	inline CMessage& operator << ( CMessage &a, CDHTime &b )
	{
		Int16 year = (Int16)b.GetYear();
		Int16 month = (Int16)b.GetMonth();
		Int16 day = (Int16)b.GetDay();
		Int16 hour = (Int16)b.GetHour();
		Int16 min = (Int16)b.GetMinute();
		Int16 sec = (Int16)b.GetSecond();
		a << year;
		a << month;
		a << day;
		a << hour;
		a << min;
		a << sec;

		return a;
	}
	inline CMessage& operator << ( CMessage &a, const CDHTime &b )
	{
		Int16 year = (Int16)b.GetYear();
		Int16 month = (Int16)b.GetMonth();
		Int16 day = (Int16)b.GetDay();
		Int16 hour = (Int16)b.GetHour();
		Int16 min = (Int16)b.GetMinute();
		Int16 sec = (Int16)b.GetSecond();
		a << year;
		a << month;
		a << day;
		a << hour;
		a << min;
		a << sec;

		return a;
	}

	template<typename T>
	inline CMessage& operator >> ( CMessage &a, std::vector<T> &b )
	{
		Int16 len = 0;
		a >> len;
		b.resize( len );
		if( len < 0 || len > 32760 )
		{
			const char *szT = typeid(T).name();
			DHLOG_ERROR( L"incorrect vector len(read) : packetID(%d), Len(%d), T(%s)", a.GetID(), len, DHNet::StringA2W( szT ).GetString() );
			return a;
		}
		for( int i = 0; i < len; ++i )
		{
			a >> b[i];
		}
		return a;
	}
	template<typename T>
	inline CMessage& operator << ( CMessage &a, std::vector<T> &b )
	{
		Int16 len = (Int16)b.size();

		if( len < 0 || len > 32760 )
		{
			const char *szT = typeid(T).name();
			DHLOG_ERROR( L"incorrect vector len(read) : packetID(%d), Len(%d), T(%s)", a.GetID(), len, DHNet::StringA2W( szT ).GetString() );
			return a;
		}
		if( (a.m_Length + (sizeof(T) * len)) > MAX_PACKET_SIZE )
		{
			const char *szT = typeid(T).name();
			DHLOG_ERROR( L"over MAX_PACKET_SIZE : packetID(%d), Len(%d), T(%s)", a.GetID(), len, DHNet::StringA2W( szT ).GetString() );
			return a;
		}

		a << len;
		for( int i = 0; i < len; ++i )
		{
			if( (a.m_Length + (sizeof( T ))) >= MAX_PACKET_SIZE )
			{
				const char *szT = typeid(T).name();
				DHLOG_ERROR( L"over MAX_PACKET_SIZE : packetID(%d), Len(%d), T(%s)", a.GetID(), len, DHNet::StringA2W( szT ).GetString() );
				return a;
			}

			a << b[i];
		}
		return a;
	}

	template<typename T>
	inline CMessage& operator << ( CMessage &a, const std::vector<T> &b )
	{
		Int16 len = (Int16)b.size();

		if( len < 0 || len > 32760 )
		{
			const char *szT = typeid(T).name();
			DHLOG_ERROR( L"incorrect vector len(read) : packetID(%d), Len(%d), T(%s)", a.GetID(), len, DHNet::StringA2W( szT ).GetString() );
			return a;
		}
		if( (a.m_Length + (sizeof( T ) * len)) > MAX_PACKET_SIZE )
		{
			const char *szT = typeid(T).name();
			DHLOG_ERROR( L"over MAX_PACKET_SIZE : packetID(%d), Len(%d), T(%s)", a.GetID(), len, DHNet::StringA2W( szT ).GetString() );
			return a;
		}

		a << len;
		for( int i = 0; i < len; ++i )
		{
			if( (a.m_Length + (sizeof( T ))) >= MAX_PACKET_SIZE )
			{
				const char *szT = typeid(T).name();
				DHLOG_ERROR( L"over MAX_PACKET_SIZE : packetID(%d), Len(%d), T(%s)", a.GetID(), len, DHNet::StringA2W( szT ).GetString() );
				return a;
			}

			a << b[i];
		}
		return a;
	}

	template<typename K, typename V>
	__forceinline CMessage& operator >> ( CMessage& a, std::map<K,V> &b )
	{
		Int16 len = 0;
		a >> len;
		if( len < 0 || len > 32760 )
		{
			const char *szK = typeid(K).name();
			const char *szV = typeid(V).name();
			DHLOG_ERROR( L"incorrect map len(read) : packetID(%d), Len(%d), K(%s), V(%s)", a.GetID(), len, DHNet::StringA2W( szK ).GetString(), DHNet::StringA2W( szV ).GetString() );
			return a;
		}
		for( int i = 0; i < len; ++i )
		{
			if( (a.m_Length + (sizeof( V ))) >= MAX_PACKET_SIZE )
			{
				const char *szT = typeid(V).name();
				DHLOG_ERROR( L"over MAX_PACKET_SIZE : packetID(%d), Len(%d), T(%s)", a.GetID(), len, DHNet::StringA2W( szT ).GetString() );
				return a;
			}

			K key;
			V val;
			a >> key;
			a >> val;
			b[key] = val;
		}
		return a;
	}
	template<typename K, typename V>
	__forceinline CMessage& operator << ( CMessage& a, std::map<K, V> &b )
	{
		Int16 len = (Int16)b.size();
		a << len;
		for( auto mit : b )
		{
			if( (a.m_Length + (sizeof( V ))) >= MAX_PACKET_SIZE )
			{
				const char *szT = typeid(V).name();
				DHLOG_ERROR( L"over MAX_PACKET_SIZE : packetID(%d), Len(%d), T(%s)", a.GetID(), len, DHNet::StringA2W( szT ).GetString() );
				return a;
			}

			a << mit.first;
			a << mit.second;
		}
		return a;
	}
	template<typename K, typename V>
	__forceinline CMessage& operator << ( CMessage& a, const std::map<K, V> &b )
	{
		Int16 len = (Int16)b.size();
		a << len;
		for( auto mit : b )
		{
			if( (a.m_Length + (sizeof( V ))) >= MAX_PACKET_SIZE )
			{
				const char *szT = typeid(V).name();
				DHLOG_ERROR( L"over MAX_PACKET_SIZE : packetID(%d), Len(%d), T(%s)", a.GetID(), len, DHNet::StringA2W( szT ).GetString() );
				return a;
			}

			a << mit.first;
			a << mit.second;
		}
		return a;
	}
}

#pragma pack(pop)
