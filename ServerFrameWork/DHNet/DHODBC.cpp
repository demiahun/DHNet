#include "stdafx.h"
#include "DHODBC.h"
#include "DHLog.h"

#pragma warning( disable : 4995 )
#pragma warning( disable : 4996 )

namespace DHNet
{
	DHODBC::DHODBC()
	{}


	DHODBC::~DHODBC()
	{
		Destory();
	}

	void DHODBC::Reset()
	{
		SQLFreeStmt( m_hStmt, SQL_CLOSE );
		SQLFreeStmt( m_hStmt, SQL_RESET_PARAMS );
	}

	bool DHODBC::Query( const wchar_t* szFormat_, ... )
	{
		wchar_t     szQuery[MAX_QUERY_SIZE] = {0,};
		va_list     vaList;

		va_start( vaList, szFormat_ );
		vswprintf_s( szQuery, MAX_QUERY_SIZE, szFormat_, vaList );

		std::wstring wstrQuery = szQuery;
		return Query( wstrQuery );
	}

	bool DHODBC::Query( const std::wstring &wstrQuery )
	{
		if( false == IsConnect() )
		{
			Destory();
			if( false == __Connect() )
			{
				DHLOG_ERROR( L"DB와의 연결이 끊어진 상태에서 재접속 실패, 주소 " );
				return false;
			}
		}

		// 호출 전 State 초기화 후 진행합니다...
		Reset();

		SQLRETURN   Ret;

		m_wstrLastQuery = wstrQuery;

		DWORD dwTime = ::GetTickCount();
		// 해당 query에 대한 row가 없는 경우 ret는 SQL_NO_DATA가 return 된다.
		Ret = ::SQLExecDirectW( m_hStmt, (SQLWCHAR*)wstrQuery.c_str(), SQL_NTS );

		// 정상처리 됐을 경우만 최대쿼리 시간을 산출한다
		if( SQLOK( Ret ) )
		{
			DWORD dwTimeGap = ::GetTickCount() - dwTime;

			if( dwTimeGap > 100 )
			{
				DHLOG_ERROR( L"ODBC SP DELAY( %d ms ) : %s", dwTimeGap, wstrQuery.c_str() );
			}
			return true;
		}

		if( Ret == SQL_ERROR || Ret == SQL_INVALID_HANDLE )
		{
			// DB 접속이 off 된 경우 재접속 요청을 하도록 해보자

			// 에러 스테이트를 구하고.
			SQLWCHAR	state[10];
			SQLINTEGER	nativeError;
			SQLWCHAR	szErrorMsg[MAX_PATH];
			memset( szErrorMsg, 0, sizeof( SQLWCHAR )*MAX_PATH );
			SQLSMALLINT	len;

			// Error, warning status info. 값을 얻어 옵니다.
			SQLGetDiagRecW( SQL_HANDLE_STMT, m_hStmt, 1, state, &nativeError, szErrorMsg, MAX_PATH, &len );

			wchar_t* err1 = L"08S01";  // Communication link failure
			wchar_t* err2 = L"24000";  // Invalid cursor state
			wchar_t* err3 = L"01000";  // Disconnect

									   // godsend 커서 오류와 DB연결 안된 오류 두가지 경우 다시 연결
			if( wcscmp( (const wchar_t*)state, err1 ) == 0 || wcscmp( (const wchar_t*)state, err2 ) == 0 || wcscmp( (const wchar_t*)state, err3 ) == 0 )
			{
				DHLOG_ERROR( L"%s : err(%s)", wstrQuery.c_str(), state );

				// 우선 연결을 끊고..
				Destory();

				// 재접속 시도를 진행
				if( true == __Connect() )
				{
					DWORD dwTime = ::GetTickCount();

					// 성공시 다시 쿼리를 요청
					Ret = ::SQLExecDirectW( m_hStmt, (SQLWCHAR*)wstrQuery.c_str(), SQL_NTS );

					if( SQLOK( Ret ) )
					{
						DWORD dwTimeGap = ::GetTickCount() - dwTime;
						if( dwTimeGap > 500 )
						{
							DHLOG_ERROR( L"ODBC SP Delay : %s", wstrQuery.c_str() );
						}

						return true;
					}
				}

				// 또 실패하면 아래에서 오류찍고 그만 ㅠㅠ
				m_bIsConnect = false;
			}

			DHLOG_ERROR( L"ODBC 재접속 실패, DB와의 연결이 끊어짐 : %s", wstrQuery.c_str() );

			// DB서버와의 연결 끊김시에 메일 보내는 기능 추가.
			{
				//StringA strLogMsg;
				//strLogMsg.Format( "DB Connection Fail ODBC Error Code : %s", strState.GetString() );

				//String wstrLogMsg = BLUtil::ConvertCharToWCHAR( strLogMsg.GetString() ).c_str();
				//SendMailInfo( wstrLogMsg );
			}//block
		}

		DBErrorLog( SQL_HANDLE_STMT, m_hStmt, wstrQuery.c_str() );

		return false;
	}


