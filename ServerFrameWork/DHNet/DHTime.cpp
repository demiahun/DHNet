#include "stdafx.h"
#include "DHTime.h"
#include "BasicType.h"


#include <time.h>
#include <oledb.h>
#include <tchar.h>
#include <math.h>
#include <assert.h>

namespace DHNet
{
	namespace _CONST_VALUE_
	{
		const int maxTimeBufferSize = 128;
		const int maxDaysInSpan = 3615897L;
		const TCHAR* szInvalidDateTime = _PNT( "Invalid DateTime" );
		const TCHAR* szInvalidDateTimeSpan = _PNT( "Invalid DateTimeSpan" );
	}

	CDHTimeSpan::CDHTimeSpan() throw() : m_span( 0 ), m_status( valid )
	{}

	CDHTimeSpan::CDHTimeSpan( double dblSpanSrc ) throw() : m_span( dblSpanSrc ), m_status( valid )
	{
		CheckRange();
	}

	CDHTimeSpan::CDHTimeSpan( int32_t lDays, int nHours, int nMins, int nSecs, int nMillisecs ) throw()
	{
		SetDateTimeSpan( lDays, nHours, nMins, nSecs, nMillisecs );
	}

	void CDHTimeSpan::SetStatus( DateTimeSpanStatus status ) throw()
	{
		m_status = status;
	}

	CDHTimeSpan::DateTimeSpanStatus CDHTimeSpan::GetStatus() const throw()
	{
		return m_status;
	}

	__declspec(selectany) const double
		CDHTimeSpan::OLE_DATETIME_HALFSECOND =
		1.0 / (2.0 * (60.0 * 60.0 * 24.0));

	double CDHTimeSpan::GetTotalDays() const throw()
	{
		assert( GetStatus() == valid );
		return (double)LONGLONG( m_span + (m_span < 0 ?
								 -OLE_DATETIME_HALFSECOND : OLE_DATETIME_HALFSECOND) );
	}

	double CDHTimeSpan::GetTotalHours() const throw()
	{
		assert( GetStatus() == valid );
		return (double)LONGLONG( (m_span + (m_span < 0 ?
								 -OLE_DATETIME_HALFSECOND : OLE_DATETIME_HALFSECOND)) * 24 );
	}

	double CDHTimeSpan::GetTotalMinutes() const throw()
	{
		assert( GetStatus() == valid );
		return (double)LONGLONG( (m_span + (m_span < 0 ?
								 -OLE_DATETIME_HALFSECOND : OLE_DATETIME_HALFSECOND)) * (24 * 60) );
	}

	double CDHTimeSpan::GetTotalSeconds() const throw()
	{
		assert( GetStatus() == valid );
		return (double)LONGLONG( (m_span + (m_span < 0 ?
								 -OLE_DATETIME_HALFSECOND : OLE_DATETIME_HALFSECOND)) * (24 * 60 * 60) );
	}

	double CDHTimeSpan::GetTotalMilliseconds() const throw()
	{
		assert( GetStatus() == valid );
		return (double)LONGLONG( (m_span + (m_span < 0 ?
								 -OLE_DATETIME_HALFSECOND : OLE_DATETIME_HALFSECOND)) * (24 * 60 * 60 * 1000) );
	}

	int32_t CDHTimeSpan::GetDays() const throw()
	{
		assert( GetStatus() == valid );
		return int32_t( GetTotalDays() );
	}

	int32_t CDHTimeSpan::GetHours() const throw()
	{
		return int32_t( GetTotalHours() ) % 24;
	}

	int32_t CDHTimeSpan::GetMinutes() const throw()
	{
		return int32_t( GetTotalMinutes() ) % 60;
	}

	int32_t CDHTimeSpan::GetSeconds() const throw()
	{
		return int32_t( GetTotalSeconds() ) % 60;
	}

	int CDHTimeSpan::GetMilliseconds() const throw()
	{
		return int( GetTotalMilliseconds() ) % 1000;
	}

	CDHTimeSpan& CDHTimeSpan::operator=( double dblSpanSrc ) throw()
	{
		m_span = dblSpanSrc;
		m_status = valid;
		CheckRange();
		return *this;
	}

