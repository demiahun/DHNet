#include "stdafx.h"
#include "sysutil.h"
#include "DHlog.h"

namespace DHNet
{

	DWORD GetCurrentThreadID()
	{
		return ::GetCurrentThreadId();
	}

	DWORD GetCoreNum()
	{
		SYSTEM_INFO sysinfo;
		GetSystemInfo( &sysinfo );
		return sysinfo.dwNumberOfProcessors;
	}

	bool FileExists( const char* fileName )
	{
		struct stat stFileInfo;
		int iRet;

		iRet = stat( fileName, &stFileInfo );
		if( iRet == 0 )
		{
			return true;
		}
		return false;
	}

	static const int LOCAL_MAX_PATH = 1024;
	wchar_t WSAMsg[LOCAL_MAX_PATH] = {0};

	const wchar_t* GetMsgWSALastError()
	{
		try
		{
			FormatMessageW( FORMAT_MESSAGE_FROM_SYSTEM, NULL, ::WSAGetLastError(),
							GetSystemDefaultLangID()/*MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)*/,
							WSAMsg, LOCAL_MAX_PATH, NULL );

            size_t uiSize = min( LOCAL_MAX_PATH - 1, wcslen( WSAMsg ) - 1 );

			WSAMsg[uiSize] = L'\0';   // 문자열의 마지막에 있는 개행(\n)을 없앰.
		}
		catch( std::exception &ex )
		{
			DHLOG_ERROR( L"Exception : %s", StringA2W( ex.what() ).GetString() );
		}

		return WSAMsg;
	}

	DHNet::StringW GetRandGuid()
	{
		UUID id;
		// Guid 생성
		if( RPC_S_OK != ::UuidCreate( &id ) )
		{
			id = GUID_NULL;
			DHLOG_ERROR( L"fail UuidCreta.!" );
		}

		// Guid 문자열로 변경
		wchar_t wszIID[129];
		int r = ::StringFromGUID2( id, wszIID, 128 );

		StringW strValue = wszIID;
		strValue.Replace( L"{", L"" );
		strValue.Replace( L"}", L"" );

		return strValue;
	}

}
