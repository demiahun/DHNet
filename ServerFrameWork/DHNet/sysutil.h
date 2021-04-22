#pragma once

#include "DHString.h"

#include <Rpc.h>
#pragma comment( lib, "Rpcrt4.lib" )

namespace DHNet
{
	DWORD GetCurrentThreadID();

	/*
	CPU core 개수를 구해온다.
	*/
	DWORD	GetCoreNum();

	/*
	해당파일이 존재하는가.?
	*/
	bool	FileExists( const char* fileName );

	inline StringA CurrentDirectoryA()
	{
		char szCurrentDir[MAX_PATH] = "";
		::GetCurrentDirectoryA( MAX_PATH, szCurrentDir );

		return szCurrentDir;
	}

	inline StringW CurrentDirectoryW()
	{
		WCHAR wszCurrentDir[MAX_PATH] = L"";
		::GetCurrentDirectoryW( MAX_PATH, wszCurrentDir );

		return wszCurrentDir;
	}

	/*
	현재 경로에 sub folder를 만든다. 함수는 sub-folder의 이름만 받는다.
	*/
	inline bool MakeFolder( StringW src )
	{
		std::wstring strFolderPath = CurrentDirectoryW();
		strFolderPath += L"\\";
		strFolderPath += src.GetString();

		if( 0 == ::_wmkdir( strFolderPath.c_str() ) ) return true;  // 생성된 경우.

		switch( errno )
		{
		case EEXIST:    // 이미 존재하는 경우
		return true;
		break;
		default:        // unknown error.
		break;
		}

		return false;
	}

	const wchar_t* GetMsgWSALastError();			// 오류 정보.

	/*
	https://docs.microsoft.com/en-us/windows/desktop/api/rpcdce/nf-rpcdce-uuidcreate
	*/
	StringW GetRandGuid();
}

