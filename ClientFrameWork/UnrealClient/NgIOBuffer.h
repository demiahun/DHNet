#pragma once

#include "CoreMinimal.h"
#include "BasicType.h"

namespace DHNet
{
	/*
	packet IO �� ������ ���Ǵ� ����
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
		���� ����Ǯ
		������ ���ڿ� public UInt32 m_index �� �־�� ������
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

			// �̹� ����� ���� �ִٸ�
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
		���� �ݳ�
		*/
		void	Free( T* pkBuffer )
		{
			FScopeLock lock( &m_cs );

			/*
				m_vecFree.push_back( pkBuffer->m_index ); �� ���� ����
				���ֻ��Ǵ� ����Ǯ�̶� �ӵ� ������ �Ʒ��� ���� �ۼ�
			*/
			m_queFree.Enqueue( pkBuffer );
		}
		/*
		������ �� ����
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
			Ǯ���� ������� ����.
		*/
		TQueue< T* >
			m_queFree;
	};
}