	bool DHODBC::Query( const String &wstrQuery )
	{
		std::wstring query = wstrQuery.GetString();
		return Query( query );
	}

	bool DHODBC::MoveNext()
	{
		m_usColumNum = 0;
		return true;
	}

	// DB의 처리 결과를 받아오기 시작.
	bool DHODBC::BeginFetch( const bool bFetchErrorLog )
	{
		m_usColumNum = 0;
		SQLRETURN   ret = ::SQLFetch( m_hStmt );

		if( !SQLOK( ret ) )
		{
			EndFetch();
			DBErrorLog( SQL_HANDLE_STMT, m_hStmt, NULL );

			return false;
		}

		return true;
	}

	// DB의 처리 결과를 받아옵니다.
	bool DHODBC::Fetch()
	{
		m_usColumNum = 0;
		SQLRETURN ret = ::SQLFetch( m_hStmt );

		if( ret == SQL_NO_DATA )
			EndFetch();

		return SQLOK( ret );
	}

	// Fetch 종료.
	void DHODBC::EndFetch()
	{
		if( SQL_NULL_HSTMT != m_hStmt )
			::SQLCloseCursor( m_hStmt );
	}

	void DHODBC::DBErrorLog( SQLSMALLINT handletype, SQLHANDLE handle, const wchar_t *msg )
	{
		SQLWCHAR	state[10];
		SQLINTEGER	nativeError;
		SQLWCHAR	szErrorMsg[MAX_PATH];
		memset( szErrorMsg, 0, sizeof( SQLWCHAR )*MAX_PATH );
		SQLSMALLINT len;
		SQLRETURN	Ret;

		// Error, warning status info. 값을 얻어 옵니다.
		Ret = ::SQLGetDiagRecW( handletype, handle, 1, state, &nativeError, szErrorMsg, MAX_PATH, &len );
		size_t stLen = wcslen( szErrorMsg );
		if( stLen > 0 )
		{
			// msg가 배열의 사이즈를 넘는 경우가 발생하여 WString으로 변경
			std::wstring wstrPrintMsg( szErrorMsg );
			if( NULL != msg )
			{
				std::wstring wstrMsg( msg );
				wstrPrintMsg += wstrMsg;
			}

			DHLOG_ERROR( L"DBError : %s", wstrPrintMsg.c_str() );
		}
	}

	/***********************************************************************
	데이터 형에 따른 SQL data 를 얻어오는 operator 들 입니다..
	************************************************************************/

	DHODBC& DHODBC::operator >> ( bool& val_ )
	{
		char        val;
		SQLRETURN   ret;

		m_usColumNum++;
		ret = ::SQLGetData( m_hStmt,
							m_usColumNum,
							SQL_C_STINYINT,
							&val,
							sizeof( bool ),
							0 );
		if( !SQLOK( ret ) )
		{
			THROW_INVALID_QUERY();
		} // if

		val_ = !(val == 0);
		return *this;
	}


	DHODBC& DHODBC::operator >> ( char& val_ )
	{
		SQLRETURN   ret;

		m_usColumNum++;
		ret = ::SQLGetData( m_hStmt,
							m_usColumNum,
							SQL_C_STINYINT,
							&val_,
							sizeof( char ),
							0 );
		if( !SQLOK( ret ) )
		{
			THROW_INVALID_QUERY();
		} // if

		return *this;
	}

	DHODBC& DHODBC::operator >> ( wchar_t& val_ )
	{
		SQLRETURN   ret;

		m_usColumNum++;
		ret = ::SQLGetData( m_hStmt,
							m_usColumNum,
							SQL_C_WCHAR,
							&val_,
							sizeof( wchar_t ),
							0 );
		if( !SQLOK( ret ) )
		{
			THROW_INVALID_QUERY();
		} // if

		return *this;
	}

	DHODBC& DHODBC::operator >> ( short& val_ )
	{
		SQLRETURN   ret;

		m_usColumNum++;
		ret = ::SQLGetData( m_hStmt,
							m_usColumNum,
							SQL_C_SHORT,
							&val_,
							sizeof( short ),
							0 );
		if( !SQLOK( ret ) )
		{
			THROW_INVALID_QUERY();
		} // if

		return *this;
	}

