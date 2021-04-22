#pragma once

#define GOOGLE_GLOG_DLL_DECL
#define GLOG_NO_ABBREVIATED_SEVERITIES

#include "glog/logging.h"
#include "DHString.h"

namespace DHNet
{
	/*
	console 컬러 설정 키
	*/
	enum BL_COLOR_TYPE
	{
		BCT_BLACK,
		BCT_DARK_BLUE,
		BCT_DARK_GREEN,
		BCT_DARK_SKY_BLUE,
		BCT_DARK_RED,
		BCT_DARK_VOILET,
		BCT_DARK_YELLOW,
		BCT_WHITE,
		BCT_GRAY,
		BCT_BLUE,
		BCT_GREEN,
		BCT_SKY_BLUE,
		BCT_RED,
		BCT_VOILET,
		BCT_YELLOW,
	};

	/*
	Console 컬러 문자열 출력을 위한 함수
	*/
	inline void SetColorConsoleFont( BL_COLOR_TYPE eColor )
	{
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), (WORD)eColor );
	}

	enum LOG_LEVEL
	{
		LL_INFO = 0,
		LL_WARN,
		LL_ERROR,
		LL_OFF,

		LL_END,
	};

	class CDHLog
	{
	public:
		CDHLog(){}
		~CDHLog(){}

		void Init( const char *szName );

		LOG_LEVEL
			GetLevel()
		{
			return m_eLevel;
		}
		void	SetLevel( IN LOG_LEVEL eLevel )
		{
			m_eLevel = eLevel;

			switch( m_eLevel )
			{
			case LL_OFF:
				{
					SetColorConsoleFont( BCT_GREEN );
					::printf( "LOG LEVEL CHANGE : OFF == \n" );
					SetColorConsoleFont( BCT_WHITE );
				}
				break;
			case LL_INFO:
				{
					SetColorConsoleFont( BCT_GREEN );
					::printf( "LOG LEVEL CHANGE : INFO == \n" );
					SetColorConsoleFont( BCT_WHITE );
				}
				break;
			case LL_WARN:
				{
					SetColorConsoleFont( BCT_GREEN );
					::printf( "LOG LEVEL CHANGE : WARN == \n" );
					SetColorConsoleFont( BCT_WHITE );
				}
				break;
			case LL_ERROR:
				{
					SetColorConsoleFont( BCT_GREEN );
					::printf( "LOG LEVEL CHANGE : ERROR == \n" );
					SetColorConsoleFont( BCT_WHITE );
				}
				break;
			case LL_END:
				break;
			default:
				break;
			}
		}

	protected:
		LOG_LEVEL
			m_eLevel = LL_INFO;
	};

	extern CDHLog GDHLog;

}

inline void ConvertLogMsg( const wchar_t *function, const wchar_t *args, DHNet::StringA &strOut )
{
	DHNet::StringW strArgs, strMsg;

	strArgs = args;

	strMsg = function;
	strMsg += L" ";
	strMsg += strArgs += L"\n";

	strOut = DHNet::StringW2A( strMsg );
}

inline void ConvertLogMsgStringA( const char *function, const char *args, DHNet::StringA &strOut )
{
    DHNet::StringA strArgs, strMsg;

    strArgs = args;

    strMsg = function;
    strMsg += " ";
    strMsg += strArgs += "\n";

    strOut = strMsg;
}

// 구글 로그의 단계는 INFO(0) -> WARN(1) -> ERROR(2) -> FATAL(3) 이며
//# minloglevel( int ) 로 출력 단계를 설정 할수 있다.
#define DHLOG_ERROR(...) {\
	if( DHNet::LL_ERROR >= DHNet::GDHLog.GetLevel() )\
	{\
		DHNet::StringW str;\
		DHNet::StringA strMsg;\
		str.Format( __VA_ARGS__ );\
		DHNet::SetColorConsoleFont( DHNet::BCT_RED );\
		ConvertLogMsg( __FUNCTIONW__, str.GetString(), strMsg );\
		LOG( ERROR ) << strMsg.GetString();\
		DHNet::SetColorConsoleFont( DHNet::BCT_WHITE );\
	}\
}
#define DHLOG_WARN(...) {\
	if( DHNet::LL_WARN >= DHNet::GDHLog.GetLevel() )\
	{\
		DHNet::StringW str;\
		DHNet::StringA strMsg;\
		str.Format( __VA_ARGS__ );\
		DHNet::SetColorConsoleFont( DHNet::BCT_YELLOW );\
		ConvertLogMsg( __FUNCTIONW__, str.GetString(), strMsg );\
		::printf_s( "%s", strMsg.GetString() );\
		LOG( WARNING ) << strMsg.GetString();\
		DHNet::SetColorConsoleFont( DHNet::BCT_WHITE );\
	}\
}
#define DHLOG_INFO(...) {\
	if( DHNet::LL_INFO >= DHNet::GDHLog.GetLevel() )\
	{\
		DHNet::StringW str;\
		DHNet::StringA strMsg;\
		str.Format( __VA_ARGS__ );\
		DHNet::SetColorConsoleFont( DHNet::BCT_GREEN );\
		ConvertLogMsg( __FUNCTIONW__, str.GetString(), strMsg );\
		::printf_s( "%s", strMsg.GetString() );\
		LOG( INFO ) << strMsg.GetString();\
		DHNet::SetColorConsoleFont( DHNet::BCT_WHITE );\
	}\
}
#define DHLOG_FORCE_INFO(...) {\
	if( DHNet::LL_ERROR >= DHNet::GDHLog.GetLevel() )\
	{\
		DHNet::StringW str;\
		DHNet::StringA strMsg;\
		str.Format( __VA_ARGS__ );\
		DHNet::SetColorConsoleFont( DHNet::BCT_SKY_BLUE );\
		ConvertLogMsg( __FUNCTIONW__, str.GetString(), strMsg );\
		LOG( ERROR ) << strMsg.GetString();\
		DHNet::SetColorConsoleFont( DHNet::BCT_WHITE );\
	}\
}
#define DHLOG_ERROR_STRINGA(...) {\
	if( DHNet::LL_ERROR >= DHNet::GDHLog.GetLevel() )\
	{\
		DHNet::StringA str;\
		DHNet::StringA strMsg;\
		str.Format( __VA_ARGS__ );\
		DHNet::SetColorConsoleFont( DHNet::BCT_RED );\
		ConvertLogMsgStringA( __FUNCTION__, str.GetString(), strMsg );\
		LOG( ERROR ) << strMsg.GetString();\
		DHNet::SetColorConsoleFont( DHNet::BCT_WHITE );\
	}\
}