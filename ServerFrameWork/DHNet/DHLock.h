#pragma once

#include <Windows.h>
#include <assert.h>

#pragma pack(push,8)

namespace DHNet
{
	struct CriticalSection : public CRITICAL_SECTION
	{
	public:
		CriticalSection()
		{
			::InitializeCriticalSection( this );
		}
		CriticalSection( int iID )
		{
			m_iID = iID;
			::InitializeCriticalSection( this );
		}
		~CriticalSection()
		{
			DeleteCriticalSection( this );
		}

		int		GetID()
		{
			return m_iID;
		}

		void	Lock()
		{
			::EnterCriticalSection( this );
		}
		void	Unlock()
		{
			::LeaveCriticalSection( this );
		}

		int		m_iID = -1;
	};

	class CSLocker
	{
	public:
		/*
		critical section 객체를 생성자에서 바로 lock할 수 있다.
		\param cs 사용할 critical section 객체
		\param initialLock true이면 생성자에서 바로 lock한다.
		*/
		inline CSLocker( CriticalSection& cs, bool initialLock = true )
		{
			SetCriticalSection( cs, initialLock );
		}
		/*
		파괴자
		- 이미 이 객체가 점유하고 있던 CriticalSection이 있을 경우 점유 해제를 자동으로 한다.
		*/
		inline ~CSLocker()
		{
			assert( m_recursionCount >= 0 );
			for( ; m_recursionCount > 0; --m_recursionCount )
			{
				m_cs->Unlock();
			}
		}
		/*
		critical section을 나중에 세팅할때에 사용할 생성자입니다.
		*/
		inline CSLocker()
		{
			m_cs = NULL;
			m_recursionCount = 0;
		}
		inline void SetCriticalSection( CriticalSection &cs, bool initialLock )
		{
			m_cs = &cs;
			m_recursionCount = 0;
			if( initialLock )
			{
				Lock();
			}
		}

		/*
		잠금이 되어있나?
		*/
		bool IsLocked() const
		{
			assert( m_recursionCount >= 0 );
			return m_recursionCount > 0;
		}
		/*
		critical section을 점유한다.
		*/
		inline void Lock()
		{
			m_cs->Lock();
			++m_recursionCount;
		}
		/*
		critical section을 점유 해제한다.
		*/
		inline void Unlock()
		{
			if( IsLocked() )
			{
				--m_recursionCount;
				m_cs->Unlock();
			}
		}
		/*
		락이 중첩된 횟수를 조회한다.
		*/
		inline int GetRecursionCount()
		{
			assert( m_recursionCount >= 0 );
			return m_recursionCount;
		}

	private:
		CriticalSection
				*m_cs = nullptr;
		/*
		몇 번의 Lock을 호출했는가?
		주의: m_cs의 실제 lock 횟수와 다를 수 있다.
		이 값은 이 CriticalSectionLock에 의해서 Lock한 것들만 카운팅한다.
		*/
		int		m_recursionCount = 0;;
	};

#if (_WIN32_WINNT >= 0x0600)
	/*
	참조 : https://msdn.microsoft.com/ko-kr/library/windows/desktop/aa904937(v=vs.85).aspx
	성능비교 : SRWLock, SpinLock, CriticalSection
		https://megayuchi.com/2017/06/25/srwlock-%EB%B9%A0%EB%A5%B8-%EC%84%B1%EB%8A%A5%EC%9D%98-%EB%B9%84%EA%B2%B0/
	*/
	struct RWLock : public SRWLOCK
	{
		RWLock()
		{
			::InitializeSRWLock( this );
		}
		virtual ~RWLock(){}
	};
	class SReadLocker
	{
	public:
		SReadLocker( RWLock& lock )
		{
			m_pkLock = &lock;
			_Lock();
		}
		~SReadLocker()
		{
			_Leave();
		}

	private:
		void	_Lock()
		{
			::AcquireSRWLockShared( m_pkLock );
		}
		void	_Leave()
		{
			::ReleaseSRWLockShared( m_pkLock );
			m_pkLock = NULL;
		}

	private:
		RWLock* m_pkLock = nullptr;
	};

	class SWriteLocker
	{
	public:
		SWriteLocker( RWLock& lock )
		{
			m_pkLock = &lock;
			_Lock();
		}
		~SWriteLocker()
		{
			_Leave();
		}

	private:
		void	_Lock()
		{
			::AcquireSRWLockExclusive( m_pkLock );
		}
		void	_Leave()
		{
			::ReleaseSRWLockExclusive( m_pkLock );
			m_pkLock = NULL;
		}

	private:
		RWLock* m_pkLock = nullptr;
	};
#endif
}

#pragma pack(pop)