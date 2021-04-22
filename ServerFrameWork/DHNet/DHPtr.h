#pragma once

#include "BasicType.h"
#include <atomic>

#pragma pack(push,8)

namespace DHNet
{
	/*
	SmartPointer :
		RefCount 를 직접 가지고 있어 자동으로 관리 해주는 패턴
		Tombstone 를 통해 대상객체 포인터 및 RefCount 관리
		P.S : RefCount 를 대상객체 내부 코드에서 컨트롤 할수 없음.
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
			// new,delete oper 금지시킴
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
			// 이미 같은거면 스킵
			if( other.m_tombstone == m_tombstone )
				return;

			// 상대측 RefCount 증가
			if( other.m_tombstone )
			{
				++other.m_tombstone->m_count;
			}

			// 이쪽 RefCount 감소
			Tombstone* deletePendee = nullptr;
			if( m_tombstone )
			{
				--m_tombstone->m_count;
				if( m_tombstone->m_count == 0 )
				{
					deletePendee = m_tombstone;
				}
			}

			// 본격
			m_tombstone = other.m_tombstone;

			// 파괴해야 하는 객체의 뒤늦은 파괴
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
		RefObject 를 대상객체에 상속시키고 RefObjectCount 를 통해 스마트 포인터를 생성하는 방식으로
		대상객체 내부에서 RefCount 를 직접 컨트롤 할수 있는 패턴
		사용예:
			CSocket : public RefObject
			RefObjectCount<CSocket> CSocketPtr;

			CSocket::SendPacket()
			{
				IncRefCount();	// IOCP 객체를 통해 패킷이 전달 될 때 까지 대상객체를 삭제하지 않기 위해 임의로 RefCount 를 올리고
			}

			IOCP::IOCallback() 에서 Send 처리를 완료 한후 DecRefCount() 를 하여 삭제 되도록 할때 사용
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
		이 객체를 참조하고 있는 다른 변수 객체들의 갯수를 리턴한다. 즉 참조 횟수다.
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