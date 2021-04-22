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
		critical section ��ü�� �����ڿ��� �ٷ� lock�� �� �ִ�.
		\param cs ����� critical section ��ü
		\param initialLock true�̸� �����ڿ��� �ٷ� lock�Ѵ�.
		*/
		inline CSLocker( CriticalSection& cs, bool initialLock = true )
		{
			SetCriticalSection( cs, initialLock );
		}
		/*
		�ı���
		- �̹� �� ��ü�� �����ϰ� �ִ� CriticalSection�� ���� ��� ���� ������ �ڵ����� �Ѵ�.
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
		critical section�� ���߿� �����Ҷ��� ����� �������Դϴ�.
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
		����� �Ǿ��ֳ�?
		*/
		bool IsLocked() const
		{
			assert( m_recursionCount >= 0 );
			return m_recursionCount > 0;
		}
		/*
		critical section�� �����Ѵ�.
		*/
		inline void Lock()
		{
			m_cs->Lock();
			++m_recursionCount;
		}
		/*
		critical section�� ���� �����Ѵ�.
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
		���� ��ø�� Ƚ���� ��ȸ�Ѵ�.
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
		�� ���� Lock�� ȣ���ߴ°�?
		����: m_cs�� ���� lock Ƚ���� �ٸ� �� �ִ�.
		�� ���� �� CriticalSectionLock�� ���ؼ� Lock�� �͵鸸 ī�����Ѵ�.
		*/
		int		m_recursionCount = 0;;
	};

#if (_WIN32_WINNT >= 0x0600)
	/*
	���� : https://msdn.microsoft.com/ko-kr/library/windows/desktop/aa904937(v=vs.85).aspx
	���ɺ� : SRWLock, SpinLock, CriticalSection
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