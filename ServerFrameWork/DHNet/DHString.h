#pragma once

#include <xstring>
#include <cctype>
#include <memory>

#if defined(UNICODE)
#define _PNT(x) L##x
typedef wchar_t NGTCHAR;
#else
#define _PNT(x) x
typedef char NGTCHAR;
#endif

namespace DHNet
{
	template< typename XCHAR >
	class StringT
	{
	private:
	#define STRING_DEC( type ) std::basic_string< type, std::char_traits< type >, std::allocator< type > >
		STRING_DEC( XCHAR ) _str;
	public:
		StringT(){}
		~StringT(){}

		inline StringT( const StringT &src )
		{
			_str = src.GetString();
		}
		inline StringT( const STRING_DEC( XCHAR) &src )
		{
			_str = src;
		}
		StringT( const XCHAR* src )
		{
			_str = src;
		}

		inline bool IsEmpty() const
		{
			return _str.empty();
		}

		inline operator const XCHAR* () const
		{
			return GetString();
		}
		inline const XCHAR* GetString() const
		{
			return _str.c_str();
		}

		int GetLength() const
		{
			return (int)_str.length();
		}

		void SetLength( int length )
		{
			_str.resize( length );
		}

		inline void Append( const StringT &src )
		{
			_str.append( src.GetString() );
		}
		inline void Append( const XCHAR *src )
		{
			_str.append( src );
		}

		inline int Compare( const StringT &src ) const
		{
			return _str.compare( src.GetString() );
		}
		inline int Compare( const XCHAR *src ) const
		{
			return _str.compare( src );
		}

		inline void Format( const char* pszFormat, ... )
		{
			std::string strFormat = pszFormat;
			if( strFormat.length() == 0 || strFormat.length() > 1024 )
				return;

            va_list vaList;
            va_start( vaList, pszFormat );
            int len = _vscprintf_p( pszFormat, vaList ) + 1;
            char *pBuf = (char*)malloc( sizeof( char ) * len );
            if( pBuf )
            {
                vsprintf_s( pBuf, len, pszFormat, vaList );
                va_end( vaList );
                _str = pBuf;
                free( pBuf );
            }
		}
		inline void Format( const wchar_t* pszFormat, ... )
		{
			std::wstring strFormat = pszFormat;
			if( strFormat.length() == 0 || strFormat.length() > 1024 )
				return;

            va_list vaList;
            va_start( vaList, pszFormat );
            int len = _vscwprintf_p( pszFormat, vaList ) + 1;
            wchar_t *pBuf = (wchar_t*)malloc( sizeof( wchar_t ) * len );
            if( pBuf )
            {
                vswprintf_s( pBuf, len, pszFormat, vaList );
                va_end( vaList );
                _str = pBuf;
                free( pBuf );
            }
		}

		/*
		문자열 중 pszOld가 가리키는 문자열을 pszNew 로 바꾼다.
		return 0 : 한글자도 바뀌지 않음
		return int > 0 : 바뀐 글자수
		*/
		int Replace( const StringT pszOld, const StringT pszNew )
		{
			int count = 0;
			size_t start_pos = 0; //string처음부터 검사
			while( (start_pos = _str.find( pszOld, start_pos )) != std::string::npos )  //from을 찾을 수 없을 때까지
			{
				_str.replace( start_pos, pszOld.GetLength(), pszNew );
				start_pos += pszNew.GetLength(); // 중복검사를 피하고 from.length() > to.length()인 경우를 위해서

				count += 1;
			}
			return count;
		}

		inline StringT& MakeUpper()
		{
			for( auto i = _str.begin(); i != _str.end(); ++i )
				*i = (XCHAR)std::toupper( *i );

			return(*this);
		}

		inline StringT& MakeLower()
		{
			for( auto i = _str.begin(); i != _str.end(); ++i )
				*i = (XCHAR)std::tolower( *i );

			return(*this);
		}

