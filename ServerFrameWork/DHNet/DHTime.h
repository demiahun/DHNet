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
		��¥�ð� ����
		*/
		void SetDateTimeSpan( int32_t lDays, int nHours, int nMins, int nSecs, int nMillisecs ) throw();

		/*
		��¥�ð��� ���ڿ��� �����.
		*/
		String Format( const NGTCHAR *pFormat ) const;

		/*
		��¥�ð��� ���ڿ��� �����.
		*/
		String Format( uint32_t nID ) const;

		void CheckRange();

	private:
		static const double OLE_DATETIME_HALFSECOND;
	};

	/*
	��¥�ð� ������ Ÿ��
	*/
	class CDHTime  // CTime�̳� CDateTime, CPnTime�� ATL ��� �̸��� ȥ���� �� �����Ƿ�
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
		��¥�ð��� ��ȿ ����
		*/
		enum DateTimeStatus
		{
			/*
			����
			*/
			error = -1,

			/*
			��ȿ
			*/
			valid = 0,

			/*
			�߸��� ��¥ (���� �ʰ� ��)
			*/
			invalid = 1,

			/*
			�� �״�� ���� ����.
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
		������ ��´�.
		*/
		int GetYear() const throw();

		/*
		�� ���� ��´�.
		*/
		int GetMonth() const throw();

		/*
		�� ���� ��´�.
		*/
		int GetDay() const throw();

		/*
		�� ���� ��´�.
		*/
		int GetHour() const throw();

		/*
		�� ���� ��´�.
		*/
		int GetMinute() const throw();

		/*
		�� ���� ��´�.
		*/
		int GetSecond() const throw();

		/*
		�и��� ���� ��´�.
		*/
		int GetMillisecond() const throw();

		/*
		���� ���� ��´�. (1: �Ͽ���, 2: ������...)
		*/
		int GetDayOfWeek() const throw();

		/*
		1���� ���° �������� ��´�. (1: 1�� 1��)
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
		//## ��¥ ���ڿ��� �׻� : 2008-01-22 11:16:00 �̷����� �������� �Ѿ�;��Ѵ�.
		//### 2016-11-23 ó�� �ϴ������� �͵� �������� ����
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

		//{{ CTime���� exception�߻� �Ƚ�Ű���� �ۿ��� ���� �˻�
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

		//{{ �ʱⰪ üũ - exception���� �ʰ� false�� return�ϵ��� ��.
		if( iToken[0] == 1970 &&
			iToken[1] == 1 &&
			iToken[2] == 1 &&
			iToken[3] == 9 &&
			iToken[4] == 0 &&
			iToken[5] == 0 )
		{
			// �̷���쿡�� false���� �ؾ���.
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
