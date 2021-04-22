// EchoServer.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include <conio.h>
#include <iostream>
#include <exception>

#include "EchoServer.h"


int main()
{
	GDHLog.Init( "EchoServer" );

	CEchoServerPtr spServer = CEchoServerPtr( new CEchoServer() );

	try
	{
		spServer->Start();

		while( true )
		{
			if( _kbhit() )
			{
				int ch = _getch();

				// 서버 종료
				if( ch == 'e' || ch == 'E' )
					break;

				if( ch == 'i' || ch == 'I' )
				{
					DHLOG_ERROR( L"[info] socket count(%d)", GDHNetServer->GetSocketCount() );
					DHLOG_ERROR( L"[info] IOBuffer size(%d), freeSize(%d)", GMessagePool.GetSize(), GMessagePool.GetFreeSize() );
				}
			}

			::Sleep( 10 );
		}
	}
	catch( std::exception &e )
	{
		puts( e.what() );
	}

	spServer->End();

    return 0;
}

