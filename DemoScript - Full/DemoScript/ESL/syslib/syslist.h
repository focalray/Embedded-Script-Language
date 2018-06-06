

/* 
	sys_list V1R10, (c) 1999-2007 lollab. All rights reserved.
	Jinhong Kim <windgram@lollab.com>

	+++ VisualStudio 2005 compliant code +++

    * FEATURES
    ------------------------------------------------
	- Doubly linked list template.
	- Provide enough operators and simple usage.
	- Stack and queue operation supported.
	- Random access.
	- It's very stabilized and optimized.


	* sys_list::node class members
    ------------------------------------------------
	node& prev();
	node& next();
	node& operator[](int x);
	node& operator=(node& n);
	node& operator=(node& n);
	BOOL operator!=(node& n);
	BOOL operator==(node& n);
	BOOL operator!=(VT value);
	BOOL operator==(VT value);
	node& operator++();
	node& operator++(int)
	node& operator--();
	node& operator--(int);
	VT operator*();
	VT operator=(VT value);
	VT operator->();
	BOOL isend();

	* sys_list class members
    ------------------------------------------------
	node& addbefore(VT value);
	node& add(VT value);
	node& addunique(VT value);
	void insertbefore(node& n, VT value);
	void insert(node& n, VT value);
	VT pop();
	VT qpop();
	VT peek();
	VT qpeek();
	node& front();
	node& back();
	BOOL isempty();
	BOOL remove(node& n);
	BOOL remove(VT v);
	BOOL removefront();
	BOOL removeback();
	void removeall();
	void reverse();
	void remap();
	VT operator[](int x);
	node find( VT v );
	node getnode( int nth );


--------------------------------------------------------------------------------------------------------------
	<<< IMPORTANT ISSUE >>>
--------------------------------------------------------------------------------------------------------------

	* sys_list initializes itself at its class constructor.  The class constructors are instanced at anytime
	when the owning process is instanced.  Because we cannot decide the precedence of the instance-creation timing,
	it is dangerous that use sys_list in the other constructor. If you use, for example, sys_list::add() in 
	the any constructor, it is possible that just after the process of add(), so the constructor of sys_list
	could be called that it resets the sys_list breaking the already established link-contents.
	Simply, it's not good to use sys_list in any constructors.


--------------------------------------------------------------------------------------------------------------
	<<< IMPORTANT REMARK : List Empty Scheme >>>
--------------------------------------------------------------------------------------------------------------
	If you should empty a 'list', must NOT use like following code:

			for (int i=0; i<list.num; i++)
			{
				...
				delete[] list[i];
				list.remove( list[i] );
			}

	Assume that the 'list' has 4 items now. And you are to remove all the items on the list like that.
	You can think the index 'i' will repeat from 0 to 3. But after 'remove()' function,
	'list.num' will automatically decrease by 1. So you will find that the index going through
	from 0 to 1 - only two loops. You have to follow next code:

	To empty a list, there are about 3 methods which are recommended. 
	Use one of them.

	while( !list.isempty() )
	{
		...
		(1) pops the items out one by one.
		list.pop();   or...
		(2) removes items at the first index.
		list.remove( list[0] );   or...
		(3) use removeall() after removal of the allocated pointer items in the list by yourself.
	}

  
--------------------------------------------------------------------------------------------------------------
	<<< COMPARISON REMARK >>>
--------------------------------------------------------------------------------------------------------------
	sys_list<int>::node n1;
	sys_list<int>::node n2;
	sys_list<int>::node* pn;

	n1 = list.add(1);
    list.add(2);
	list.add(3);

	n2 = n1;
	(1) n1 == n2	==> true!


	pn = &n1;
	n2 = pn;
	(2) n1 == n2	==> false!	


	pn = &n1;
	n2 = *pn;
	(3) n1 == n2	==> true!	


	To test whether the node is empty:

	n == sys_list<int>::node();  or
	n == nullnode(int);


	You have to watch the following statement:

	if ( NULL == n )...					The operator '==' of node must be placed to as postfix.
										The global operator has not been defined.

	if ( n == NULL )...					If the node 'n' is empty, it raises memory access violation error.
										If not, the 'NULL' is recognized to a value of '0', and the operator executes
										value-comparison. So if the value of 'n' was '0', then the comparison
										results in true.

--------------------------------------------------------------------------------------------------------------
	<<< PRECEDENCE REMARK >>>
--------------------------------------------------------------------------------------------------------------
	*n1++		==> ++ first, * next
	(*n1)++		==> * first, ++ next


--------------------------------------------------------------------------------------------------------------	
	<<< ITERATION REMARK >>>
--------------------------------------------------------------------------------------------------------------	
	To iterate with node:

	n = list.front();

	You can iterate to the next by:
	
	n++;	or		++n;


	Assume below 3 lines:

	n = list.add( 1 );
	list.add( 2 );
	list.add( 3 );

	You cannot iterate by 'n++', because the 'n' only indicates
	the first item of 1 in the link which has not been completed.

	Assume below 3 lines:

	list.add( 1 );
	list.add( 2 );
	n = list.add( 3 );		Node 'n' indicates the last item.

	The sys_list is functionally a doubly-linked list. So you can iterate 'n' from the last item of '3'
	up to the first item of '1'.

--------------------------------------------------------------------------------------------------------------	
	<<< SUBSTITUTION REMARK >>>
--------------------------------------------------------------------------------------------------------------	

	You cannot substitute an empty node with a value.

	For example:

	sys_list<int>			list;
	sys_list<int>::node		n, tmp;

	n = 10;			==> error!		It raises memory access violation error!
	...

	tmp = list.add(1);
	n = tmp;						Node can substitute another node although it is an empty one.

	n = 10;			==> ok!			'10' is injected to 'tmp's ptr node. The 'tmp' is indicated by node 'n'.

//-----------------------------------------------------------------------------------------------------------//	




	Revision History:

	UPDATE DATE		WHO			ACT			DESCRIPTION
	----------------------------------------------------------------------------------------------------------
	2005-Nov-28		JHKIM		renew		
	2005-Nov-28		JHKIM		add			add peek and qpeek member functions.
	2005-Dec-16		JHKIM		modify		Performance improved.
	2005-Dec-16		JHKIM		remove		removed: node* operator&()
	2005-Dec-20		JHKIM		add			added: nullnode(vt) MACRO
*/




