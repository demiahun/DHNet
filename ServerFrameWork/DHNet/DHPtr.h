#pragma once

#include "BasicType.h"
#include <atomic>

#pragma pack(push,8)

namespace DHNet
{
	/*
	SmartPointer :
		RefCount �� ���� ������ �־� �ڵ����� ���� ���ִ� ����
		Tombstone �� ���� ���ü ������ �� RefCount ����
		P.S : RefCount �� ���ü ���� �ڵ忡�� ��Ʈ�� �Ҽ� ����.
	*/
	template<typename T>
	class RefCount
	{
	public:
		typedef RefCount<T> Type;
	private:
		struct Tombstone
		{
			T*		m_ptr = nullptr;

			std::atomic< intptr_t >
					m_count;

			inline Tombstone()
			{
				m_count = 0;
			}
			inline ~Tombstone() {}

		#pragma push_macro("new")
		#undef new
		private:
			// new,delete oper ������Ŵ
			void* operator new(size_t size);
			void operator delete(void* ptr, size_t size);
		public:
		#pragma pop_macro("new")

		public:
			inline static Tombstone* NewInstance()
			{
				Tombstone* ret = (Type::Tombstone*)::malloc( sizeof( Type::Tombstone ) );

				return ret;
			}

			inline static void DeleteInstance( Tombstone* object )
			{
				delete object->m_ptr;
				::free( (Type::Tombstone*)object );
			}

			NO_COPYABLE( Tombstone )
		};

		Tombstone* m_tombstone = nullptr;

	public:
		inline void IncRefCount()
		{
			if( m_tombstone == nullptr )
				return;
			++m_tombstone->m_count;
		}
		inline void DecRefCount()
		{
			if( m_tombstone == nullptr )
				return;
			--m_tombstone->m_count;
		}

		inline intptr_t GetRefCount()
		{
			if( m_tombstone == nullptr )
				return 0;

			return m_tombstone->m_count;
		}

		inline explicit RefCount( T* p = 0 )
		{
			if( p != nullptr )
			{
				m_tombstone = Tombstone::NewInstance();

				m_tombstone->m_count = 1;
				m_tombstone->m_ptr = p;
			}
			else
			{
				m_tombstone = nullptr;
			}
		}

		inline RefCount( const Type& other )
		{
			m_tombstone = nullptr;
			AssignFrom( other );
		}

		inline ~RefCount()
		{
			Reset();
		}

		inline void Reset()
		{
			if( m_tombstone )
			{
				--m_tombstone->m_count;
				if( m_tombstone->m_count == 0 )
				{
					Tombstone::DeleteInstance( m_tombstone );
				}
			}
			m_tombstone = nullptr;
		}

		inline void reset()
		{
			Reset();
		}

	private:
		inline void AssignFrom( const Type& other )
		{
			// �̹� �����Ÿ� ��ŵ
			if( other.m_tombstone == m_tombstone )
				return;

			// ����� RefCount ����
			if( other.m_tombstone )
			{
				++other.m_tombstone->m_count;
			}

			// ���� RefCount ����
			Tombstone* deletePendee = nullptr;
			if( m_tombstone )
			{
				--m_tombstone->m_count;
				if( m_tombstone->m_count == 0 )
				{
					deletePendee = m_tombstone;
				}
			}

			// ����
			m_tombstone = other.m_tombstone;

			// �ı��ؾ� �ϴ� ��ü�� �ڴ��� �ı�
			if( deletePendee != nullptr )
			{
				Tombstone::DeleteInstance( deletePendee );
			}
		}

	public:
		inline Type& operator=( const Type& other )
		{
			AssignFrom( other );
			return *this;
		}

		inline operator T* () const
		{
			return m_tombstone ? m_tombstone->m_ptr : 0;
		}
		inline T* operator->() const
		{
			return m_tombstone ? m_tombstone->m_ptr : 0;
		}
		inline T* get() const
		{
			return m_tombstone ? m_tombstone->m_ptr : 0;
		}

	};

