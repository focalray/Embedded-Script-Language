
///******************* LEGACY CODE: DO NOT EDIT ********************///

/*
** Tokenizer, 2001-2009, Jinhong Kim, marujet@gmail.com
*/


#include <windows.h>
#include <stdio.h>
#include "syslist.h"
#include "tokenizer.h"
#include "DataStream.h"


char buff[256];

Tokenizer::Tokenizer()
{
	Init();
}

Tokenizer::Tokenizer(char* filename)
{
	ds.Init(filename);
	ds.OpenToRead();

	fs = ds.GetSize();

	Init();
}

Tokenizer::~Tokenizer()
{
	ds.Close();
}

void Tokenizer::Init()
{
	ClearGroupMakerKey();
	ClearSeparatorKey();
	ClearDelimiterKey();
	ClearIgnoreKey();
	ClearNullKey();

	group_maker_list_cnt = 0;
	separator_list_cnt = 0;
	delimiter_list_cnt = 0;
	ignore_list_cnt = 0;
	null_list_cnt = 0;

	b_groupmaker = TRUE;
	b_separator = TRUE;
	b_delimiter = TRUE;

	cbparam = NULL;
	callbackfn = NULL;

	LoadDefaultKey();
}

void Tokenizer::SetLine( char* ptr, int len )
{
	ds.InitDirectMemoryAccess( ptr, len );
	fs = len;
}


void Tokenizer::LoadDefaultKey()
{
	ClearGroupMakerKey();
	ClearSeparatorKey();
	ClearDelimiterKey();
	ClearIgnoreKey();

	AddGroupMakerKey( '"', '"' );
	AddGroupMakerKey( '\'', '\'' );
	AddSeparatorKey( ' ' );
	AddSeparatorKey( '\t' );
	AddSeparatorKey( '\n' );
	AddSeparatorKey( '\r' );
	AddSeparatorKey( ',' );
	AddDelimiterKey( '{' );
	AddDelimiterKey( '}' );
	AddDelimiterKey( ':' );
	AddDelimiterKey( ';' );
}

void Tokenizer::SetCallback( void (*_cb)(int, char c, void*), void* param )
{
	callbackfn	= _cb;
	cbparam		= param;
}


char Tokenizer::GetGroupSymmetryKey( char k )
{
	for ( int i=0; i<group_maker_list_cnt; i++)
	{
		if ( k == ignore_list[i] )
			return 0;
		if ( k == null_list[i] )
			return 0;

		if ( b_groupmaker )
		{
			if ( k == group_maker_list[i][0] || k == group_maker_list[i][1] )
			{
				if ( k == group_maker_list[i][0] )
					return group_maker_list[i][1];
				else
					return group_maker_list[i][0];
			}
		}
	}

	return 0;
}

// return 1: group maker, 2: separator, 3: delimiter, 4: null space, 0 for no-match.
int	Tokenizer::TestTokenChar( char c, BOOL b_cb )
{
	for ( int i=0; i<group_maker_list_cnt; i++)
	{
		if ( c == ignore_list[i] )
			return 0;
		if ( c == null_list[i] )
			return 4;

		if ( b_groupmaker )
		{
			if ( c == group_maker_list[i][0] || c == group_maker_list[i][1] )
			{			
				if ( b_cb && callbackfn )
					(*callbackfn)( 1, c, cbparam );
				return 1;
			}
		}
	}

	for ( int i=0; i<separator_list_cnt; i++)
	{
		if ( c == ignore_list[i] )
			return 0;
		if ( c == null_list[i] )
			return 4;

		if ( b_separator && c == separator_list[i] )
		{
			if ( b_cb && callbackfn )
				(*callbackfn)( 2, c, cbparam );
			return 2;
		}
	}

	for ( int i=0; i<delimiter_list_cnt; i++)
	{
		if ( c == ignore_list[i] )
			return 0;
		if ( c == null_list[i] )
			return 4;

		if ( b_delimiter && c == delimiter_list[i] )
		{
			if ( b_cb && callbackfn )
				(*callbackfn)( 3, c, cbparam );
			return 3;
		}
	}

	return 0;
}


void Tokenizer::SetKeyActivation( int type, BOOL b_state )
{
	if ( 1 == type )
	{
		b_groupmaker = b_state;
		return;
	}
	if ( 2 == type )
	{
		b_separator = b_state;
		return;
	}
	if ( 3 == type )
	{
		b_delimiter = b_state;
		return;
	}
}

void Tokenizer::ClearGroupMakerKey()
{
	memset( group_maker_list, 0, sizeof(char)*MAX_ITEM*2 );
	group_maker_list_cnt = 0;
}

void Tokenizer::ClearSeparatorKey()
{
	memset( separator_list, 0, sizeof(char)*MAX_ITEM );
	separator_list_cnt = 0;
}

void Tokenizer::ClearDelimiterKey()
{
	memset( delimiter_list, 0, sizeof(char)*MAX_ITEM );
	delimiter_list_cnt = 0;
}

