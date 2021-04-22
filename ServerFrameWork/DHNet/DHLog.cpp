#include "stdafx.h"
#include "DHLog.h"
#include "sysutil.h"


namespace DHNet
{
	CDHLog GDHLog;

	void CDHLog::Init( const char *szName )
	{
		std::wcout.imbue( std::locale( "korean" ) );

		MakeFolder( StringA2W( szName ) );

		google::InitGoogleLogging( szName );
		StringA strInit = "./";
		strInit += szName;

		FLAGS_log_dir = strInit.GetString();
	}

}
