#pragma once

#include <sql.h>
#include <sqlext.h>
#include <cassert>
#include <cstring>
#include <exception>
#include <atltime.h>
#include <vector>
#include "DHString.h"
#include "DHPtr.h"
#include "DHTime.h"


#pragma comment(lib, "odbc32.lib")

namespace DHNet
{

	class DHODBC
	{
	public:
	#	define MAX_QUERY_SIZE 8192

	#   define SQLOK(result) (result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO)
	#   define SQLNODATA(result) (result == SQL_NO_DATA)
	#   define THROW_INVALID_QUERY() { StringA strQuery = StringW2A( m_wstrLastQuery ).GetString(); std::cout << __FILE__ << __FUNCTION__ << "Exception : " << strQuery.GetString() << std::endl; throw std::exception("DB Query Exception"); }
	#   define FETCH_DATA( odbc, varlist ) \
	{ \
	try \
	{ \
	odbc >> varlist;\
		} \
		catch( std::exception& ex) \
	{ \
	std::cout << __FILE__ << __FUNCTION__ << ex.what() << std::endl;\
		} \
		catch( ... ) \
	{ \
	std::cout << __FILE__ << __FUNCTION__ << "Unknown exception accur" << std::endl;\
		} \
		}

	public:
		DHODBC();
		~DHODBC();

		bool	Init( String &strDSN );

		/*
		query call
		*/
		bool	Query( const wchar_t* szFormat, ... );
		bool	Query( const std::wstring &wstrQuery );
		bool	Query( const String &wstrQuery );

		void	Reset();

		// ErrorLog
		void    DBErrorLog( IN SQLSMALLINT handletype, IN SQLHANDLE handle, IN const wchar_t *msg );

		// Fetch
		bool	BeginFetch( const bool bFetchErrorLog = true );
		bool	Fetch();
		void	EndFetch();
		bool	MoveNext();
		const std::wstring&
				GetLastQueryW()
		{
			return m_wstrLastQuery;
		}
		const std::string
				GetLastQueryA()
		{
			std::string strLastQuery = StringW2A( m_wstrLastQuery );
			return strLastQuery;
		}

		bool	IsConnect()
		{
			return m_bIsConnect;
		}
		void	Destory( void );	// 연결 되어있는 DB 연결을 종료

									// operator >>    
		DHODBC& operator >> ( bool& val );
		DHODBC& operator >> ( char& val );
		DHODBC& operator >> ( wchar_t& val );
		DHODBC& operator >> ( short& val );
		DHODBC& operator >> ( int& val );
		DHODBC& operator >> ( UINT& val );
		DHODBC& operator >> ( LONG& val );
		DHODBC& operator >> ( UCHAR& val );
		DHODBC& operator >> ( USHORT& val );
		DHODBC& operator >> ( DWORD& val );
		DHODBC& operator >> ( char* val );
		DHODBC& operator >> ( std::string& val );
		DHODBC& operator >> ( float& val );
		DHODBC& operator >> ( CTime& val );
		DHODBC& operator >> ( CDHTime& val );
		DHODBC& operator >> ( wchar_t* val );
		DHODBC& operator >> ( std::wstring& val );
		DHODBC& operator >> ( StringW& val );
		DHODBC& operator >> ( ULONGLONG& val );
		DHODBC& operator >> ( LONGLONG& val );
		DHODBC& operator >> ( SYSTEMTIME& val );

		DHODBC& FetchString( OUT std::wstring& val ); // 4096 byte의 String을 반환한다.

	private:
		bool	__Connect( void );	// DB 접속요청

	public:
		enum
		{
			BLOB_DATA_PARAM_MARKER,     // blob 데이터를 전송하기 위해 사용할 아이디
		};

		USHORT  m_usColumNum = 0;

	private:
		SQLHENV	m_hEnv = SQL_NULL_HENV;
		SQLHDBC m_hDbc = SQL_NULL_HDBC;		// Connection 을 할당 받을 핸들.
		SQLHSTMT
				m_hStmt = SQL_NULL_HSTMT;	// Statement 를 할당 받을 핸들.
		bool	m_bIsConnect = false;

		String	m_strDSN;
		std::wstring
				m_wstrLastQuery;
	};

	REFCOUNT( DHODBC );

	class CQueryHelper
	{
		std::vector< StringW >
				m_vecArgs;
		StringW	m_strQueryHeader;
	public:
		CQueryHelper( wchar_t *wszQueryHeader = L"EXECSP" )
		{
			m_strQueryHeader = wszQueryHeader;
		}
		~CQueryHelper(){}

		void Reset()
		{
			m_vecArgs.resize(0);
		}

		CQueryHelper& operator << ( const bool& v )
		{
			String str;
			str.Format( L"%d", v );
			*this << str;
			return *this;
		}
		CQueryHelper& operator << ( const int& v )
		{
			String str;
			str.Format( L"%d", v );
			*this << str;
			return *this;
		}
		CQueryHelper& operator << ( const Int64& v )
		{
			String str;
			str.Format( L"%I64d", v );
			*this << str;
			return *this;
		}
		CQueryHelper& operator << ( const char *v )
		{
			StringW wstr = StringA2W( v );
			m_vecArgs.push_back( wstr );
			return *this;
		}
		CQueryHelper& operator << ( const wchar_t *v )
		{
			m_vecArgs.push_back( v );
			return *this;
		}
		CQueryHelper& operator << ( const String& v )
		{
			m_vecArgs.push_back( v );
			return *this;
		}
		CQueryHelper& operator << ( const CDHTime& v )
		{
			StringW str;
			str.Format( L"%s", v.Format( L"%Y-%m-%d %H:%M:%S" ).GetString() );
			*this << str;
			return *this;
		}
        CQueryHelper& operator << ( const float& v )
        {
            String str;
            str.Format( L"%f", v );
            *this << str;
            return *this;
        }

		template<typename T>
		void	AddArg( T &v )
		{
			*this << v;
		}

		String	GetResult()
		{
			String result;
			result.Format( L"{call dbo.%s(", m_strQueryHeader.GetString() );

			for( UInt32 i = 0; i < m_vecArgs.size(); ++i )
			{
				result += L"N'";
				result += m_vecArgs[i].GetString();
				result += L"'";

				if( i < (m_vecArgs.size() - 1) )
					result += L",";
			}

			result += L")}";
			return result;
		}
	};

}