	bool CDHTimeSpan::operator==( const CDHTimeSpan& dateSpan ) const throw()
	{
		if( GetStatus() == dateSpan.GetStatus() )
		{
			if( GetStatus() == valid )
				return (m_span == dateSpan.m_span);

			return (GetStatus() == null);
		}

		return false;
	}

	bool CDHTimeSpan::operator!=( const CDHTimeSpan& dateSpan ) const throw()
	{
		return !operator==( dateSpan );
	}

	bool CDHTimeSpan::operator<( const CDHTimeSpan& dateSpan ) const throw()
	{
		assert( GetStatus() == valid );
		assert( dateSpan.GetStatus() == valid );
		if( (GetStatus() == valid) && (GetStatus() == dateSpan.GetStatus()) )
			return m_span < dateSpan.m_span;

		return false;
	}

	bool CDHTimeSpan::operator>( const CDHTimeSpan& dateSpan ) const throw()
	{
		assert( GetStatus() == valid );
		assert( dateSpan.GetStatus() == valid );
		if( (GetStatus() == valid) && (GetStatus() == dateSpan.GetStatus()) )
			return m_span > dateSpan.m_span;

		return false;
	}

	bool CDHTimeSpan::operator<=( const CDHTimeSpan& dateSpan ) const throw()
	{
		return operator<( dateSpan ) || operator==( dateSpan );
	}

	bool CDHTimeSpan::operator>=( const CDHTimeSpan& dateSpan ) const throw()
	{
		return operator>( dateSpan ) || operator==( dateSpan );
	}

	CDHTimeSpan CDHTimeSpan::operator+( const CDHTimeSpan& dateSpan ) const throw()
	{
		CDHTimeSpan dateSpanTemp;

		// If either operand Null, result Null
		if( GetStatus() == null || dateSpan.GetStatus() == null )
		{
			dateSpanTemp.SetStatus( null );
			return dateSpanTemp;
		}

		// If either operand Invalid, result Invalid
		if( GetStatus() == invalid || dateSpan.GetStatus() == invalid )
		{
			dateSpanTemp.SetStatus( invalid );
			return dateSpanTemp;
		}

		// Add spans and validate within legal range
		dateSpanTemp.m_span = m_span + dateSpan.m_span;
		dateSpanTemp.CheckRange();

		return dateSpanTemp;
	}

	CDHTimeSpan CDHTimeSpan::operator-( const CDHTimeSpan& dateSpan ) const throw()
	{
		CDHTimeSpan dateSpanTemp;

		// If either operand Null, result Null
		if( GetStatus() == null || dateSpan.GetStatus() == null )
		{
			dateSpanTemp.SetStatus( null );
			return dateSpanTemp;
		}

		// If either operand Invalid, result Invalid
		if( GetStatus() == invalid || dateSpan.GetStatus() == invalid )
		{
			dateSpanTemp.SetStatus( invalid );
			return dateSpanTemp;
		}

		// Subtract spans and validate within legal range
		dateSpanTemp.m_span = m_span - dateSpan.m_span;
		dateSpanTemp.CheckRange();

		return dateSpanTemp;
	}

	CDHTimeSpan& CDHTimeSpan::operator+=( const CDHTimeSpan dateSpan ) throw()
	{
		assert( GetStatus() == valid );
		assert( dateSpan.GetStatus() == valid );
		*this = *this + dateSpan;
		CheckRange();
		return *this;
	}

	CDHTimeSpan& CDHTimeSpan::operator-=( const CDHTimeSpan dateSpan ) throw()
	{
		assert( GetStatus() == valid );
		assert( dateSpan.GetStatus() == valid );
		*this = *this - dateSpan;
		CheckRange();
		return *this;
	}

	CDHTimeSpan CDHTimeSpan::operator-() const throw()
	{
		return -this->m_span;
	}

	void CDHTimeSpan::SetDateTimeSpan( int32_t lDays, int nHours, int nMins, int nSecs, int nMillisecs ) throw()
	{
		// Set date span by breaking into fractional days (all input ranges valid)
		m_span = lDays + ((double)nHours) / 24 + ((double)nMins) / (24 * 60) +
			((double)nSecs) / (24 * 60 * 60) + ((double)nMillisecs) / (24 * 60 * 60 * 1000);
		m_status = valid;
		CheckRange();
	}

