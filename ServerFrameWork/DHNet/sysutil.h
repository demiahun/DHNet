#pragma once

#include "DHString.h"

#include <Rpc.h>
#pragma comment( lib, "Rpcrt4.lib" )

namespace DHNet
{
	DWORD GetCurrentThreadID();

	/*
	CPU core ������ ���ؿ´�.
	*/
	DWORD	GetCoreNum();

	/*
	�ش������� �����ϴ°�.?
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
	���� ��ο� sub folder�� �����. �Լ��� sub-folder�� �̸��� �޴´�.
	*/
	inline bool MakeFolder( StringW src )
	{
		std::wstring strFolderPath = CurrentDirectoryW();
		strFolderPath += L"\\";
		strFolderPath += src.GetString();

		if( 0 == ::_wmkdir( strFolderPath.c_str() ) ) return true;  // ������ ���.

		switch( errno )
		{
		case EEXIST:    // �̹� �����ϴ� ���
		return true;
		break;
		default:        // unknown error.
		break;
		}

		return false;
	}

	const wchar_t* GetMsgWSALastError();			// ���� ����.

	/*
	https://docs.microsoft.com/en-us/windows/desktop/api/rpcdce/nf-rpcdce-uuidcreate
	*/
	StringW GetRandGuid();
}