	/*
	SmartPointer :
		RefObject �� ���ü�� ��ӽ�Ű�� RefObjectCount �� ���� ����Ʈ �����͸� �����ϴ� �������
		���ü ���ο��� RefCount �� ���� ��Ʈ�� �Ҽ� �ִ� ����
		��뿹:
			CSocket : public RefObject
			RefObjectCount<CSocket> CSocketPtr;

			CSocket::SendPacket()
			{
				IncRefCount();	// IOCP ��ü�� ���� ��Ŷ�� ���� �� �� ���� ���ü�� �������� �ʱ� ���� ���Ƿ� RefCount �� �ø���
			}

			IOCP::IOCallback() ���� Send ó���� �Ϸ� ���� DecRefCount() �� �Ͽ� ���� �ǵ��� �Ҷ� ���
	*/
	class RefObject
	{
	public:
		RefObject()
		{
			m_count = 0;
		}
		virtual ~RefObject(){}

		inline void IncRefCount()
		{
			++m_count;
		}
		inline void DecRefCount()
		{
			--m_count;

			if( m_count == 0 )
			{
				delete this;
			}
		}
		/*
		�� ��ü�� �����ϰ� �ִ� �ٸ� ���� ��ü���� ������ �����Ѵ�. �� ���� Ƚ����.
		*/
		inline intptr_t GetRefCount()
		{
			return m_count;
		}

	private:
		std::atomic< intptr_t >
				m_count;
	};
	template <class T> 
	class RefObjectCount
	{
	public:
		// construction and destruction
		inline RefObjectCount( T* pObject = (T*)0 )
		{
			m_pObject = pObject;
			if( m_pObject )
				m_pObject->IncRefCount();
		}
		inline RefObjectCount( const RefObjectCount& ptr )
		{
			m_pObject = ptr.m_pObject;
			if( m_pObject )
				m_pObject->IncRefCount();
		}
		inline ~RefObjectCount()
		{
			if( m_pObject )
				m_pObject->DecRefCount();
		}

		// implicit conversions
		inline operator T*() const
		{
			return m_pObject;
		}
		inline T& operator*() const
		{
			return *m_pObject;
		}
		inline T* operator->() const
		{
			return m_pObject;
		}

		// assignment
		RefObjectCount& operator=( const RefObjectCount& ptr )
		{
			RefObjectCount( ptr ).swap( *this );

			return *this;
		}
		RefObjectCount& operator=( T* pObject )
		{
			RefObjectCount( ptr ).swap( *this );

			return *this;
		}

		// comparisons
		bool operator==( T* pObject ) const
		{
			return (m_pObject == pObject);
		}
		bool operator!=( T* pObject ) const
		{
			return (m_pObject != pObject);
		}
		bool operator==( const RefObjectCount& ptr ) const
		{
			return (m_pObject == ptr.m_pObject);
		}
		bool operator!=( const RefObjectCount& ptr ) const
		{
			return (m_pObject != ptr.m_pObject);
		}

		void swap( RefObjectCount& ptr )
		{
			T* pTemp = m_pObject;
			m_pObject = ptr.m_pObject;
			ptr.m_pObject = pTemp;
		}

		void swap_( T* pTarget )
		{
			T* pTemp = m_pObject;

			InterlockedExchangePointer( (PVOID*)&m_pObject, (PVOID)pTarget );

			if( pTemp )
				pTemp->DecRefCount();

			if( pTarget )
				pTarget->IncRefCount();
		}

	protected:
		// the managed pointer
		T* m_pObject;
	};
}

#define REFCOUNT(classname) typedef DHNet::RefCount<classname> classname##Ptr
#define REFOBJECTCOUNT(classname) typedef DHNet::RefObjectCount<classname> classname##Ptr;

#pragma pack(pop)