	void CDHTimeSpan::CheckRange()
	{
		if( m_span < -_CONST_VALUE_::maxDaysInSpan || m_span > _CONST_VALUE_::maxDaysInSpan )
			m_status = invalid;
	}

	CDHTime CDHTime::GetCurrTime() throw()
	{
		SYSTEMTIME st;
		//GetSystemTime( &st );
		::GetLocalTime( &st );

		CDHTime tmTemp;
		tmTemp.m_status = tmTemp.ConvertSystemTimeToVariantTime( st ) ? valid : invalid;
		CDHTime time( tmTemp.GetYear(), tmTemp.GetMonth(), tmTemp.GetDay(), tmTemp.GetHour(), tmTemp.GetMinute(), tmTemp.GetSecond(), 0 );

		return time;
	}

	CDHTime::CDHTime( const VARIANT& varSrc ) throw() :
		m_dt( 0 ), m_status( valid )
	{
		*this = varSrc;
	}


	CDHTime CDHTime::FromDATE( DATE dtSrc )
	{
		CDHTime ret;
		ret.m_dt = dtSrc;
		ret.m_status = valid;
		return ret;
	}

	CDHTime::CDHTime() throw() :
		m_dt( 0 ), m_status( valid )
	{}

	CDHTime::CDHTime( const SYSTEMTIME& systimeSrc ) throw() :
		m_dt( 0 ), m_status( valid )
	{
		*this = systimeSrc;
	}

	CDHTime::CDHTime( const FILETIME& filetimeSrc ) throw() :
		m_dt( 0 ), m_status( valid )
	{
		*this = filetimeSrc;
	}

	CDHTime::CDHTime( int nYear, int nMonth, int nDay,
					  int nHour, int nMin, int nSec, int nMsec ) throw()
	{
		SetDateTime( nYear, nMonth, nDay, nHour, nMin, nSec, nMsec );
	}

	CDHTime::CDHTime( uint16_t wDosDate, uint16_t wDosTime ) throw()
	{
		m_status = ::DosDateTimeToVariantTime( wDosDate, wDosTime, &m_dt ) ?
			valid : invalid;
	}

	void CDHTime::SetStatus( DateTimeStatus status ) throw()
	{
		m_status = status;
	}

	CDHTime::DateTimeStatus CDHTime::GetStatus() const throw()
	{
		return m_status;
	}

	bool VariantTimeToSystemTimeMs( const double dt, SYSTEMTIME& sysTime )
	{
		//http://www.codeproject.com/Articles/17576/SystemTime-to-VariantTime-with-Milliseconds
		double dFraction = dt - (int)dt;

		double hours;
		hours = (dFraction - (int)dFraction) * 24;

		double minutes;
		minutes = (hours - (int)hours) * 60;

		double seconds;
		seconds = (minutes - (int)minutes) * 60;

		double ms;
		ms = (seconds - (int)seconds) * 1000.0 + 0.5;

		if( ms < 1.0 || ms > 999.999 )
			ms = 0;

		double wMilliseconds = (WORD)ms / static_cast<double>(24.0 * 60.0 * 60.0 * 1000.0);
		int retVal = VariantTimeToSystemTime( dt - wMilliseconds, &sysTime );
		sysTime.wMilliseconds = (WORD)ms;

		return (retVal != 0);
	}

	bool SystemTimeToVariantTimeMs( const SYSTEMTIME& systime, double* dt )
	{
		// http://www.codeproject.com/Articles/17576/SystemTime-to-VariantTime-with-Milliseconds
		double dMillsondsPerDay;

		SystemTimeToVariantTime( const_cast<SYSTEMTIME*>(&systime), dt );
		dMillsondsPerDay = static_cast<double>(systime.wMilliseconds) / static_cast<double>(24 * 60 * 60 * 1000);
		(*dt) += dMillsondsPerDay;

		return true;
	}

	bool CDHTime::GetAsSystemTime( SYSTEMTIME& sysTime ) const throw()
	{
		VariantTimeToSystemTimeMs( m_dt, sysTime );
		return GetStatus() == valid;
	}

	bool CDHTime::GetAsUDATE( UDATE &udate ) const throw()
	{
		return SUCCEEDED( ::VarUdateFromDate( m_dt, 0, &udate ) );
	}

