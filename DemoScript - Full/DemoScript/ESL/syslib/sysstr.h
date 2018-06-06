

/*  
	String Class V1R2, (c) 2006-2007 lollab. All rights reserved.
	Jinhong Kim <windgram@lollab.com>

	Revision History:
	UPDATE DATE		WHO			ACT			DESCRIPTION
	----------------------------------------------------------------------------------------------------------
	2006-Dec-12		JHKIM		add			w2m() added
	2006-Dec-20		JHKIM		add			m2w() added
	2007-Feb-15		JHKIM		add			getpathonly, getfilenameonly
*/


#ifndef SYSSTR_H
#define SYSSTR_H

#include "syslist.h"
#include "stdio.h"
#include "shellapi.h"

inline void _sysstr_internal_log(const char* fmt, ...)
{

	char		text[1024];			
	va_list		ap;					
	
	if (fmt == NULL)				
		return;						
	
	char uf[256];
	sprintf_s( uf, 256, "%s", fmt ); 
	va_start(ap, fmt);				
	vsprintf_s( text, 1024, uf, ap);	
	va_end(ap);						


	FILE* fp = NULL;
	if ( 0 != fopen_s( &fp, "C:/ESYSSTR.LOG", "a+t" ) )
		return;

	fprintf( fp, "%s", text );

	fclose( fp );
}


class sys_str
{
public:
	sys_str()
	{
	}
	~sys_str()
	{
	}

	static const char* tabs( int level )
	{
		static char sbuf[256];
		
		sys_str::zero( sbuf, sizeof(char)*250 );
		
		for (int i=0; i<level; i++)
			sbuf[i] = '\t';
		
		return sbuf;
	}

	static const char* reps( int n = 1, int unit = 1, char c = ' ' )
	{
		static char sbuf[256];
		
		sys_str::zero( sbuf, sizeof(char)*250 );
		
		int pos = 0;

		for (int k=0; k<n; k++)
			for (int i=0; i<unit; i++)
			{
				if ( 250 == pos )
					break;

				sbuf[pos++] = c;
			}
		
		return sbuf;
	}

	static char* getlaststr( char* str, int wlen )
	{
		int slen = sys_str::length( str );
		if ( 0 == slen )
			return NULL;

		if ( wlen > slen )
			return NULL;

		return str + slen - wlen;
	}

	static char* getlastslash( char* path )
	{
		char* ptr = getlastchar( '\\', path );
		if ( ptr )
			return ptr;

		ptr = getlastchar( '/', path );
		if ( ptr )
			return ptr;

		return NULL;
	}

	static void striplastslash( char* path )
	{
		int len = sys_str::length( path );
		if ( 0 == len )
			return;

		if ( '\\' == path[len-1] || '/' == path[len-1] )
			path[len-1] = NULL;
	}

	static char* getlastchar( char ch, char* name )
	{
		int i = sys_str::length( name ) - 1;
		if ( -1 == i )
			return NULL;

		for (; i >= 0 ; i-- )
			if ( ch == name[ i ] )
				return name+i;

		return NULL;
	}

	// NOTE: 
	// Do not use this at recursive function. 
	// The internal list can have only 1 instance. 
	// Recursive function ruins it.
	static BOOL isduplicated( int op_code, char* string )
	{
		static sys_list<char*> vlist;
		
		if ( 0 == op_code )
		{
			// reset vlist.
			while ( ! vlist.isempty() )
				delete[] vlist.pop();
			
			return TRUE;
		}
		else if ( 1 == op_code )
		{
			char* stmp = new char[256];

			if ( FALSE == sys_str::copy( stmp, 256, string ) )
			{
				delete[] stmp;
				return FALSE;
			}

			// add string data.
			vlist.add( stmp );
			
			return TRUE;
		}
		else if ( 2 == op_code )
		{
			for (int i=0; i<vlist.num; i++)
			{
				if ( sys_str::icompare( vlist[i], string ) )
				{
					isduplicated( 0, NULL );
					return TRUE;
				}
			}
		}

		isduplicated( 0, NULL );
		return FALSE;
	}

	static BOOL isduplicated( sys_list<char*>& vlist, int op_code, char* string )
	{	
		if ( 0 == op_code )
		{
			// reset vlist.
			while ( ! vlist.isempty() )
				delete[] vlist.pop();
			
			return TRUE;
		}
		else if ( 1 == op_code )
		{
			char* stmp = new char[256];

			if ( FALSE == sys_str::copy( stmp, 256, string ) )
			{
				delete[] stmp;
				return FALSE;
			}

			// add string data.
			vlist.add( stmp );
			
			return TRUE;
		}
		else if ( 2 == op_code )
		{
			for (int i=0; i<vlist.num; i++)
			{
				if ( sys_str::icompare( vlist[i], string ) )
				{
//					isduplicated( vlist, 0, NULL );
					return TRUE;
				}
			}
		}

//		isduplicated( vlist, 0, NULL );
		return FALSE;
	}