#ifndef _FLINKEDLIST_H
#define _FLINKEDLIST_H


#include <windows.h>
//#include "fhash.h"
#include "assert.h"
#include "malloc.h"


//#define USECACHE

#ifdef USECACHE
#define cachesize 10000
#endif

#define nullnode(vt)	sys_list<vt>::node()


#define SYSLIST_NEW(t,n)	(t*)malloc(sizeof(t)*n)
#define SYSLIST_DEL			free


template <class VT>
class sys_list
{
public:
	class node
	{
		friend sys_list;

	private:
		VT			v;
		node*		ptr;
		node*		p;			// previous
		node*		n;			// next
		int			vidx;

	public:
		node()
		{
			ptr		= NULL;
			p		= NULL;
			n		= NULL;
			memset( (void*)&v, 0, sizeof(VT) );
			vidx	= 0;
		}
		node(node& _n)
		{
			node();
			v		= _n.v;
			ptr		= _n.ptr;
			p		= _n.p;
			n		= _n.n;
			vidx	= _n.vidx;
		}
		node(node* _n)
		{
			node();
			if (NULL != _n)
			{
				v		= _n->v;
				ptr		= _n->ptr;
				p		= _n->p;
				n		= _n->n;
				vidx	= _n->vidx;
			}
		}
		~node() {}


		node& prev() { return *p; }
		node& next() { return *n; }