	int CDHTime::GetYear() const throw()
	{
		SYSTEMTIME st;
		return GetAsSystemTime( st ) ? st.wYear : error;
	}

	int CDHTime::GetMonth() const throw()
	{
		SYSTEMTIME st;
		return GetAsSystemTime( st ) ? st.wMonth : error;
	}

	int CDHTime::GetDay() const throw()
	{
		SYSTEMTIME st;
		return GetAsSystemTime( st ) ? st.wDay : error;
	}

	int CDHTime::GetHour() const throw()
	{
		SYSTEMTIME st;
		return GetAsSystemTime( st ) ? st.wHour : error;
	}

	int CDHTime::GetMinute() const throw()
	{
		SYSTEMTIME st;
		return GetAsSystemTime( st ) ? st.wMinute : error;
	}

	int CDHTime::GetSecond() const throw()
	{
		SYSTEMTIME st;
		return GetAsSystemTime( st ) ? st.wSecond : error;
	}

	int CDHTime::GetMillisecond() const throw()
	{
		SYSTEMTIME st;
		return GetAsSystemTime( st ) ? st.wMilliseconds : error;
	}

	int CDHTime::GetDayOfWeek() const throw()
	{
		SYSTEMTIME st;
		return GetAsSystemTime( st ) ? st.wDayOfWeek + 1 : error;
	}

	int CDHTime::GetDayOfYear() const throw()
	{
		UDATE udate;
		return GetAsUDATE( udate ) ? udate.wDayOfYear : error;
	}

	CDHTime& CDHTime::operator=( const VARIANT& varSrc ) throw()
	{
		if( varSrc.vt != VT_DATE )
		{
			VARIANT varDest;
			varDest.vt = VT_EMPTY;
			if( SUCCEEDED( ::VariantChangeType( &varDest, const_cast<VARIANT *>(&varSrc), 0, VT_DATE ) ) )
			{
				m_dt = varDest.date;
				m_status = valid;
			}
			else
				m_status = invalid;
		}
		else
		{
			m_dt = varSrc.date;
			m_status = valid;
		}

		return *this;
	}

	CDHTime& CDHTime::operator=( DATE dtSrc ) throw()
	{
		m_dt = dtSrc;
		m_status = valid;
		return *this;
	}

	CDHTime& CDHTime::operator=( const __time32_t& timeSrc ) throw()
	{
		return operator=( static_cast<__time64_t>(timeSrc) );
	}

	CDHTime& CDHTime::operator=( const __time64_t& timeSrc ) throw()
	{
		struct tm dumpTime;
		errno_t err = _localtime64_s( &dumpTime, &timeSrc );
		if( err != 0 )
		{
			m_status = invalid;
			return *this;
		}

		SYSTEMTIME systemTime;
		systemTime.wYear = (WORD)(1900 + dumpTime.tm_year);
		systemTime.wMonth = (WORD)(1 + dumpTime.tm_mon);
		systemTime.wDayOfWeek = (WORD)dumpTime.tm_wday;
		systemTime.wDay = (WORD)dumpTime.tm_mday;
		systemTime.wHour = (WORD)dumpTime.tm_hour;
		systemTime.wMinute = (WORD)dumpTime.tm_min;
		systemTime.wSecond = (WORD)dumpTime.tm_sec;
		systemTime.wMilliseconds = 0;

		m_status = ConvertSystemTimeToVariantTime( systemTime ) == TRUE ? valid : invalid;

		return *this;
	}

	CDHTime &CDHTime::operator=( const SYSTEMTIME &systimeSrc ) throw()
	{
		m_status = ConvertSystemTimeToVariantTime( systimeSrc ) ? valid : invalid;
		return *this;
	}

	CDHTime &CDHTime::operator=( const FILETIME &filetimeSrc ) throw()
	{
		FILETIME ftl;
		SYSTEMTIME st;

		m_status = ::FileTimeToLocalFileTime( &filetimeSrc, &ftl ) &&
			::FileTimeToSystemTime( &ftl, &st ) &&
			ConvertSystemTimeToVariantTime( st ) ? valid : invalid;

		return *this;
	}