	DHODBC& DHODBC::operator >> ( int& val_ )
	{
		SQLRETURN   ret;

		m_usColumNum++;
		ret = ::SQLGetData( m_hStmt,
							m_usColumNum,
							SQL_C_SLONG,
							&val_,
							sizeof( int ),
							0 );
		if( !SQLOK( ret ) )
		{
			THROW_INVALID_QUERY();
		} // if

		return *this;
	}

	DHODBC& DHODBC::operator >> ( UINT& val_ )
	{
		SQLRETURN   ret;

		m_usColumNum++;
		ret = ::SQLGetData( m_hStmt,
							m_usColumNum,
							SQL_C_SLONG,
							&val_,
							sizeof( UINT ),
							0 );
		if( !SQLOK( ret ) )
		{
			THROW_INVALID_QUERY();
		} // if

		return *this;
	}

	DHODBC& DHODBC::operator >> ( LONG& val_ )
	{
		SQLRETURN   ret;

		m_usColumNum++;
		ret = ::SQLGetData( m_hStmt,
							m_usColumNum,
							SQL_C_SLONG,
							&val_,
							sizeof( LONG ),
							0 );
		if( !SQLOK( ret ) )
		{
			THROW_INVALID_QUERY();
		} // if

		return *this;
	}

	DHODBC& DHODBC::operator >> ( UCHAR& val_ )
	{
		SQLRETURN   ret;

		m_usColumNum++;
		ret = ::SQLGetData( m_hStmt,
							m_usColumNum,
							SQL_C_UTINYINT,
							&val_,
							sizeof( UCHAR ),
							0 );
		if( !SQLOK( ret ) )
		{
			THROW_INVALID_QUERY();
		}

		return *this;
	}


	DHODBC& DHODBC::operator >> ( USHORT& val_ )
	{
		SQLRETURN   ret;

		m_usColumNum++;
		ret = ::SQLGetData( m_hStmt,
							m_usColumNum,
							SQL_C_SSHORT,
							&val_,
							sizeof( USHORT ),
							0 );
		if( !SQLOK( ret ) )
		{
			THROW_INVALID_QUERY();
		}

		return *this;
	}


	DHODBC& DHODBC::operator >> ( DWORD& val_ )
	{
		SQLRETURN   ret;

		m_usColumNum++;
		ret = ::SQLGetData( m_hStmt,
							m_usColumNum,
							SQL_C_SLONG,
							&val_,
							sizeof( DWORD ),
							0 );
		if( !SQLOK( ret ) )
		{
			THROW_INVALID_QUERY();
		}

		return *this;
	}

	DHODBC& DHODBC::operator >> ( ULONGLONG& val_ )
	{
		SQLRETURN   ret;

		m_usColumNum++;
		ret = ::SQLGetData( m_hStmt,
							m_usColumNum,
							SQL_C_SBIGINT,
							&val_,
							sizeof( ULONGLONG ),
							0 );
		if( !SQLOK( ret ) )
		{
			THROW_INVALID_QUERY();
		}

		return *this;
	}

	DHODBC& DHODBC::operator >> ( LONGLONG& val_ )
	{
		SQLRETURN   ret;

		m_usColumNum++;
		ret = ::SQLGetData( m_hStmt,
							m_usColumNum,
							SQL_C_SBIGINT,
							&val_,
							sizeof( LONGLONG ),
							0 );
		if( !SQLOK( ret ) )
		{
			THROW_INVALID_QUERY();
		}

		return *this;
	}


	DHODBC& DHODBC::operator >> ( std::string& val_ )
	{
		char acBuffer[MAX_PACKET_SIZE] = {0,};

		*this >> acBuffer;
		val_ = acBuffer;

		return *this;
	}

	DHODBC& DHODBC::operator >> ( char* val_ )
	{
		SQLRETURN   ret;
        SQLLEN      nLen;
        SQLLEN  LenBin;

		m_usColumNum++;
		SQLColAttribute( m_hStmt,
						 m_usColumNum,
						 SQL_DESC_OCTET_LENGTH,
						 0, 0, 0,
						 &nLen );
		ret = ::SQLGetData( m_hStmt,
							m_usColumNum,
							SQL_C_CHAR,
							val_,
							MAX_PACKET_SIZE,
							&LenBin );

		if( LenBin == SQL_NULL_DATA )   // SqlGetData는 읽을 필드가 NULL이더라도 SQL_SUCCESS를 리턴한다.
			return *this;

		if( !SQLOK( ret ) )
		{
			THROW_INVALID_QUERY();
		}

		return *this;
	}

