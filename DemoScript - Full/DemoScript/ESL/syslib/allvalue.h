

/*  
	All-Value Class V1R0, (c) 2006-2007 lollab. All rights reserved.
	Jinhong Kim
	windgram@lollab.com

	Revision History:
	UPDATE DATE		WHO			ACT			DESCRIPTION
	----------------------------------------------------------------------------------------------------------
	2006-Apr-03		JHKIM		generated
	2007-May-28		JHKIM		add			supports: float, short, byte
*/


#ifndef _ALLVALUE_H
#define _ALLVALUE_H

#include "stdio.h"
#include "stdarg.h"

enum AVTYPE
{
	AV_STRING = 0,
	AV_DOUBLE,
	AV_FLOAT,
	AV_INT,
	AV_SHORT,
	AV_BYTE,
	AV_BOOL,
	AV_VOIDPTR
};

struct ALLVALUE
{
	union
	{
		char				string[256];
		double				dbl_val;	
		int					int_val;	
		BOOL				bool_val;	
		void*				void_ptr;
		unsigned char		byte_val;
		unsigned short int	short_val;
	};

	AVTYPE	type;

	void set_stringf( const char* fmt, ... )
	{
		char text[256];	
		va_list		ap;					

		if (fmt == NULL)				
			return;						

		va_start(ap, fmt);				
		vsprintf_s(text, 256, fmt, ap);	
		va_end(ap);						

		set_string( text );
	}

	void set_string( char* val )
	{
		if ( NULL == val )
			return;
		strcpy_s( string, 256, val );
		type = AV_STRING;
	}
	void set_double( double val )
	{
		dbl_val = val;
		type = AV_DOUBLE;
	}
	void set_float( double val )
	{
		dbl_val = val;
		type = AV_FLOAT;
	}
	void set_int( int val )
	{
		int_val = val;
		type = AV_INT;
	}
	void set_short( int val )
	{
		int_val = val;
		type = AV_SHORT;
	}
	void set_byte( int val )
	{
		int_val = val;
		type = AV_BYTE;
	}
	void set_bool( BOOL val )
	{
		bool_val = val;
		type = AV_BOOL;
	}
	void set_voidptr( void* val )
	{
		void_ptr = val;
		type = AV_VOIDPTR;
	}
};


#endif