	static BOOL icompare( char* str1, char* str2 )
	{
		BOOL res = FALSE;

		__try
		{
			res = (_stricmp( str1, str2 ) == 0);
		}
		__except(GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION)
		{
			_sysstr_internal_log( "icompare: [%s, %s]\n", str1, str2 );
			res = FALSE;
		}

		return res;
	}

	static BOOL copy( char* dest, int dsize, char* src )
	{
		__try
		{
			strcpy_s( dest, dsize, src );
		}
		__except(GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION)
		{
			_sysstr_internal_log( "copy: [%s, %s]\n", dest, src );
			return FALSE;
		}

		return TRUE;
	}

	static BOOL ncopy( char* dest, int dsize, char* src, int n )
	{
		__try
		{
			strncpy_s( dest, dsize, src, n );
		}
		__except(GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION)
		{
			_sysstr_internal_log( "ncopy: [%s, %s, %d]\n", dest, src, n );
			return FALSE;
		}

		return TRUE;
	}

	static BOOL rawcopy( void* dest, void* src, int n )
	{
		__try
		{
			memcpy( dest, src, n );
		}
		__except(GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION)
		{
			_sysstr_internal_log( "rawcopy: [%s, %s, %d]\n", dest, src, n );
			return FALSE;
		}

		return TRUE;
	}

	static int length( char* str )
	{
		int len = 0;

		__try
		{
			len = (int)strlen( str );
		}
		__except(GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION)
		{
			_sysstr_internal_log( "length: [%s]\n", str );
			len = 0;
		}

		return len;
	}

	static BOOL zero( void* ptr, int len )
	{
		__try
		{
			memset( ptr, 0, len );
		}
		__except(GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION)
		{
			_sysstr_internal_log( "zero: [%X, %d]\n", ptr, len );
			return FALSE;
		}
		
		return TRUE;
	}

	static char* search( char* string, char* searchfor )
	{
		char* res = NULL;

		if ( NULL == string || NULL == searchfor )
			return NULL;

		__try
		{
			res = strstr( string, searchfor );
		}
		__except(GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION)
		{
			_sysstr_internal_log( "search: [%s, %s]\n", string, searchfor );
			res = NULL;
		}
		
		return res;
	}

	static BOOL lwr( char* str, int size )
	{
		__try
		{
			_strlwr_s( str, size );
		}
		__except(GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION)
		{
			_sysstr_internal_log( "lwr: [%s]\n", str );
			return FALSE;
		}
		
		return TRUE;
	}

	static BOOL upr( char* str, int size )
	{
		__try
		{
			_strupr_s( str, size );
		}
		__except(GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION)
		{
			_sysstr_internal_log( "upr: [%s]\n", str );
			return FALSE;
		}
		
		return TRUE;
	}

	static BOOL cat( char* dest, int dsize, char* src )
	{
		__try
		{
			strcat_s( dest, dsize, src );
		}
		__except(GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION)
		{
			_sysstr_internal_log( "cat: [%s, %s]\n", dest, src );
			return FALSE;
		}
		
		return TRUE;
	}

	static int w2m( wchar_t* wstr, char* mstr, int msize )
	{
		int i=0;

		__try
		{
			if ( NULL == wstr || NULL == mstr )
				__leave;

			for(;;)
			{
				if ( msize == i )
					__leave;

				mstr[ i ] = (char)wstr[ i ];
				if ( NULL == wstr[ i ] )
					break;
				i++;
			}
		}
		__except( GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION )
		{
		}

		return i;
	}

	static int m2w( char* mstr, wchar_t* wstr, int wsize )
	{
		int i=0;

		__try
		{
			if ( NULL == wstr || NULL == mstr )
				__leave;

			for(;;)
			{
				if ( wsize == i )
					__leave;

				wstr[ i ] = (wchar_t)mstr[ i ];
				if ( NULL == mstr[ i ] )
					break;
				i++;
			}
		}
		__except( GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION )
		{
		}

		return i;
	}

	static void getpathonly( char* fullpath, char* path )
	{
		char* lastslashptr = sys_str::getlastslash( fullpath );
		if ( NULL == lastslashptr )
		{
			path[0] = NULL;
			return;
		}

		sys_str::ncopy( path, 256, fullpath, (int)(lastslashptr - fullpath + 1) );
		path[ (int)(lastslashptr - fullpath + 1) ] = NULL;
	}

	static void getfilenameonly( char* fullpath, char* fname )
	{
		char* lastslashptr = sys_str::getlastslash( fullpath );
		if ( NULL == lastslashptr )
		{
			sys_str::copy( fname, 256, fullpath );
			return;
		}

		sys_str::copy( fname, 256, lastslashptr+1 );
	}
};


#endif