	inline BOOL PnConvertSystemTimeToVariantTime( const SYSTEMTIME& systimeSrc, double* pVarDtTm )
	{
		assert( pVarDtTm != NULL );
		//Convert using ::SystemTimeToVariantTime and store the result in pVarDtTm then
		//convert variant time back to system time and compare to original system time.	
		//		BOOL ok = ::SystemTimeToVariantTime(const_cast<SYSTEMTIME*>(&systimeSrc), pVarDtTm);
		BOOL ok = SystemTimeToVariantTimeMs( systimeSrc, pVarDtTm );
		SYSTEMTIME sysTime;
		::ZeroMemory( &sysTime, sizeof( SYSTEMTIME ) );

		ok = ok && VariantTimeToSystemTimeMs( *pVarDtTm, sysTime );
		ok = ok && (systimeSrc.wYear == sysTime.wYear &&
					 systimeSrc.wMonth == sysTime.wMonth &&
					 systimeSrc.wDay == sysTime.wDay &&
					 systimeSrc.wHour == sysTime.wHour &&
					 systimeSrc.wMinute == sysTime.wMinute &&
					 systimeSrc.wSecond == sysTime.wSecond &&
					 systimeSrc.wMilliseconds == sysTime.wMilliseconds);

		return ok;
	}

	BOOL CDHTime::ConvertSystemTimeToVariantTime( const SYSTEMTIME& systimeSrc )
	{
		return PnConvertSystemTimeToVariantTime( systimeSrc, &m_dt );
	}
	CDHTime &CDHTime::operator=( const UDATE &udate ) throw()
	{
		m_status = (S_OK == VarDateFromUdate( (UDATE*)&udate, 0, &m_dt )) ? valid : invalid;

		return *this;
	}

	bool CDHTime::operator==( const CDHTime& date ) const throw()
	{
		if( GetStatus() == date.GetStatus() )
		{
			if( GetStatus() == valid )
				return(m_dt == date.m_dt);

			return (GetStatus() == null);
		}
		return false;

	}

	bool CDHTime::operator!=( const CDHTime& date ) const throw()
	{
		return !operator==( date );
	}

	bool CDHTime::operator<( const CDHTime& date ) const throw()
	{
		assert( GetStatus() == valid );
		assert( date.GetStatus() == valid );
		if( (GetStatus() == valid) && (GetStatus() == date.GetStatus()) )
			return(DoubleFromDate( m_dt ) < DoubleFromDate( date.m_dt ));

		return false;
	}

	bool CDHTime::operator>( const CDHTime& date ) const throw()
	{
		assert( GetStatus() == valid );
		assert( date.GetStatus() == valid );
		if( (GetStatus() == valid) && (GetStatus() == date.GetStatus()) )
			return(DoubleFromDate( m_dt ) > DoubleFromDate( date.m_dt ));

		return false;
	}

	bool CDHTime::operator<=( const CDHTime& date ) const throw()
	{
		return operator<( date ) || operator==( date );
	}

	bool CDHTime::operator>=( const CDHTime& date ) const throw()
	{
		return operator>( date ) || operator==( date );
	}

	CDHTime CDHTime::operator+( CDHTimeSpan dateSpan ) const throw()
	{
		assert( GetStatus() == valid );
		assert( dateSpan.GetStatus() == valid );
		return(CDHTime::FromDATE( DateFromDouble( DoubleFromDate( m_dt ) + dateSpan.m_span ) ));
	}

	CDHTime CDHTime::operator-( CDHTimeSpan dateSpan ) const throw()
	{
		assert( GetStatus() == valid );
		assert( dateSpan.GetStatus() == valid );
		return CDHTime::FromDATE( DateFromDouble( DoubleFromDate( m_dt ) - dateSpan.m_span ) );
	}

	CDHTime& CDHTime::operator+=( CDHTimeSpan dateSpan ) throw()
	{
		assert( GetStatus() == valid );
		assert( dateSpan.GetStatus() == valid );
		m_dt = DateFromDouble( DoubleFromDate( m_dt ) + dateSpan.m_span );
		return(*this);
	}

	CDHTime& CDHTime::operator-=( CDHTimeSpan dateSpan ) throw()
	{
		assert( GetStatus() == valid );
		assert( dateSpan.GetStatus() == valid );
		m_dt = DateFromDouble( DoubleFromDate( m_dt ) - dateSpan.m_span );
		return(*this);
	}