		int Find( XCHAR ch, int iStart = 0 ) const throw()
		{
			auto pos = _str.find( ch, iStart );

			return (int)pos;
		}
		int Find( XCHAR *sz ) const throw()
		{
			auto pos = _str.find( sz );

			return (int)pos;
		}
		int Find( StringT &str ) const throw()
		{
			auto pos = _str.find( str.GetString() );

			return (int)pos;
		}

		inline bool operator==( const StringT &src ) const
		{
			return Compare( src ) == 0;
		}
		inline bool operator==( const XCHAR *src ) const
		{
			return Compare( src ) == 0;
		}
		inline bool operator==( const STRING_DEC( XCHAR ) &src ) const	// STRING_DEC 는 : std::string and std::wstring
		{
			return Compare( src.c_str() ) == 0;
		}
		inline bool operator!=( const StringT &src ) const
		{
			return Compare( src ) != 0;
		}
		inline bool operator!=( const XCHAR *src ) const
		{
			return Compare( src ) != 0;
		}
		inline bool operator!=( const STRING_DEC( XCHAR ) &src ) const	// STRING_DEC 는 : std::string and std::wstring
		{
			return Compare( src.c_str() ) != 0;
		}

		inline StringT& operator+=( const StringT& src )
		{
			Append( src );
			return *this;
		}
		inline StringT& operator+=( const XCHAR *src )
		{
			Append( src );
			return *this;
		}
		inline StringT& operator+=( const STRING_DEC( XCHAR ) &src )	// STRING_DEC 는 : std::string and std::wstring
		{
			Append( src.c_str() );
			return *this;
		}

		inline StringT& operator=( const StringT& src )
		{
			_str = src.GetString();
			return *this;
		}
		inline StringT& operator=( const XCHAR *src )
		{
			_str = src;
			return *this;
		}
		inline StringT& operator=( const STRING_DEC( XCHAR ) &src )	// STRING_DEC 는 : std::string and std::wstring
		{
			_str = src;
			return *this;
		}
	};

	/*
	ANSI 캐릭터 문자열
	*/
	typedef StringT<char> StringA;
	/*
	Unicode 캐릭터 문자열
	*/
	typedef StringT<wchar_t> StringW;

#if defined(UNICODE)
	typedef StringW String;
#else
	typedef StringA String;
#endif

	// 스트링 헬퍼
	inline StringW StringA2W( const char *src, UINT codePage = CP_ACP )
	{
		StringW buf;
		if( src == nullptr )
			return buf;
		int len = (int)::strlen( src );
		if( len == 0 || len == -1 )
			return buf;

		int nChars = ::MultiByteToWideChar( codePage, 0, src, len, NULL, 0 );
		if( nChars <= 0 )
			return buf;

		buf.SetLength( nChars );
		::MultiByteToWideChar( codePage, 0, src, len, const_cast<wchar_t*>(buf.GetString()), nChars );

		return buf;
	}
	inline StringW StringA2W( const StringA &src, UINT codePage = CP_ACP )
	{
		return StringA2W( src.GetString(), codePage );
	}
	inline StringW StringA2W( const std::string &src, UINT codePage = CP_ACP )
	{
		return StringA2W( src.c_str(), codePage );
	}
	inline StringA StringW2A( const wchar_t* src, UINT codePage = CP_ACP )
	{
		StringA buf;
		if( src == nullptr )
			return buf;
		int len = (int)::wcslen( src );
		if( len == 0 || len == -1 )
			return buf;

		int nChars = ::WideCharToMultiByte( codePage, 0, src, len, NULL, 0, NULL, NULL );
		if( nChars <= 0 )
			return buf;

		buf.SetLength( nChars );
		::WideCharToMultiByte( codePage, 0, src, len, const_cast<char*>(buf.GetString()), nChars, NULL, NULL );

		return buf;
	}
	inline StringA StringW2A( const StringW &src, UINT codePage = CP_ACP )
	{
		return StringW2A( src.GetString(), codePage );
	}
	inline StringA StringW2A( const std::wstring &src, UINT codePage = CP_ACP )
	{
		return StringW2A( src.c_str(), codePage );
	}
}