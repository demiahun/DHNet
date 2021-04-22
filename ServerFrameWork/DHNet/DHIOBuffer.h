#pragma once

#include <vector>
#include <queue>
#include "BasicType.h"
#include "DHLock.h"
#include "DHLog.h"

#pragma pack(push,8)

namespace DHNet
{
	/*
	packet IO 를 받을때 사용되는 버퍼
	*/
	class IOBuffer
	{
	public:
		IOBuffer(){}		
		virtual ~IOBuffer(){}

		virtual void Clear();

	public:
		char	m_buffer[MAX_PACKET_SIZE] = {0,};
		Int32	m_Length = 0;
		HostID	m_HostID = 0;
	};

	/*
		재사용 버퍼풀
		데이터 인자에 public UInt32 m_index 가 있어야 동작함
	*/
	template<typename T>
	class IOBufferPool
	{
	public:
		IOBufferPool(){}
		virtual ~IOBufferPool()
		{
			for( int i = 0; i < (int)m_vecPool.size(); ++i )
			{
				delete m_vecPool[i];
			}
			m_vecPool.clear();
		}

		T*		Alloc()
		{
			CSLocker lock( m_cs );

			// 이미 만들어 진게 있다면
			if( m_queFree.size() > 0 )
			{
				auto pBuffer = m_queFree.front();
				m_queFree.pop();
								
				pBuffer->Clear();
				return pBuffer;
			}

			auto newBuffer = new T();
			if( newBuffer == nullptr )
			{
				DHLOG_ERROR( L"[FATAL] Not enough memory ================================" );

				return nullptr;
			}
			m_vecPool.push_back( newBuffer );

			return newBuffer;
		}
		/*
		버퍼 반납
		*/
		void	Free( T* pkBuffer )
		{
			CSLocker lock( m_cs );

			/*
				m_vecFree.push_back( pkBuffer->m_index ); 와 같은 동작
				자주사용되는 버퍼풀이라 속도 때문에 아래와 같이 작성
			*/
			m_queFree.push( pkBuffer );
		}
        void    Free( std::queue<T*> &qBuffers )
        {
            CSLocker lock( m_cs );

            auto size = qBuffers.size();
            for( int i = 0; i < size; ++i )
            {
                m_queFree.push( qBuffers.front() );
                qBuffers.pop();
            }
        }
		/*
		버퍼의 총 개수
		*/
		int		GetSize()
		{
			int iSize = 0;
			{
				CSLocker lock( m_cs );
				iSize = (int)m_vecPool.size();
			}
			return iSize;
		}
		int		GetFreeSize()
		{
			int iSize = 0;
			{
				CSLocker lock( m_cs );
				iSize = (int)m_queFree.size();
			}
			return iSize;
		}

		/*
			버퍼의 크기를 미리 잡아 둔다
		*/
		void	Reserve( IN UInt32 _Size )
		{
			std::vector< T* > vecBuff;
			for( UInt32 i = 0; i < _Size; ++i )
			{
				vecBuff.push_back( Alloc() );
			}
			for( auto &msg : vecBuff )
			{
				Free( msg );
			}
		}
		/*
			사용되지 않는 메모리 풀을 줄인다.
		*/
		void	DeleteFreeMemery( IN UInt32 _Size )
		{
			CSLocker lock( m_cs );

			if( m_queFree.size() < _Size )
				return;

			for( UInt32 i = 0; i < _Size; ++i )
			{
				auto pBuffer = m_queFree.front();
				m_queFree.pop();

				auto vit = std::find( m_vecPool.begin(), m_vecPool.end(), pBuffer );
				if( vit != m_vecPool.end() )
				{
					m_vecPool.erase( vit );
				}

				delete pBuffer;
			}
		}

	protected:
		CriticalSection
				m_cs;
		std::vector< T* >
				m_vecPool;
		UInt32	m_indexPool = 0;
		/*
			풀에서 대기중인 버퍼.
		*/
		std::queue< T* >
				m_queFree;
	};
}

#pragma pack(pop)