	CDHTimeSpan CDHTime::operator-( const CDHTime& date ) const throw()
	{
		assert( GetStatus() == valid );
		assert( date.GetStatus() == valid );
		return DoubleFromDate( m_dt ) - DoubleFromDate( date.m_dt );
	}

	int CDHTime::SetDateTime( int nYear, int nMonth, int nDay,
							  int nHour, int nMin, int nSec, int nMillisec ) throw()
	{
		SYSTEMTIME st;
		::ZeroMemory( &st, sizeof( SYSTEMTIME ) );

		st.wYear = uint16_t( nYear );
		st.wMonth = uint16_t( nMonth );
		st.wDay = uint16_t( nDay );
		st.wHour = uint16_t( nHour );
		st.wMinute = uint16_t( nMin );
		st.wSecond = uint16_t( nSec );
		st.wMilliseconds = uint16_t( nMillisec );

		m_status = ConvertSystemTimeToVariantTime( st ) ? valid : invalid;
		return m_status;
	}

	int CDHTime::SetDate( int nYear, int nMonth, int nDay ) throw()
	{
		return SetDateTime( nYear, nMonth, nDay, 0, 0, 0, 0 );
	}

	int CDHTime::SetTime( int nHour, int nMin, int nSec, int nMillisec ) throw()
	{
		// Set date to zero date - 12/30/1899
		return SetDateTime( 1899, 12, 30, nHour, nMin, nSec, nMillisec );
	}

	double CDHTime::DoubleFromDate( DATE date ) throw()
	{
		double fTemp;

		// No problem if positive
		if( date >= 0 )
		{
			return(date);
		}

		// If negative, must convert since negative dates not continuous
		// (examples: -1.25 to -.75, -1.50 to -.50, -1.75 to -.25)
		fTemp = ceil( date );

		return(fTemp - (date - fTemp));
	}

	DATE CDHTime::DateFromDouble( double f ) throw()
	{
		double fTemp;

		// No problem if positive
		if( f >= 0 )
		{
			return(f);
		}

		// If negative, must convert since negative dates not continuous
		// (examples: -.75 to -1.25, -.50 to -1.50, -.25 to -1.75)
		fTemp = floor( f ); // fTemp is now whole part

		return(fTemp + (fTemp - f));
	}

	void CDHTime::CheckRange()
	{
		// About year 100 to about 9999
		if( m_dt > VTDATEGRE_MAX || m_dt < VTDATEGRE_MIN )
		{
			SetStatus( invalid );
		}
	}

	bool CDHTime::ParseDateTime( const TCHAR* lpszDate, uint32_t dwFlags, LCID lcid ) throw()
	{
		LPOLESTR p( NULL );
	#ifdef _UNICODE
		p = (LPOLESTR)lpszDate;
	#else
		p = (LPOLESTR)&StringA2W( lpszDate );
	#endif
		if( p == NULL )
		{
			m_dt = 0;
			m_status = invalid;
			return false;
		}

		HRESULT hr;

		if( FAILED( hr = VarDateFromStr( p, lcid, dwFlags, &m_dt ) ) )
		{
			if( hr == DISP_E_TYPEMISMATCH )
			{
				// Can't convert string to date, set 0 and invalidate
				m_dt = 0;
				m_status = invalid;
				return false;
			}
			else if( hr == DISP_E_OVERFLOW )
			{
				// Can't convert string to date, set -1 and invalidate
				m_dt = -1;
				m_status = invalid;
				return false;
			}
			else
			{
				//NTTNTRACE( "COleDateTime VarDateFromStr call failed.\n" );
				// Can't convert string to date, set -1 and invalidate
				m_dt = -1;
				m_status = invalid;
				return false;
			}
		}

		m_status = valid;
		return true;
	}

	String CDHTimeSpan::Format( const NGTCHAR* pFormat ) const
	{
		if( GetStatus() == null )
			return _PNT( "" );

		struct tm tmTemp;
		tmTemp.tm_sec = GetSeconds();
		tmTemp.tm_min = GetMinutes();
		tmTemp.tm_hour = GetHours();
		tmTemp.tm_mday = GetDays();
		tmTemp.tm_mon = 0;
		tmTemp.tm_year = 0;
		tmTemp.tm_wday = 0;
		tmTemp.tm_yday = 0;
		tmTemp.tm_isdst = 0;

		TCHAR strDateBuf[256] = {0,};
		_tcsftime( strDateBuf, 256, pFormat, &tmTemp );

		String strDate = strDateBuf;

		return strDate;
	}