		node& operator[](int x)
		{
			if ( 0 == num )
			{
				assert( num > 0 );
				return node();
			}

			if (vidx == x)
				return (*ptr);
			
			int diff = x - vidx;
			static int i;
			
			if (diff > 0)
			{
				if (1 == diff) return ++(*this);
				if (2 == diff) return ++(++(*this));
				if (3 == diff) return ++(++(++(*this)));
				for (i=0; i<diff; i++) (*this)++;
			}
			else
			{
				if (-1 == diff) return --(*this);
				if (-2 == diff) return --(--(*this));
				if (-3 == diff) return --(--(--(*this)));
				for (i=0; i<-diff; i++) (*this)--;
			}
					
			return (*this);
		}

		// copies the right node. and this is extendable.  i.e.) a = b = c;
		node& operator=(node& _n)
		{
			ptr		= _n.ptr;
			v		= _n.v;
			n		= _n.n;
			p		= _n.p;
			vidx	= _n.vidx;
			return (*this);
		}
		node& operator=(node* _n)
		{
			if (NULL != _n)
			{
				ptr		= _n;
				v		= _n->v;
				n		= _n->n;
				p		= _n->p;
				vidx	= _n->vidx;
			}
			else
			{
				ptr		= NULL;
				v		= NULL;
				n		= NULL;
				p		= NULL;
				vidx	= -1;
			}
			return (*this);
		}
		// let the real item's value of the l-valued node.
		node& operator=(VT value) { v = value; return (*this); }

		BOOL operator!=(node& _n) { return !((*this) == _n); }
		BOOL operator==(node& _n) { return ( ptr == _n.ptr ); }
		BOOL operator!=(VT value) { return !( (*this) == value ); }
		BOOL operator==(VT value) { return (ptr->v == value); }
		BOOL operator==(node* _n) { return ((*this) == *_n); }
		BOOL operator!=(node* _n) { return !((*this) == *_n); }
		node& operator++()		// prefix
		{ 
			(*this)++;			// toss to postfix
			return (*this); 
		}
		node& operator++(int)	// postfix
		{
			if (NULL != n)
			{
				vidx++;
				ptr		= n->ptr;
				v		= n->v;
				p		= n->p;
				n		= n->n;
				return (*this);
			}

			ptr		= NULL;
			n		= NULL;
			v		= NULL;
			return (*this);
		}
		node& operator--() { (*this)--; return (*this); }
		node& operator--(int)
		{
			if (NULL != p)
			{
				vidx--;
				ptr		= p->ptr;
				v		= p->v;
				n		= p->n;
				p		= p->p;
				return (*this);
			}

			ptr		= NULL;
			n		= NULL;
			v		= NULL;
			return (*this);
		}

		VT operator*()	{ return (ptr) ? (ptr->v) : NULL; }
		VT operator->() { return (ptr) ? ((*this).v) : NULL; }

		BOOL isend() { return (NULL == ptr); }
	};


	sys_list();
	~sys_list();
	

	// add-Before
	node& addbefore(VT value);
	// insert value after the last
	node& add(VT value);
	node& addunique(VT value);

	void insertbefore(node& n,  VT value);
	void insert(node& n, VT value);

	// stack style pop
	VT pop();
	// queue style pop
	VT qpop();

	VT qpeek();
	VT peek();


	node& front();
	node& back();

	BOOL isempty();

	BOOL remove(node& n);
	BOOL remove(VT v);
	BOOL remove(long nth);
	BOOL removefront();
	BOOL removeback();
	void removeall();
	void reverse();

