#pragma once

#include "DHString.h"

#pragma pack(push,8)

struct tagDBTIMESTAMP;
typedef tagDBTIMESTAMP DBTIMESTAMP;

namespace DHNet
{
	class CDHTimeSpan
	{
	public:
		CDHTimeSpan() throw();

		CDHTimeSpan( double dblSpanSrc ) throw();

		CDHTimeSpan( int32_t lDays, int nHours, int nMins, int nSecs, int nMillisecs ) throw();

		enum DateTimeSpanStatus
		{
			valid = 0,
			invalid = 1,    // Invalid span (out of range, etc.)
			null = 2,       // Literally has no value
		};

		double m_span;
		DateTimeSpanStatus m_status;

		void SetStatus( DateTimeSpanStatus status ) throw();
		DateTimeSpanStatus GetStatus() const throw();

		double GetTotalDays() const throw();    // span in days (about -3.65e6 to 3.65e6)

		double GetTotalHours() const throw();   // span in hours (about -8.77e7 to 8.77e6)

		double GetTotalMinutes() const throw(); // span in minutes (about -5.26e9 to 5.26e9)

		double GetTotalSeconds() const throw(); // span in seconds (about -3.16e11 to 3.16e11)

		double GetTotalMilliseconds() const throw();	// span in milliseconds

		int32_t GetDays() const throw();       // component days in span

		int32_t GetHours() const throw();      // component hours in span (-23 to 23)

		int32_t GetMinutes() const throw();    // component minutes in span (-59 to 59)

		int32_t GetSeconds() const throw();    // component seconds in span (-59 to 59)

		int GetMilliseconds() const throw();

		CDHTimeSpan& operator=( double dblSpanSrc ) throw();

		bool operator==( const CDHTimeSpan& dateSpan ) const throw();

		bool operator!=( const CDHTimeSpan& dateSpan ) const throw();

		bool operator<( const CDHTimeSpan& dateSpan ) const throw();

		bool operator>( const CDHTimeSpan& dateSpan ) const throw();

		bool operator<=( const CDHTimeSpan& dateSpan ) const throw();

		bool operator>=( const CDHTimeSpan& dateSpan ) const throw();

		CDHTimeSpan operator+( const CDHTimeSpan& dateSpan ) const throw();

		CDHTimeSpan operator-( const CDHTimeSpan& dateSpan ) const throw();

		CDHTimeSpan& operator+=( const CDHTimeSpan dateSpan ) throw();

		CDHTimeSpan& operator-=( const CDHTimeSpan dateSpan ) throw();

		CDHTimeSpan operator-() const throw();


		/*
		날짜시간 설정
		*/
		void SetDateTimeSpan( int32_t lDays, int nHours, int nMins, int nSecs, int nMillisecs ) throw();

		/*
		날짜시간을 문자열로 만든다.
		*/
		String Format( const NGTCHAR *pFormat ) const;

		/*
		날짜시간을 문자열로 만든다.
		*/
		String Format( uint32_t nID ) const;

		void CheckRange();

	private:
		static const double OLE_DATETIME_HALFSECOND;
	};

	/*
	날짜시간 데이터 타입
	*/
	class CDHTime  // CTime이나 CDateTime, CPnTime은 ATL 등과 이름이 혼동될 수 있으므로
	{
		// Constructors
	public:
		static CDHTime GetCurrTime() throw();

		CDHTime() throw();

		CDHTime( const VARIANT& varSrc ) throw();

		CDHTime( const SYSTEMTIME& systimeSrc ) throw();

		CDHTime( const FILETIME& filetimeSrc ) throw();

		CDHTime( int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec, int nMillisec ) throw();

		CDHTime( uint16_t wDosDate, uint16_t wDosTime ) throw();

		static CDHTime FromDATE( DATE dtSrc );

		CDHTime( const DBTIMESTAMP& dbts ) throw();
		bool GetAsDBTIMESTAMP( DBTIMESTAMP& dbts ) const throw();

		/*
		날짜시간의 유효 상태
		*/
		enum DateTimeStatus
		{
			/*
			에러
			*/
			error = -1,

			/*
			유효
			*/
			valid = 0,

			/*
			잘못된 날짜 (범위 초과 등)
			*/
			invalid = 1,

			/*
			말 그대로 값이 없음.
			*/
			null = 2
		};

		DATE m_dt;
		DateTimeStatus m_status;

		void SetStatus( DateTimeStatus status ) throw();
		DateTimeStatus GetStatus() const throw();

		bool GetAsSystemTime( SYSTEMTIME& sysTime ) const throw();
		bool GetAsUDATE( UDATE& udate ) const throw();

		/*
		연도를 얻는다.
		*/
		int GetYear() const throw();

		/*
		월 값을 얻는다.
		*/
		int GetMonth() const throw();

		/*
		일 값을 얻는다.
		*/
		int GetDay() const throw();

		/*
		시 값을 얻는다.
		*/
		int GetHour() const throw();

		/*
		분 값을 얻는다.
		*/
		int GetMinute() const throw();

		/*
		초 값을 얻는다.
		*/
		int GetSecond() const throw();

		/*
		밀리초 값을 얻는다.
		*/
		int GetMillisecond() const throw();

