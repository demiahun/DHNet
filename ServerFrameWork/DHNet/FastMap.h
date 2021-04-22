#pragma once




template< typename K, typename V >
class CFastMap : public CAtlMap<K,V>
{
public:
	CFastMap(){}
	virtual ~CFastMap()
	{
		RemoveAll();
	}

	inline void Clear()
	{
		RemoveAll();
	}

	inline bool Add( K k, V v )
	{
		auto pair = Lookup( k );
		if( pair != nullptr )
			return false;

		SetAt( k, v );
		return true;
	}

	const CPair* find( KINARGTYPE key ) const throw()
	{
		return Lookup( key );
	}
	CPair* find( KINARGTYPE key ) throw()
	{
		return Lookup( key );
	}

    bool erase( KINARGTYPE key ) throw()
    {
        return RemoveKey( key );
    }

	class value_type
	{
	public:
		POSITION m_pos;
		CFastMap* m_owner;

		__declspec(property(get = GetFirst)) const K& m_key;
		__declspec(property(get = GetSecond, put = SetSecond)) V& m_value;

		inline const K& GetFirst() const
		{
			return m_owner->GetKeyAt( m_pos );
		}
		inline V& GetSecond() const
		{
			return m_owner->GetValueAt( m_pos );
		}
		inline void SetSecond( const V& val ) const
		{
			return m_owner->SetValueAt( m_pos, val );
		}
	};

	class iterator;
	friend class iterator;

	class reverse_iterator;
	friend class reverse_iterator;

	class const_iterator :public value_type
	{
	public:
		inline const_iterator() {}
		inline const_iterator( const iterator& src ) :value_type( src ) {}

		const_iterator& operator++()
		{
			value_type::m_owner->GetNext( value_type::m_pos );
			return (*this);
		}

		const_iterator operator++( int )
		{
			const_iterator _Tmp = *this;
			++*this;
			return (_Tmp);
		}

		inline bool operator==( const const_iterator& a ) const
		{
			return value_type::m_pos == a.m_pos && value_type::m_owner == a.m_owner;
		}

		inline bool operator!=( const const_iterator& a ) const
		{
			return !(value_type::m_pos == a.m_pos && value_type::m_owner == a.m_owner);
		}

		inline const value_type& operator*() const
		{
			return *this;
		}

		inline const value_type* operator->() const
		{
			return this;
		}
	};

	class const_reverse_iterator;
	friend class const_reverse_iterator;


	class const_reverse_iterator :public value_type
	{
	public:
		inline const_reverse_iterator() {}
		inline const_reverse_iterator( const reverse_iterator& src ) :value_type( src ) {}

		const_reverse_iterator& operator++()
		{
			value_type::m_owner->GetPrev( value_type::m_pos );
			return (*this);
		}

		const_reverse_iterator operator++( int )
		{
			const_reverse_iterator _Tmp = *this;
			++*this;
			return (_Tmp);
		}

		inline bool operator==( const const_reverse_iterator& a ) const
		{
			return value_type::m_pos == a.m_pos && value_type::m_owner == a.m_owner;
		}

		inline bool operator!=( const const_reverse_iterator& a ) const
		{
			return !(value_type::m_pos == a.m_pos && value_type::m_owner == a.m_owner);
		}

		inline const value_type& operator*() const
		{
			return *this;
		}

		inline const value_type* operator->() const
		{
			return this;
		}
	};

	class iterator :public value_type
	{
	public:
		inline iterator() {}
		inline iterator( const const_iterator& src ) :value_type( src ) {}

		inline bool operator==( const iterator& a ) const
		{
			return value_type::m_pos == a.m_pos && value_type::m_owner == a.m_owner;
		}

		inline bool operator!=( const iterator& a ) const
		{
			return !(value_type::m_pos == a.m_pos && value_type::m_owner == a.m_owner);
		}

		inline iterator& operator++()
		{
			value_type::m_owner->GetNext( value_type::m_pos );
			return (*this);
		}

		inline iterator operator++( int )
		{
			iterator _Tmp = *this;
			++*this;
			return (_Tmp);
		}

		inline iterator& operator--()
		{
			value_type::m_owner->GetPrev( value_type::m_pos );
			return (*this);
		}

		inline iterator operator--( int )
		{
			--(*this);
			return (*this);
		}

		inline const value_type& operator*() const
		{
			return *this;
		}

		inline const value_type* operator->() const
		{
			return this;
		}
	};

	inline iterator begin()
	{
		iterator ret;
		ret.m_pos = GetStartPosition();
		ret.m_owner = this;

		return ret;
	}

	inline iterator end()
	{
		iterator ret;
		ret.m_pos = NULL;
		ret.m_owner = this;

		return ret;
	}

	inline value_type front()
	{
		if( GetCount() == 0 )
			return value_type;

		return *begin();
	}

	class reverse_iterator :public value_type
	{
	public:
		inline reverse_iterator()
		{}
		inline reverse_iterator( const const_reverse_iterator& src ) :value_type( src )
		{}

		inline bool operator==( const reverse_iterator& a ) const
		{
			return value_type::m_pos == a.m_pos && value_type::m_owner == a.m_owner;
		}

		inline bool operator!=( const reverse_iterator& a ) const
		{
			return !(value_type::m_pos == a.m_pos && value_type::m_owner == a.m_owner);
		}

		inline reverse_iterator& operator++()
		{
			value_type::m_owner->GetPrev( value_type::m_pos );
			return (*this);
		}

		inline reverse_iterator operator++( int )
		{
			reverse_iterator _Tmp = *this;
			++*this;
			return (_Tmp);
		}

		inline reverse_iterator& operator--()
		{
			value_type::m_owner->GetNext( value_type::m_pos );
			return (*this);
		}

		inline reverse_iterator operator--( int )
		{
			--(*this);
			return (*this);
		}

		inline const value_type& operator*() const
		{
			return *this;
		}

		inline const value_type* operator->() const
		{
			return this;
		}
	};

	inline reverse_iterator rbegin()
	{
		reverse_iterator ret;
		ret.m_pos = GetEndPosition();
		ret.m_owner = this;

		return ret;
	}

	inline reverse_iterator rend()
	{
		reverse_iterator ret;
		ret.m_pos = NULL;
		ret.m_owner = this;

		return ret;
	}

	inline const_iterator begin() const
	{
		const_iterator ret;
		ret.m_pos = GetStartPosition();
		ret.m_owner = (CFastMap*)this;

		return ret;
	}

	inline const_iterator end() const
	{
		const_iterator ret;
		ret.m_pos = NULL;
		ret.m_owner = (CFastMap*)this;

		return ret;
	}

	inline const_reverse_iterator rbegin() const
	{
		const_reverse_iterator ret;
		ret->m_pos = GetEndPosition();
		ret->m_owner = (CFastMap*)this;

		return ret;
	}

	inline const_reverse_iterator rend() const
	{
		const_reverse_iterator ret;
		ret->m_pos = NULL;
		ret->m_owner = (CFastMap*)this;

		return ret;
	}
};