	void remap()
	{
		modified = FALSE;

		if ( radim )
		{
			SYSLIST_DEL( radim );
			radim = NULL;
		}

		if ( 0 == num )
			return;

		assert( pFirst );

		radim = SYSLIST_NEW( VT, num );
		node* tmp = pFirst;
		for (int i=0; i<num; i++)
		{
#ifdef USECACHE
			cache[ FHash( *(int*)&(tmp->v) ) % cachesize ] = tmp;
#endif
			radim[i] = tmp->v;
			tmp = tmp->n;
		}
	}
	VT operator[](int x)
	{
		//assert( pFirst && x >= 0 && x < num );
		if ( NULL == radim || modified )
			remap();
		return radim[x];
	}
	node find( VT v )	// return by value, not a reference. 
	{
		if ( 0 == num )
			return NULL;

//		assert( pFirst );
#ifdef USECACHE
		node* hititem = cache[ FHash( *(int*)&v ) % cachesize ];
		if ( NULL == hititem )
			return NULL;

		if ( hititem->v == v )
			return hititem;
#endif
		node* tmp = pFirst;
		for (int i=0; i<num; i++)
		{
			if ( tmp->v == v )
				return tmp;
			tmp = tmp->n;
		}

		return NULL;
	}
	node getnode( int nth )	// return by value, not a reference. 
	{
		node* tmp = pFirst;
		for (int i=0; i<nth; i++)
			tmp = tmp->n;
		return tmp;
	}

public:
	int			num;

private:
	node**		cache;
	VT*			radim;
	node*		pFirst;
	node*		pLast;
	BOOL		modified;

private:
	node* _makeitem(VT value);
	void _chkvalidate();
};


template <class VT>
sys_list<VT>::sys_list()
{
	pFirst			= NULL;
	pLast			= NULL;
	num				= 0;
	radim			= NULL;
	modified		= FALSE;
#ifdef USECACHE
	cache			= SYSLIST_NEW( node, cachesize );
	memset(	cache, 0, sizeof( node* ) * cachesize );
#endif
}


template <class VT>
sys_list<VT>::~sys_list()
{
//	removeall();
#ifdef USECACHE
	SYSLIST_DEL( cache );
#endif
}


template <class VT>
typename sys_list<VT>::node* sys_list<VT>::_makeitem(VT value)
{
	node* item = SYSLIST_NEW( node, 1 );
	item->ptr = item;
	item->v = value;
	item->p = NULL;
	item->n = NULL;
	item->vidx = 0;
	modified = TRUE;
	return item;
}


template <class VT>
typename sys_list<VT>::node& sys_list<VT>::addbefore(VT value)
{
	node* item = _makeitem(value);

	if (NULL == pFirst)
	{
		pFirst = item;
		pLast = item;
	}
	else
	{
		item->n = pFirst;
		pFirst->p = item;
		pFirst = item;
	}

	num++;

	return *item;
}


template <class VT>
typename sys_list<VT>::node& sys_list<VT>::add(VT value)
{
	node* item = _makeitem(value);

	if (NULL == pFirst)
	{
		pFirst = item;
		pLast = item;
	}
	else
	{
		pLast->n = item;
		item->p = pLast;
		pLast = item;
	}

	num++;
	return *item;
}


template <class VT>
typename sys_list<VT>::node& sys_list<VT>::addunique(VT value)
{
	static node null;
	if ( find( value ) )
		return null;
	return add(value);
}


template <class VT>
void sys_list<VT>::insertbefore(node& n, VT value)
{
	node* item = _makeitem(value);

	if (n == pFirst || NULL == pFirst)
	{
		addbefore(value);
		return;
	}

	item->n = n;
	item->p = n->p;
	if (NULL != n->p)
		n->p->n = item;
	n->p = item;
}

template <class VT>
void sys_list<VT>::insert(node& n, VT value)
{
	node* item = _makeitem(value);

	if (n == pLast || NULL == pFirst)
	{
		add(value);
		return;
	}

	item->p = n;
	item->n = n->n;
	n->n = item;
	if (NULL != n->n)
		n->n->p = item;
}


template <class VT>
void sys_list<VT>::_chkvalidate()
{
	assert( num >= 0 );

	if (0 == num)
	{
		pFirst = NULL;
		pLast = NULL;

		SYSLIST_DEL( radim );
		radim = NULL;
		modified = FALSE;
	}
}


template <class VT>
BOOL sys_list<VT>::removefront()
{
	node* tmp;

	if (NULL == pFirst)
		return FALSE;

	if (pFirst != pLast)
		pFirst->n->p = NULL;

	tmp = pFirst->n;

#ifdef USECACHE
	cache[ FHash( *(int*)&(pFirst->v) ) % cachesize ] = NULL;
#endif

	SYSLIST_DEL( pFirst );
	pFirst = tmp;
	num--;
	modified = TRUE;

	_chkvalidate();
	return TRUE;
}


