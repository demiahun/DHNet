#pragma once

#pragma warning(disable : 4996)

#include <fstream>
#include "Json/json.h"
#include "sysutil.h"
#include "DHLog.h"
#include "DHString.h"

class CJsonHelper
{
public:
	CJsonHelper();
	~CJsonHelper();

	static const int BUFFER_SIZE = (1024 * 1024 * 10);

	static bool DoFile( const char *_FileName, Json::Value &root )
	{
		FILE *fp = nullptr;
		::fopen_s( &fp, _FileName, "rb" );

		if( fp == nullptr )
			return false;

		static char buffer[BUFFER_SIZE];

		::ZeroMemory( buffer, BUFFER_SIZE );

		size_t fileLength = ::fread( buffer, 1, BUFFER_SIZE, fp );

		::fclose( fp );

		std::string strBuffer = buffer;

		Json::Reader reader;
		if( reader.parse( strBuffer, root ) == false )
			return false;

		return true;
	}
	static bool DoFile( const char *_FileName, std::string &reader )
	{
		FILE *fp = nullptr;
		::fopen_s( &fp, _FileName, "rb" );

		if( fp == nullptr )
			return false;

		static char buffer[BUFFER_SIZE];

		::ZeroMemory( buffer, BUFFER_SIZE );

		size_t fileLength = ::fread( buffer, 1, BUFFER_SIZE, fp );

		::fclose( fp );

		reader = buffer;

		return true;
	}
	static bool DoFileToTable( const wchar_t *_FileName, Json::Value &_Root )
	{
		DHNet::StringA strFileName = DHNet::StringW2A( _FileName );

		if( DHNet::FileExists( strFileName.GetString() ) == false )
		{
			DHLOG_ERROR( L"file not exist : %s", _FileName );
			return false;
		}

		std::string strTemp;
		if( CJsonHelper::DoFile( strFileName.GetString(), strTemp ) == false )
		{
			DHLOG_ERROR( L"Json DoFile Fail : %s", _FileName );
			return false;
		}
		std::string strBuffer = "{\"table\":" + strTemp + "}";

		Json::Reader reader;
		if( reader.parse( strBuffer, _Root ) == false )
		{
			DHLOG_ERROR( L"Json parse Fail : %s", _FileName );
			return false;
		}

		if( _Root.isMember( "table" ) == false )
		{
			DHLOG_ERROR( L"Json not member(table) : %s", _FileName );
			return false;
		}

		return true;
	}

    static bool DoStringTable( const std::string strData, Json::Value &_Root )
    {
        std::string strBuffer = "{\"table\":" + strData + "}";

        Json::Reader reader;
        if( reader.parse( strBuffer, _Root ) == false )
        {
            DHLOG_ERROR( L"Json parse Fail" );
            return false;
        }

        if( _Root.isMember( "table" ) == false )
        {
            DHLOG_ERROR( L"Json not member(table)" );
            return false;
        }

        return true;
    }
};