	String CDHTime::Format( const NGTCHAR* pFormat ) const
	{
		assert( pFormat != NULL );

		if( GetStatus() == null )
			return _PNT( "" );

		if( GetStatus() == invalid )
		{
			return _CONST_VALUE_::szInvalidDateTime;
		}

		UDATE ud;
		if( S_OK != VarUdateFromDate( m_dt, 0, &ud ) )
		{
			return _CONST_VALUE_::szInvalidDateTime;
		}

		struct tm tmTemp;
		tmTemp.tm_sec = ud.st.wSecond;
		tmTemp.tm_min = ud.st.wMinute;
		tmTemp.tm_hour = ud.st.wHour;
		tmTemp.tm_mday = ud.st.wDay;
		tmTemp.tm_mon = ud.st.wMonth - 1;
		tmTemp.tm_year = ud.st.wYear - 1900;
		tmTemp.tm_wday = ud.st.wDayOfWeek;
		tmTemp.tm_yday = ud.wDayOfYear - 1;
		tmTemp.tm_isdst = 0;

		TCHAR strDateBuf[256] = {0,};
		_tcsftime( strDateBuf, 256, pFormat, &tmTemp );
		String strDate = strDateBuf;

		return strDate;
	}
	DHNet::StringW CDHTime::ToString() const
	{
		if( GetStatus() == null )
			return _PNT( "" );

		if( GetStatus() == invalid )
		{
			return _CONST_VALUE_::szInvalidDateTime;
		}

		UDATE ud;
		if( S_OK != VarUdateFromDate( m_dt, 0, &ud ) )
		{
			return _CONST_VALUE_::szInvalidDateTime;
		}

		struct tm tmTemp;
		tmTemp.tm_sec = ud.st.wSecond;
		tmTemp.tm_min = ud.st.wMinute;
		tmTemp.tm_hour = ud.st.wHour;
		tmTemp.tm_mday = ud.st.wDay;
		tmTemp.tm_mon = ud.st.wMonth - 1;
		tmTemp.tm_year = ud.st.wYear - 1900;
		tmTemp.tm_wday = ud.st.wDayOfWeek;
		tmTemp.tm_yday = ud.wDayOfYear - 1;
		tmTemp.tm_isdst = 0;

		wchar_t strDateBuf[256] = {0,};
		wcsftime( strDateBuf, 256, L"%Y-%m-%d %H:%M:%S", &tmTemp );
		StringW strDate = strDateBuf;

		return strDate;
	}

	CDHTime::CDHTime( const DBTIMESTAMP& dbts )
	{
		SYSTEMTIME st;
		::ZeroMemory( &st, sizeof( SYSTEMTIME ) );

		st.wYear = uint16_t( dbts.year );
		st.wMonth = uint16_t( dbts.month );
		st.wDay = uint16_t( dbts.day );
		st.wHour = uint16_t( dbts.hour );
		st.wMinute = uint16_t( dbts.minute );
		st.wSecond = uint16_t( dbts.second );
		st.wMilliseconds = uint16_t( dbts.fraction );

		//		m_status = ::SystemTimeToVariantTime(&st, &m_dt) ? valid : invalid;
		m_status = SystemTimeToVariantTimeMs( st, &m_dt ) ? valid : invalid;
	}

	bool CDHTime::GetAsDBTIMESTAMP( DBTIMESTAMP& dbts ) const
	{
		UDATE ud;
		if( S_OK != VarUdateFromDate( m_dt, 0, &ud ) )
			return false;

		dbts.year = (SHORT)ud.st.wYear;
		dbts.month = (uint16_t)ud.st.wMonth;
		dbts.day = (uint16_t)ud.st.wDay;
		dbts.hour = (uint16_t)ud.st.wHour;
		dbts.minute = (uint16_t)ud.st.wMinute;
		dbts.second = (uint16_t)ud.st.wSecond;
		dbts.fraction = (uint16_t)ud.st.wMilliseconds;

		return true;
	}

}