template <class VT>
BOOL sys_list<VT>::removeback()
{
	node* tmp;

	if (NULL == pFirst)
		return FALSE;

	if (pFirst != pLast)
		pLast->p->n = NULL;

	tmp = pLast->p;

#ifdef USECACHE
	cache[ FHash( *(int*)&(pLast->v) ) % cachesize ] = NULL;
#endif

	SYSLIST_DEL( pLast );
	pLast = tmp;
	num--;
	modified = TRUE;

	_chkvalidate();
	return TRUE;
}



template <class VT>
BOOL sys_list<VT>::remove(node& n)
{
	if (NULL == pFirst)
		return FALSE;

	if (n == pFirst)
		return removefront();

	if (n == pLast)
		return removeback();

	if (NULL != n.p)
		n.p->n = n.n;

	if (NULL != n.n)
		n.n->p = n.p;

#ifdef USECACHE
	cache[ FHash( *(int*)&(n->v) ) % cachesize ] = NULL;
#endif

	SYSLIST_DEL( n.ptr );
	num--;
	modified = TRUE;

	_chkvalidate();
	return TRUE;
}


template <class VT>
BOOL sys_list<VT>::remove(VT v)
{
	node n = find( v );
	return remove( n );
}

template <class VT>
BOOL sys_list<VT>::remove(long nth)
{
	return remove( getnode( nth ) );
}

template <class VT>
void sys_list<VT>::removeall()
{
	node* tmp;

	if (NULL == pFirst)
		return;

	while (1)
	{
		tmp = pFirst;

		pFirst = pFirst->n;

		if (NULL == pFirst)
		{
			SYSLIST_DEL( tmp );
			break;
		}

		pFirst->p = NULL;
		SYSLIST_DEL( tmp );
	}

	pFirst = NULL;
	pLast = NULL;
	num = 0;

	SYSLIST_DEL( radim );
	radim	= NULL;
	modified = FALSE;

#ifdef USECACHE
	memset( cache, 0, sizeof( node* ) * cachesize );
#endif
}


template <class VT>
typename sys_list<VT>::node& sys_list<VT>::front()
{
	static node null;
	if ( NULL == pFirst )
		return null;

	return (*pFirst);
}

template <class VT>
typename sys_list<VT>::node& sys_list<VT>::back()
{
	static node null;
	if ( NULL == pLast )
		return null;

	return (*pLast);
}

template <class VT>
typename BOOL sys_list<VT>::isempty()
{
	return NULL == pFirst;
}


template <class VT>
typename VT sys_list<VT>::qpeek()
{
	if (NULL != pFirst)
	{
		return pFirst->v;
	}

	return (VT)NULL;
}


template <class VT>
typename VT sys_list<VT>::peek()
{
	if (NULL != pFirst)
	{
		return pLast->v;
	}

	return (VT)NULL;
}


template <class VT>
typename VT sys_list<VT>::qpop()
{
	VT v = (VT)NULL;

	if (NULL != pFirst)
	{
		v = pFirst->v;
		removefront();
	}

	return v;
}


template <class VT>
typename VT sys_list<VT>::pop()
{
	VT v = (VT)NULL;

	if (NULL != pFirst)
	{
		v = pLast->v;
		removeback();
	}

	return v;
}


template <class VT>
typename void sys_list<VT>::reverse()
{
	node*	n = front();
	node*	tmp;

	if (NULL == pFirst)
		return;

	while(1)
	{
		tmp = n->p;
		n->p = n->n;
		n->n = tmp;
		if (n == pLast) break;
		n = n->p;	// p has 'n' ptr
	}

	tmp = pFirst;
	pFirst = pLast;
	pLast = tmp;

	modified = TRUE;
}

#ifdef USECACHE
#undef cachesize
#endif

#endif