BOOL Tokenizer::AddGroupMakerKey( char k, char k_sym )
{
	if ( group_maker_list_cnt >= MAX_ITEM )
		return FALSE;

	group_maker_list[ group_maker_list_cnt ][0] = k;
	group_maker_list[ group_maker_list_cnt ][1] = k_sym;
	group_maker_list_cnt++;

	return TRUE;
}

BOOL Tokenizer::AddSeparatorKey( char ch )
{
	if ( separator_list_cnt >= MAX_ITEM )
		return FALSE;

	separator_list[ separator_list_cnt++ ] = ch;

	return TRUE;
}

BOOL Tokenizer::AddDelimiterKey( char ch )
{
	if ( delimiter_list_cnt >= MAX_ITEM )
		return FALSE;

	delimiter_list[ delimiter_list_cnt++ ] = ch;

	return TRUE;
}

BOOL Tokenizer::AddIgnoreKey( char ch )
{
	if ( ignore_list_cnt >= MAX_ITEM )
		return FALSE;

	ignore_list[ ignore_list_cnt++ ] = ch;

	return TRUE;
}

void Tokenizer::ClearIgnoreKey()
{
	memset( ignore_list, 0, sizeof(char)*MAX_ITEM );
	ignore_list_cnt = 0;
}

BOOL Tokenizer::AddNullKey( char ch )
{
	if ( null_list_cnt >= MAX_ITEM )
		return FALSE;

	null_list[ null_list_cnt++ ] = ch;

	return TRUE;
}

void Tokenizer::ClearNullKey()
{
	memset( null_list, 0, sizeof(char)*MAX_ITEM );
	null_list_cnt = 0;
}


char* Tokenizer::PeekNext( int* len )
{
	int pos;

	ds.Current( &pos );

	char* ptr = GetNext( len );

	ds.Seek( pos );

	return ptr;
}

char* Tokenizer::PeekPtr()
{
	char* raw = (char*)ds.GetRawCurPtr<char>();
	return raw;
}

char* Tokenizer::GetNext(int* len)
{
	char	c					= NULL;
	char	cn					= NULL;
	int		k					= 0;
	BOOL	inStr				= FALSE;
	BOOL	inTok				= FALSE;
	int		tokentype			= 0;
	int		tokentype_next		= 0;
	char	group_key_closer	= 0;
	BOOL	b_group_key_include = FALSE;
	
	while(1)
	{
		// EOF process
		DWORD ret = ds.GetData<char>(&c);
		if(0 == ret || 0 == c)
		{
			if ( k > 0 )
			{
				buff[k]		= NULL;
				*len		= k;					
				k			= 0;
				inTok		= FALSE;	// token is end.
				
				if ( callbackfn )
					(*callbackfn)( 9, c, cbparam );

				return buff;
			}
			
			return NULL;
		}

		tokentype		= TestTokenChar( c, TRUE );

		// null-space
		if ( 4 == tokentype )
		{
			continue;
		}

		char* p_next = (char*)ds.GetRawCurPtr<char>();
		if ( p_next )
			cn = *p_next;
		tokentype_next	= TestTokenChar( cn, FALSE );


		// group
		if ( 1 == tokentype )
		{
			if ( FALSE == inStr )
			{
				group_key_closer = GetGroupSymmetryKey( c );
				inStr = TRUE;

				// check delimiter list:
				// if the group key is exist delimiter list also,
				// tokenizer must include the key too.
				for ( int i=0; i<delimiter_list_cnt; i++)
				{
					if ( c == delimiter_list[i] )
					{
						b_group_key_include = TRUE;
						break;
					}
				}
			}
			else
			{
				if ( c == group_key_closer )
				{
					group_key_closer = 0;
					inStr = FALSE;
				}
			}
			
			if ( FALSE == inStr )
			{
				if ( b_group_key_include )
				{
					buff[k]				= c;
					k++;
					b_group_key_include = FALSE;
				}

				buff[k]				= NULL;
				*len				= k;					
				k					= 0;
				inTok				= FALSE;	// token is end.
				
				if ( callbackfn )
					(*callbackfn)( 9, c, cbparam );

				return buff;
			}

			if ( FALSE == b_group_key_include )
				continue;
		}
		

		if ( FALSE == inStr && 2 == tokentype )
		{
			if ( inTok )
			{
				// complete the buffer. make it a string.
				buff[k]		= NULL;
				*len		= k;
				k			= 0;
				inTok		= FALSE;	// token is end.
				
				if ( callbackfn )
					(*callbackfn)( 9, c, cbparam );

				return buff;
			}
		}
		else
		{
			inTok	= TRUE;
			buff[k] = c;
			k++;

			// delimiter is regarded as a single-discrete-word even if it was only a part of a word.
			if ( !inStr && (3 == tokentype || 3 == tokentype_next) )
			{
				buff[k]		= NULL;
				*len		= k;					
				k			= 0;
				inTok		= FALSE;	// token is end.
				
				if ( callbackfn )
					(*callbackfn)( 9, c, cbparam );

				return buff;
			}
		}
	}

	return NULL;
}


