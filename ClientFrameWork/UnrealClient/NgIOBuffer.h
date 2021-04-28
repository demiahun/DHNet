#pragma once

#include "CoreMinimal.h"
#include "BasicType.h"

namespace DHNet
{
	/*
	packet IO 를 받을때 사용되는 버퍼
	*/
	class IOBuffer
	{
	public:
		IOBuffer()
		{}
		virtual ~IOBuffer()
		{}

		virtual void Clear()
		{
			m_HostID = 0;
			m_Length = 0;
			FMemory::Memzero( m_buffer, sizeof( char ) * MAX_PACKET_SIZE );
		}

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
		IOBufferPool()
		{}
		virtual ~IOBufferPool()
		{
			for( Int32 i = 0; i < m_vecPool.Num(); ++i )
			{
				delete m_vecPool[i];
			}
			m_vecPool.Empty();
		}

		T*		Alloc()
		{
			FScopeLock lock( &m_cs );

			// 이미 만들어 진게 있다면
			if( m_queFree.IsEmpty() == false )
			{
				CMessage* pBuffer = nullptr;
				m_queFree.Dequeue( pBuffer );

				pBuffer->Clear();
				return pBuffer;
			}

			auto newBuffer = new T();
			if( newBuffer == nullptr )
			{
				//NGLOG_ERROR( L"[FATAL] Not enough memory ================================" );

				return nullptr;
			}
			m_vecPool.Add( newBuffer );

			return newBuffer;
		}
		/*
		버퍼 반납
		*/
		void	Free( T* pkBuffer )
		{
			FScopeLock lock( &m_cs );

			/*
				m_vecFree.push_back( pkBuffer->m_index ); 와 같은 동작
				자주사용되는 버퍼풀이라 속도 때문에 아래와 같이 작성
			*/
			m_queFree.Enqueue( pkBuffer );
		}
		/*
		버퍼의 총 개수
		*/
		int		GetSize()
		{
			int iSize = 0;
			{
				FScopeLock lock( &m_cs );
				iSize = m_vecPool.size();
			}
			return iSize;
		}
		int		GetFreeSize()
		{
			int iSize = 0;
			//{
			//	FScopeLock lock( &m_cs );
			//	iSize = m_queFree.size();
			//}
			return iSize;
		}

	protected:
		FCriticalSection
			m_cs;
		TArray< T* >
			m_vecPool;
		UInt32	m_indexPool = 0;
		/*
			풀에서 대기중인 버퍼.
		*/
		TQueue< T* >
			m_queFree;
	};
}