	DHODBC& DHODBC::operator >> ( wchar_t* val_ )
	{
		SQLRETURN   ret;
        SQLLEN      nLen;
        SQLLEN      LenBin;

		m_usColumNum++;
		SQLColAttribute( m_hStmt,
						 m_usColumNum,
						 SQL_DESC_OCTET_LENGTH,
						 0, 0, 0,
						 &nLen );
		ret = ::SQLGetData( m_hStmt,
							m_usColumNum,
							SQL_C_WCHAR,
							val_,
							MAX_PACKET_SIZE,
							&LenBin );

		if( LenBin == SQL_NULL_DATA )   // SqlGetData는 읽을 필드가 NULL이더라도 SQL_SUCCESS를 리턴한다.
			return *this;

		if( !SQLOK( ret ) )
		{
			THROW_INVALID_QUERY();
		} // if

		return *this;
	}

	DHODBC& DHODBC::operator >> ( std::wstring& val_ )
	{
		wchar_t acBuffer[MAX_PACKET_SIZE] = {0,};

		*this >> acBuffer;
		val_ = acBuffer;

		return *this;
	}

	DHODBC& DHODBC::operator >> ( StringW& val_ )
	{
		wchar_t acBuffer[MAX_PACKET_SIZE] = {0,};

		*this >> acBuffer;
		val_ = acBuffer;

		return *this;
	}

	DHODBC& DHODBC::operator >> ( float& val_ )
	{
		SQLRETURN   ret;

		m_usColumNum++;
		ret = ::SQLGetData( m_hStmt,
							m_usColumNum,
							SQL_C_FLOAT,
							&val_,
							sizeof( float ),
							0 );
		if( !SQLOK( ret ) )
		{
			THROW_INVALID_QUERY();
		} // if

		return *this;
	}

	DHODBC& DHODBC::operator >> ( SYSTEMTIME& val_ )
	{
		SQLRETURN   ret;
		TIMESTAMP_STRUCT sqlTime;

		m_usColumNum++;

		ret = ::SQLGetData( m_hStmt,
							m_usColumNum,
							SQL_C_TYPE_TIMESTAMP,
							&sqlTime,
							sizeof( sqlTime ),
							0 );

		if( !SQLOK( ret ) )
		{
			THROW_INVALID_QUERY();
		} // if

		val_.wYear = sqlTime.year;
		val_.wMonth = sqlTime.month;
		val_.wDay = sqlTime.day;
		val_.wHour = sqlTime.hour;
		val_.wMinute = sqlTime.minute;
		val_.wSecond = sqlTime.second;
		val_.wMilliseconds = 0;
		val_.wDayOfWeek = 0;

		return *this;
	}

	DHODBC& DHODBC::FetchString( OUT std::wstring& val )
	{
		SQLRETURN   ret;
        SQLLEN      nLen;
        SQLLEN      LenBin;
		wchar_t acBuffer[MAX_QUERY_SIZE] = {0,};

		m_usColumNum++;
		SQLColAttribute( m_hStmt,
						 m_usColumNum,
						 SQL_DESC_OCTET_LENGTH,
						 0, 0, 0,
						 &nLen );
		ret = ::SQLGetData( m_hStmt,
							m_usColumNum,
							SQL_C_WCHAR,
							acBuffer,
							MAX_QUERY_SIZE,
							&LenBin );

		if( LenBin == SQL_NULL_DATA )   // SqlGetData는 읽을 필드가 NULL이더라도 SQL_SUCCESS를 리턴한다.
			return *this;

		if( !SQLOK( ret ) )
		{
			THROW_INVALID_QUERY();
		} // if

		val = acBuffer;

		return *this;
	}

	DHODBC& DHODBC::operator >> ( CTime& val_ )
	{
		SQLRETURN   ret;
		TIMESTAMP_STRUCT sqlTime;

		m_usColumNum++;

		ret = ::SQLGetData( m_hStmt,
							m_usColumNum,
							SQL_C_TYPE_TIMESTAMP,
							&sqlTime,
							sizeof( sqlTime ),
							0 );

		if( !SQLOK( ret ) )
		{
			THROW_INVALID_QUERY();
		} // if

		val_ = CTime( sqlTime.year, sqlTime.month, sqlTime.day, sqlTime.hour, sqlTime.minute, sqlTime.second );

		return *this;
	}

