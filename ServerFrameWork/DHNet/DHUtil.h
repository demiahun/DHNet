#pragma once

#include <vector>
#include "DHString.h"

#if (_WIN32_WINNT >= 0x0600)
#include <WS2tcpip.h>	// 최소지원 OS : vista 이상 / Server 2008 이상
#endif

#pragma warning( disable : 4995 )
#pragma warning( disable : 4996 )

namespace DHNet
{
	/*
	std::vector 을 빨리 지우는 기능
	vector 내용중 delete 항목과 last 항목의 값을 바꾸고 last 항목만 삭제 하므로서 erase 시 수행되는 메모리 카피비용을 줄이는 방식
	*/
	template <class Container>
	inline typename Container::iterator EraseUnorderly( Container& container, typename Container::iterator iter )
	{
		typename Container::iterator iterLast = container.end();
		iterLast--;
		if( iterLast == iter )
		{
			return	container.erase( iter );
		}//if
		*iter = *iterLast;
		Container::size_type dist = iter - container.begin();
		container.erase( iterLast );
		return	container.begin() + dist;
	}
    template<typename T>
    inline std::vector<T>& operator+=( std::vector<T>& _Left, const std::vector<T>& _Right )
    {
        std::copy( _Right.begin(), _Right.end(), std::back_inserter( _Left ) );
        return _Left;
    }
	/*
	호스트가 갖고 있는 로컬 IP 주소를 모두 얻어냅니다.
	*/
	inline void GetLocalIPs( std::vector< String > &output )
	{
		char szHostName[256] = {0,};
		PHOSTENT pHostInfo = NULL;

		if( ::gethostname( szHostName, sizeof( szHostName ) ) != 0 )
		{
			return;
		}

		pHostInfo = ::gethostbyname( szHostName );
		if( pHostInfo == NULL )
		{
			return;
		}

		for( int i = 0; NULL != pHostInfo->h_addr_list[i]; ++i )
		{
			StringA ip = inet_ntoa( *(struct in_addr *)pHostInfo->h_addr_list[i] );
			output.push_back( StringA2W( ip ) );
		}
	}
	inline void GetIPsbyHostname( IN String strHostName, OUT std::vector< String > &vecIPs )
	{
		PHOSTENT pHostInfo = NULL;
		pHostInfo = ::gethostbyname( StringW2A( strHostName ).GetString() );
		if( pHostInfo == NULL )
		{
			return;
		}

		for( int i = 0; NULL != pHostInfo->h_addr_list[i]; ++i )
		{
			StringA ip = inet_ntoa( *(struct in_addr *)pHostInfo->h_addr_list[i] );
			vecIPs.push_back( StringA2W( ip ) );
		}
	}
	inline String GetLocalIP()
	{
		std::vector< String > vecIP;
		GetLocalIPs( vecIP );
		String ret = vecIP.size() > 0 ? vecIP[0] : String();
		return ret;
	}

	/*
	현재 실행되는 파일명을 알려줌
	*/
	inline String GetFileNamebyModule( void )
	{
		WCHAR szBuffer[MAX_PATH];

		int iPathLength = ::GetModuleFileName( NULL, szBuffer, MAX_PATH );
		if( 0 >= iPathLength )
			return std::wstring();

		WCHAR szDrive[MAX_PATH] = L"";
		WCHAR szDir[MAX_PATH] = L"";
		WCHAR szFileName[MAX_PATH] = L"";
		WCHAR szExt[MAX_PATH] = L"";

		::_wsplitpath_s( szBuffer, szDrive, szDir, szFileName, szExt );

		return szFileName;
	}

	inline bool RunExecute( String & wstrFile )
	{
		HANDLE hProc[2];
		STARTUPINFO si;
		DWORD dwExitCode = 0, dwRet = 0;
		PROCESS_INFORMATION pi;
		::ZeroMemory( &si, sizeof( si ) );
		si.cb = sizeof( si );

		WCHAR wszFile[MAX_PATH] = L"";
		::wcscpy_s( wszFile, wstrFile.GetString() );
		if( ::CreateProcess( NULL, wszFile, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ) == TRUE )
		{
			if( ::GetExitCodeProcess( pi.hProcess, &dwExitCode ) )
			{
				// 아직 설치 중인가.?
				if( dwExitCode == STILL_ACTIVE )
				{
					hProc[0] = pi.hProcess;
					dwRet = ::WaitForMultipleObjects( 1, hProc, FALSE, INFINITE );
				}
			}
			::CloseHandle( pi.hProcess );
			::CloseHandle( pi.hThread );

			return true;
		}

		return false;
	}
}

#pragma warning( default : 4995 )
#pragma warning( default : 4996 )