		/*
		요일 값을 얻는다. (1: 일요일, 2: 월요일...)
		*/
		int GetDayOfWeek() const throw();

		/*
		1년중 몇번째 날인지를 얻는다. (1: 1월 1일)
		*/
		int GetDayOfYear() const throw();

		CDHTime& operator=( const VARIANT& varSrc ) throw();
		CDHTime& operator=(DATE dtSrc) throw();
 		
		CDHTime& operator=(const __time32_t& timeSrc) throw();

		CDHTime& operator=(const __time64_t& timeSrc) throw();

		CDHTime& operator=( const SYSTEMTIME& systimeSrc ) throw();

		CDHTime& operator=( const FILETIME& filetimeSrc ) throw();

		CDHTime& operator=( const UDATE& udate ) throw();

		bool operator==( const CDHTime& date ) const throw();

		bool operator!=( const CDHTime& date ) const throw();

		bool operator<( const CDHTime& date ) const throw();

		bool operator>( const CDHTime& date ) const throw();

		bool operator<=( const CDHTime& date ) const throw();

		bool operator>=( const CDHTime& date ) const throw();

		CDHTime operator+( CDHTimeSpan dateSpan ) const throw();

		CDHTime operator-( CDHTimeSpan dateSpan ) const throw();

		CDHTime& operator+=( CDHTimeSpan dateSpan ) throw();

		CDHTime& operator-=( CDHTimeSpan dateSpan ) throw();

		CDHTimeSpan operator-( const CDHTime& date ) const throw();
		
		int SetDateTime( int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec, int nMillisec ) throw();

		int SetDate( int nYear, int nMonth, int nDay ) throw();

		int SetTime( int nHour, int nMin, int nSec, int nMillisec ) throw();

		bool ParseDateTime( const NGTCHAR* lpszDate, uint32_t dwFlags = 0, LCID lcid = LANG_USER_DEFAULT ) throw();

		String Format( uint32_t dwFlags = 0, LCID lcid = LANG_USER_DEFAULT ) const;

		String	Format( const NGTCHAR* lpszFormat ) const;
		StringW	ToString() const;

		String Format( uint32_t nFormatID ) const;


	protected:
		static double DoubleFromDate( DATE date ) throw();
		static DATE DateFromDouble( double f ) throw();

		void CheckRange();
		BOOL ConvertSystemTimeToVariantTime( const SYSTEMTIME& systimeSrc );

	};

	inline class CDHTime ConvertString2NgTime( StringW &src )
	{
		CDHTime time;
		//## 날짜 문자열은 항상 : 2008-01-22 11:16:00 이런식의 패턴으로 넘어와야한다.
		//### 2016-11-23 처럼 일단위까지 와도 컨버팅을 하자
		if( src.GetLength() < 10 )
			return time;

		//#0. year
		//#1. month
		//#2. day
		//#3. hour
		//#4. min
		//#5. sec
		std::wstring wstrToken[6];
		int iNowToken = 0;

		const WCHAR* pT = src.GetString();

		while( (*pT) != 0 )
		{
			switch( (*pT) )
			{
			case L'-':
			case L' ':
			case L':':
			case L'.':
			{
				++iNowToken;
			}
			break;

			default:
			{
				if( (*pT) >= L'0' && (*pT) <= L'9' )
				{
					if( wstrToken[iNowToken].size() == 0 )
						wstrToken[iNowToken] = (*pT);
					else
						wstrToken[iNowToken] += (*pT);
				}
			}
			}

			if( iNowToken >= 6 )
				break;

			++pT;
		}

        int	iToken[6] = {0,0,0,0,0,0};
		for( int i = 0; i < 6; ++i )
		{
			iToken[i] = 0;

			if( wstrToken[i].empty() )
				continue;

			iToken[i] = _wtoi( wstrToken[i].c_str() );
		}

		//{{ CTime에서 exception발생 안시키도록 밖에서 따로 검사
		if( iToken[0] < 1900 )
		{
			return time;
		}
		if( iToken[1] < 1 || iToken[1] > 12 )
		{
			return time;
		}
		if( iToken[2] < 1 || iToken[2] > 31 )
		{
			return time;
		}
		//}}

		//{{ 초기값 체크 - exception내지 않고 false를 return하도록 함.
		if( iToken[0] == 1970 &&
			iToken[1] == 1 &&
			iToken[2] == 1 &&
			iToken[3] == 9 &&
			iToken[4] == 0 &&
			iToken[5] == 0 )
		{
			// 이런경우에도 false리턴 해야함.
			return time;
		}
		//}}

		time = CDHTime( iToken[0], iToken[1], iToken[2], iToken[3], iToken[4], iToken[5], 0 );

		return time;
	}
	inline class CDHTime ConvertString2NgTime( StringA &src )
	{
		return ConvertString2NgTime( StringA2W( src ) );
	}
	inline class CDHTime ConvertString2NgTime( wchar_t *src )
	{
		StringW str = src;
		return ConvertString2NgTime( str );
	}
	inline class CDHTime ConvertString2NgTime( char *src )
	{
		StringA str = src;
		return ConvertString2NgTime( str );
	}
}

#pragma pack(pop)