	DHODBC& DHODBC::operator >> ( CDHTime& val_ )
	{
		SQLRETURN   ret;
		TIMESTAMP_STRUCT sqlTime;

		m_usColumNum++;

		ret = ::SQLGetData( m_hStmt,
							m_usColumNum,
							SQL_C_TYPE_TIMESTAMP,
							&sqlTime,
							sizeof( sqlTime ),
							0 );

		if( !SQLOK( ret ) )
		{
			THROW_INVALID_QUERY();
		} // if

		val_.SetDateTime( sqlTime.year, sqlTime.month, sqlTime.day, sqlTime.hour, sqlTime.minute, sqlTime.second, 0 );

		return *this;
	}

	// ------------------------------------------------------------------------------------------------------------------------

	bool DHODBC::Init( String &strDSN )
	{
		if( strDSN.GetLength() == 0 )
			return false;

		m_strDSN = L"FILEDSN=";
		m_strDSN += strDSN;

		bool bRet = __Connect();

		return bRet;
	}

	bool DHODBC::__Connect( void )
	{
		if( m_strDSN.GetLength() == 0 ) // DSN 파일 주소가 없으면 접속을 못함.
		{
			DHLOG_ERROR( L"DSN 스트링 정보가 없음.!" );
			return false;
		}

		// allocate environment handle.	
		SQLRETURN Ret = ::SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv );
		if( !SQLOK( Ret ) )
		{
			DHLOG_ERROR( L"SQL 환경변수 핸들 생성실패.!" );
			return false;
		}

		// set the ODBC version environment attribute.
		Ret = ::SQLSetEnvAttr( m_hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER );
		if( !SQLOK( Ret ) )
		{
			DHLOG_ERROR( L"SQL 환경변수 속성 설정실패.!" );
			return false;
		}

	#define BUFSIZE 1024
		SQLWCHAR InCon[BUFSIZE];
		SQLWCHAR OutCon[BUFSIZE];
		SQLSMALLINT cbOutCon;

		Ret = ::SQLAllocHandle( SQL_HANDLE_DBC, m_hEnv, &m_hDbc );

		if( !SQLOK( Ret ) )
		{
			DBErrorLog( SQL_HANDLE_ENV, m_hEnv, NULL );
			m_hDbc = SQL_NULL_HDBC;
			m_hStmt = SQL_NULL_HSTMT;
		}
		else
		{
			wsprintfW( (wchar_t*)InCon, L"%s", m_strDSN.GetString() );
			Ret = SQLDriverConnectW( m_hDbc, NULL, (SQLWCHAR*)InCon, BUFSIZE, OutCon, BUFSIZE, &cbOutCon, SQL_DRIVER_NOPROMPT );

			//에러검사
			if( !SQLOK( Ret ) )
			{
				DBErrorLog( SQL_HANDLE_DBC, m_hDbc, NULL );
				SQLFreeHandle( SQL_HANDLE_DBC, m_hDbc );
				m_hDbc = SQL_NULL_HDBC;
				m_hStmt = SQL_NULL_HSTMT;

				return false;
			}
			else
			{
				//명령핸들 할당
				Ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hDbc, &m_hStmt );
				if( !SQLOK( Ret ) )
				{
					DBErrorLog( SQL_HANDLE_DBC, m_hDbc, NULL );
					SQLFreeHandle( SQL_HANDLE_DBC, m_hDbc );
					m_hDbc = SQL_NULL_HDBC;
					m_hStmt = SQL_NULL_HSTMT;
				}
			}
		}

		m_bIsConnect = true;

		return true;
	}

	void DHODBC::Destory()
	{
		m_bIsConnect = false;

		if( SQL_NULL_HSTMT != m_hStmt )
		{
			if( !SQLOK( SQLFreeHandle( SQL_HANDLE_STMT, m_hStmt ) ) )
				printf( "SQLFreeHandle( SQL_HANDLE_STMT, m_hStmt ) Fail!\n" );
			else
				m_hStmt = SQL_NULL_HSTMT;
		}

		if( SQL_NULL_HDBC != m_hDbc )
		{
			SQLDisconnect( m_hDbc );
			if( !SQLOK( SQLFreeHandle( SQL_HANDLE_DBC, m_hDbc ) ) )
				printf( "SQLFreeHandle( SQL_HANDLE_DBC, m_hDbc ) Fail!\n" );
			else
				m_hDbc = SQL_NULL_HDBC;
		}

		if( SQL_NULL_HENV != m_hEnv )
		{
			if( !SQLOK( SQLFreeHandle( SQL_HANDLE_ENV, m_hEnv ) ) )
				printf( "SQLFreeHandle( SQL_HANDLE_ENV, m_hEnv ) Fail!\n" );
			else
				m_hEnv = SQL_NULL_HENV;
		}

		m_usColumNum = 0;
	}
}

#pragma warning( default : 4995 )
#pragma warning( default : 4996 )