
/*
	Citrus - Embedded Script Language - HBVM 1.4.0

	Copyright by 2007-2011 HaangilSoft. All rights reserved.
	Jinhong Kim <Redperf@gmail.com>
	
	<REMARK>

	A small embedded script language specially designed for I/G.


	Revision History:
	UPDATE DATE		WHO			ACT			DESCRIPTION
	----------------------------------------------------------------------------------------------------------
	2007-May-09		JHKIM		build		The first release.
	2007-May-15		JHKIM		add			unary operator -
								change		runtime code optimization.
	2007-May-22		JHKIM		add			#include support.
											IGVM_set_verbatim_handler() added: can be used to read xml data.
											exp() added.
	2007-May-23		JHKIM		add			Script: GETPORTSIZE, MAPPORT, READPORT, WHILE loop support.
	2007-May-25		JHKIM		add			Script: RETURN
								fix			IF-ELSE-ENDIF: supports nested block.
	2007-May-31		JHKIM		add			#PORTPACK, GETOFFSET added.
	2007-Jun-01		JHKIM		add			Operator % support.  
	2007-Jun-18		JHKIM		fix			Fixed some bugs
											Supports READSTACK, POPAWAY
											Supports Call-by-reference facility.
											Changes code style: more similar to C.
	2007-Jun-19		JHKIM		add			Added LOADMODULE, UNLOADMODULE, GETPROC, CALLPROC
								fix			string and string array copy problem.
	2007-Jun-20		JHKIM		fix			I_CALL bug fixed. 
											Added '$' ( Same to CALL )
	2007-Jun-21		JHKIM		fix			Verbatim mode errors: couldn't correctly recognize closing mark.
								fix			I_RETURN also does CALL-BY-VALUE.
	2007-Jun-22		JHKIM		add			Added Bind functions.
	2007-Jun-27		JHKIM		fix			An error of read_operand function for binding variable.
								fix			Wrong memory alignment.
	2007-Jul-13		JHKIM		add			Binary format export, import support.
	2007-Jul-23		JHKIM		rename		'OUTCALL' has been renamed to 'EVENT'
	2007-Sep-20		JHKIM		fix			"r = call foo a,b"  error: priority sequence problem.
								modify		'$' is now identical to the CALLPROC. ( 'CALLPROC' is obsoleted )
											And, '$' is no more means 'CALL' 
								fix			"r = $foo a,b" is available too. 
	2007-Oct-15		JHKIM		add			Added 'vm_getfunc' to get the function entry ip.
								fix			'vm_run' now can have start_ip number. So that you can
											call another function dynamically and can get the return
											value from it.
	2007-Oct-16		JHKIM		change		Applied: HBVM has been wrapped with a namespace 'ddvm'
											Changed: IGVM_API -> DDVM_API
											Changed: vm_XXXX -> XXXX
											Changed: vm_run -> call
								fix			'pop' error: wrong string var substituting.
	2007-Oct-17		JHKIM		fix			Passing a value in array via parameters is now available for "$" and "CALL". (v1.2)
	2008-Oct-27		JHKIM		Renewal		Converted HBVM API set into CCitrus class to support multiple instances. (v1.3)
	2008-Oct-28		JHKIM		Fix			CCitrus::call's exit condition has an error.
													VM has been exit without processing statements when the main() has no calls.
													So, changed the exit condition :

													from:
													if ( -1 == r || 1 == r )

													to:
													if ( -1 == r || (1 == r && TOK_ENDPROC == vmi->type) )
	2011-Aug-10		JHKIM		Modified	Some bug fixed and some funtionalities added.

*/


#include "ESL.h"

void (_stdcall *CCitrus::_vm_callback)(VMINT, VMINT);

CCitrus::CCitrus()
{
	vm_r0			= 0.0;
	vm_datapack		= 4;
	vm_exit_code	= 0;
	vm_procret		= 0;

	vm_script_path[0] = NULL;
	sys_str::copy(vm_entry_name, 256, "main");
	b_in_array_brace_at_unknown = FALSE;
}

CCitrus::~CCitrus()
{
}


#include "getkid.h"


VMITEM* CCitrus::make_item( TOKTYPE type, char* name, VMINT sid, BOOL b_op, VMINT debug_line )
{
	VMITEM* t = new VMITEM;
	t->type			= type;
	sys_str::copy( t->name, 256, name );
	t->sid			= sid;
	t->pri			= 0;
	t->spec			= NULL;
	t->b_op			= b_op;
	t->rtf			= type;
	vm_items.add( t );
	t->pos			= vm_items.num - 1;
	t->debug_line	= debug_line;
	t->left			= NULL;
	t->left_op		= NULL;
	t->right		= NULL;
	t->right_op		= NULL;

	t->cls.cconst		= 0;
	t->cls.carrayidx	= b_in_array_brace_at_unknown ? 1:0;
	t->cls.ckeyword		= 0;
	t->cls.carray		= 0;
	t->cls.clinkable	= 1;

	if (TOK_VAR_INT == type || TOK_VAR_WORD == type || TOK_VAR_SHORT == type || 
		TOK_VAR_BYTE == type || TOK_VAR_INT64 == type || TOK_VAR_DWORD == type)
	{
		t->cls.cls		= VMCLS_I;
	}
	else if ( TOK_CONST_INT == type )
	{
		t->cls.cls		= VMCLS_I;
		t->cls.cconst	= 1;
	}
	else if ( TOK_VAR_FLOAT == type || TOK_VAR_DOUBLE == type )
	{
		t->cls.cls		= VMCLS_F;
	}
	else if ( TOK_CONST_DOUBLE == type )
	{
		t->cls.cls		= VMCLS_F;
		t->cls.cconst	= 1;
	}
	else if ( TOK_VAR_STR == type )
	{
		t->cls.cls		= VMCLS_S;
	}
	else if ( TOK_CONST_STR == type )
	{
		t->cls.cls		= VMCLS_S;
		t->cls.cconst	= 1;
	}
	else if ( TOK_VAR_FORMAT == type )
	{
		t->cls.cls		= VMCLS_O;
	}
	else
	{
		t->cls.ckeyword	= 1;
		t->cls.cconst	= 1;	// all keyword are assumed as a constant.
	}

	return t;
}

VMINT CCitrus::make_vm_var_format( VMITEM* t, FORMATITEM* fi, int size, VMINT repeat )
{
	VMASSERT2( -1 == get_var_sid( t->name ), EC_1004_REDEFVAR, t );

	VARITEM* v = new VARITEM;

	sys_str::copy( v->name, 256, t->name );
	t->sid				= varlist.num;
	v->tok_type			= t->type;
	v->cls				= t->cls;
	v->repeat			= repeat;
	v->val.set_voidptr( (void*)fi );
	v->cls.cformat		= 1;

	v->realaddr		= new char[size*repeat];
	memset( v->realaddr, 0, sizeof(char)*size*repeat );

	if ( repeat > 1 )
	{
		t->cls.carray	= 1;
		v->cls.carray	= 1;
	}

	varlist.add( v );

	return t->sid;
}

VMINT CCitrus::make_vm_var_i( VMITEM* t, VMINT initval, VMINT repeat, BOOL b_format_inst )
{
	VMASSERT2( -1 == get_var_sid( t->name ), EC_1004_REDEFVAR, t );

	int sid = varlist.num;

	VARITEM* v = new VARITEM;
	sys_str::copy( v->name, 256, t->name );
	t->sid				= sid;		// for array, sid means a base index.
	v->tok_type			= t->type;
	v->cls				= t->cls;
	v->repeat			= repeat;
	v->realaddr			= NULL;
	v->cls.cformat		= b_format_inst ? 1:0;
	v->bind				= NULL;

	if ( repeat > 1 )
	{
		t->cls.carray	= 1;
		v->cls.carray	= 1;
		v->realaddr		= new VMINT[ repeat ];
		memset( v->realaddr, 0, sizeof(VMINT)*repeat );
	}
	else
		v->val.set_int( initval );

	varlist.add( v );

	return sid;
}

VMINT CCitrus::make_vm_var_bind_i( const char* varname, int* bindvar, VMINT repeat )
{
	VMASSERT( -1 == get_var_sid( (char*)varname ), EC_1004_REDEFVAR );

	int sid = varlist.num;

	VARITEM* v = new VARITEM;
	sys_str::copy( v->name, 256, (char*)varname );
	v->tok_type			= TOK_VAR_INT;
	v->repeat			= repeat;
	v->realaddr			= NULL;
	v->bind				= (void*)bindvar;

	v->cls.cls			= VMCLS_I;
	v->cls.cformat		= 0;
	v->cls.cconst		= 0;
	v->cls.carrayidx	= 0;
	v->cls.ckeyword		= 0;
	v->cls.carray		= 0;
	v->cls.clinkable	= 1;

	if ( repeat > 1 )
	{
		v->cls.carray	= 1;
		v->realaddr		= new VMINT[ repeat ];
		memset( v->realaddr, 0, sizeof(VMINT)*repeat );
	}
	else
		v->val.set_int( *bindvar );

	varlist.add( v );

	return sid;
}


VMINT CCitrus::make_vm_var_f( VMITEM* t, VMDOUBLE initval, VMINT repeat, BOOL b_format_inst )
{
	VMASSERT2( -1 == get_var_sid( t->name ), EC_1004_REDEFVAR, t );

	int sid = varlist.num;

	VARITEM* v = new VARITEM;
	sys_str::copy( v->name, 256, t->name );
	t->sid				= sid;
	v->tok_type			= t->type;
	v->cls				= t->cls;
	v->repeat			= repeat;
	v->realaddr			= NULL;
	v->bind				= NULL;
	v->cls.cformat		= b_format_inst ? 1:0;

	if ( repeat > 1 )
	{
		t->cls.carray	= 1;
		v->cls.carray	= 1;
		v->realaddr		= new VMDOUBLE[ repeat ];
		memset( v->realaddr, 0, sizeof(VMDOUBLE)*repeat );
	}
	else
		v->val.set_double( initval );

	varlist.add( v );

	return sid;
}

VMINT CCitrus::make_vm_var_bind_f( const char* varname, double* bindvar, VMINT repeat )
{
	VMASSERT( -1 == get_var_sid( (char*)varname ), EC_1004_REDEFVAR );

	int sid = varlist.num;

	VARITEM* v = new VARITEM;
	sys_str::copy( v->name, 256, (char*)varname );
	v->tok_type			= TOK_VAR_DOUBLE;
	v->repeat			= repeat;
	v->realaddr			= NULL;
	v->bind				= (void*)bindvar;

	v->cls.cls			= VMCLS_F;
	v->cls.cformat		= 0;
	v->cls.cconst		= 0;
	v->cls.carrayidx	= 0;
	v->cls.ckeyword		= 0;
	v->cls.carray		= 0;
	v->cls.clinkable	= 1;

	if ( repeat > 1 )
	{
		v->cls.carray	= 1;
		v->realaddr		= new VMDOUBLE[ repeat ];
		memset( v->realaddr, 0, sizeof(VMDOUBLE)*repeat );
	}
	else
		v->val.set_double( *bindvar );

	varlist.add( v );

	return sid;
}


VMINT CCitrus::make_vm_var_str( VMITEM* t, char* initval, VMINT repeat, BOOL b_format_inst )
{
	VMASSERT2( -1 == get_var_sid( t->name ), EC_1004_REDEFVAR, t );

	int sid = varlist.num;

	VARITEM* v = new VARITEM;
	sys_str::copy( v->name, 256, t->name );
	t->sid				= sid;
	v->tok_type			= t->type;
	v->cls				= t->cls;
	v->repeat			= repeat;
	v->realaddr			= NULL;
	v->bind				= NULL;
	v->cls.cformat		= b_format_inst ? 1:0;

	if ( repeat > 1 )
	{
		t->cls.carray	= 1;
		v->cls.carray	= 1;
		v->realaddr		= new char[ 256*repeat ];
		memset( v->realaddr, 0, 256*repeat );
	}
	else
		v->val.set_string( initval );

	varlist.add( v );

	return sid;
}

VMINT CCitrus::make_vm_var_bind_str( const char* varname, const char* bindvar, VMINT repeat )
{
	VMASSERT( -1 == get_var_sid( (char*)varname ), EC_1004_REDEFVAR );

	int sid = varlist.num;

	VARITEM* v = new VARITEM;
	sys_str::copy( v->name, 256, (char*)varname );
	v->tok_type			= TOK_VAR_STR;
	v->repeat			= repeat;
	v->realaddr			= NULL;
	v->bind				= (void*)bindvar;

	v->cls.cls			= VMCLS_S;
	v->cls.cformat		= 0;
	v->cls.cconst		= 0;
	v->cls.carrayidx	= 0;
	v->cls.ckeyword		= 0;
	v->cls.carray		= 0;
	v->cls.clinkable	= 1;

	if ( repeat > 1 )
	{
		v->cls.carray	= 1;
		v->realaddr		= new char[ 256*repeat ];
		memset( v->realaddr, 0, 256*repeat );
	}
	else
		v->val.set_string( (char*)bindvar );

	varlist.add( v );

	return sid;
}

// read string var
char* CCitrus::read_vm_var_str( VMITEM* t )
{
	VMASSERT( VMCLS_S == t->cls.cls, EC_0001_RNONSTR );
	VARITEM* vi = varlist[ t->sid ];
	if ( vi->bind )
		return (char*)(vi->bind);

	ALLVALUE* v = &(vi->val);
	return v->string;
}

void CCitrus::write_vm_var_str( VMITEM* t, char* val )
{
	VMASSERT( VMCLS_S == t->cls.cls, EC_0002_WNONSTR );
	VARITEM* vi = varlist[ t->sid ];
	ALLVALUE* v = &(vi->val);
	v->set_string( val );
	if ( vi->bind )
		sys_str::copy( (char*)(vi->bind), 256, val );
}

// read float var
VMDOUBLE CCitrus::read_vm_var_f( VMITEM* t )
{
	VMASSERT( VMCLS_F == t->cls.cls, EC_0003_RNONF );
	VARITEM* vi = varlist[ t->sid ];
	if ( vi->bind )
		return *(VMDOUBLE*)(vi->bind);

	ALLVALUE* v = &(vi->val);
	return v->dbl_val;
}

void CCitrus::write_vm_var_f( VMITEM* t, VMDOUBLE val )
{
	VMASSERT( VMCLS_F == t->cls.cls, EC_0004_WNONF );
	VARITEM* vi = varlist[ t->sid ];
	ALLVALUE* v = &(vi->val);
	v->set_double( val );
	if ( vi->bind )
		*(VMDOUBLE*)(vi->bind) = val;
}


// read integer var
VMINT CCitrus::read_vm_var_i( VMITEM* t )
{
	VMASSERT( VMCLS_I == t->cls.cls, EC_0005_RNONI );
	VARITEM* vi = varlist[ t->sid ];
	if ( vi->bind )
		return *(VMINT*)(vi->bind);

	ALLVALUE* v = &(vi->val);
	return v->int_val;
}

void CCitrus::write_vm_var_i( VMITEM* t, VMINT val )
{
	VMASSERT( VMCLS_I == t->cls.cls, EC_0006_WNONI );
	VARITEM* vi = varlist[ t->sid ];
	ALLVALUE* v = &(vi->val);
	v->set_int( val );
	if ( vi->bind )
		*(VMINT*)(vi->bind) = val;
}


int CCitrus::get_array_index( VMITEM* arrayitem, VMITEM* right_array )
{
	// if the array item has no '[]', then returns an index of 0.
	if ( TOK_ARRAY_OPEN != arrayitem->type )
		return 0;

//	VMASSERT2( TOK_ARRAY_OPEN == arrayitem->type, EC_0007_NONAOI, arrayitem );
	SPEC_ARRAY_OPEN* sp_ao = (SPEC_ARRAY_OPEN*)arrayitem->spec;

	int idx = -1;
	if ( sp_ao->expr.num > 0 )
		idx = (int)vm_evaluate_expression( sp_ao->expr );
	else
	{
		if ( 1 == right_array->cls.cconst )
			idx = right_array->initval.int_val;
		else
			idx = read_vm_var_i( right_array );
	}

	return idx;
}

// evaluate function
VMDOUBLE CCitrus::read_operand( VMITEM* t )
{
	VMASSERT( t, EC_0008_ROCE );

	VMDOUBLE sign = 1.0;

	// check unary
	VMITEM* vmi = vm_items[ t->pos-1 ];
	if ( TOK_OPR_SUBTRACT_UNARY == vmi->type )
		sign = -1.0;


	if ( TOK_VAR_FORMAT == t->type )
	{
		VARITEM* vi = varlist[ t->sid ];
		char* addr = (char*)vi->realaddr;


		// fmt[a].sub1[b].sub2[c];
		// PART: fmt[a]
		if ( 1 == t->cls.carray )
		{// array format
			int idx = get_array_index( vm_items[ t->pos + 1 ], t->right_array );
			VMASSERT2( idx >= 0 && idx < varlist[ t->sid ]->repeat, EC_1001_OUTIDX, t );

			FORMATITEM* fi = (FORMATITEM*)varlist[ t->sid ]->val.void_ptr;
			addr += fi->size * idx;
		}


		VMITEM* iter = t->right_op;

		int totaloffset = 0;

		FORMAT_ELE_TYPE	etype;

		while ( 1 )
		{
			SPEC_PERIOD* sp_period = (SPEC_PERIOD*)iter->spec;
			int offset	= sp_period->fe->offset; // get the base offset.

			etype = sp_period->fe->ftype;

			if ( sp_period->fe->repeat > 1 )
			{// array element.
				int idx = get_array_index( vm_items[ iter->pos + 1 ], iter->right_array );
				VMASSERT2( idx >= 0 && idx < sp_period->fe->repeat, EC_1001_OUTIDX, t );

				offset		+= sp_period->fe->size * idx;
			}

			totaloffset += offset;

			if ( NULL == iter->right_op || TOK_PERIOD != iter->right_op->type )
				break;

			iter = iter->right_op;
		}

		addr += totaloffset;

		switch( etype )
		{
		case FMT_INT:
			{
				VMINT* peek = (VMINT*)addr;
				return sign * *peek;
			}
			break;
		case FMT_BYTE:
			{
				VMBYTE* peek = (VMBYTE*)addr;
				return sign * *peek;
			}
			break;
		case FMT_WORD:
			{
				VMWORD* peek = (VMWORD*)addr;
				return sign * *peek;
			}
			break;
		case FMT_DWORD:
			{
				VMDWORD* peek = (VMDWORD*)addr;
				return sign * *peek;
			}
			break;
		case FMT_SHORT:
			{
				VMSHORT* peek = (VMSHORT*)addr;
				return sign * *peek;
			}
			break;
		case FMT_INT64:
			{
				VMINT64* peek = (VMINT64*)addr;
				return sign * *peek;
			}
			break;
		case FMT_FLOAT:
			{
				VMFLOAT* peek = (VMFLOAT*)addr;
				return sign * *peek;
			}
			break;
		case FMT_DOUBLE:
			{
				VMDOUBLE* peek = (VMDOUBLE*)addr;
				return sign * *peek;
			}
			break;
		}

		VMASSERT2( 0, EC_1003_WTNDV, t );
	}


	// sid = -1 means a constant: a constant cannot be array. ;)
	if ( -1 != t->sid && 1 == t->cls.carray )
	{// array
		int idx = get_array_index( vm_items[ t->pos + 1 ], t->right_array );
		VMASSERT2( idx >= 0 && idx < varlist[ t->sid ]->repeat, EC_1001_OUTIDX, t );

		VARITEM* vi = varlist[ t->sid ];
		char* addr = (char*)vi->realaddr;

		if ( VMCLS_F == t->cls.cls )
		{
			VMDOUBLE* peek = (VMDOUBLE*)(addr + sizeof(VMDOUBLE)*idx);
			return sign * *peek;
		}
		else
		{
			VMINT* peek = (VMINT*)(addr + sizeof(VMINT)*idx);
			return sign * *peek;
		}
	}
	else
	{
		if ( 1 == t->cls.cconst )
		{
			if ( VMCLS_F == t->cls.cls )
				return sign * t->initval.dbl_val;
			else
				return sign * (VMDOUBLE)t->initval.int_val;
		}
		else
		{
			if ( VMCLS_F == t->cls.cls )
				return sign * read_vm_var_f( t );
			else
				return sign * (VMDOUBLE)read_vm_var_i( t );
		}
	}

	VMASSERT2( 0, EC_1003_WTNDV, t );
	return 0;
}


// evaluate function
char* CCitrus::read_operand_str( VMITEM* t )
{
	VMASSERT( t, EC_0008_ROCE );

	if ( TOK_VAR_FORMAT == t->type )
	{
		VARITEM* vi = varlist[ t->sid ];
		char* addr = (char*)vi->realaddr;

		// fmt[a].sub1[b].sub2[c];
		// PART: fmt[a]
		if ( 1 == t->cls.carray )
		{// array format
			int idx = get_array_index( vm_items[ t->pos + 1 ], t->right_array );
			VMASSERT2( idx >= 0 && idx < varlist[ t->sid ]->repeat, EC_1001_OUTIDX, t );

			FORMATITEM* fi = (FORMATITEM*)varlist[ t->sid ]->val.void_ptr;
			addr += fi->size * idx;
		}


		VMITEM* iter = t->right_op;

		int totaloffset = 0;

		FORMAT_ELE_TYPE	etype;

		while ( 1 )
		{
			SPEC_PERIOD* sp_period = (SPEC_PERIOD*)iter->spec;
			int offset	= sp_period->fe->offset; // get the base offset.

			etype = sp_period->fe->ftype;

			if ( sp_period->fe->repeat > 1 )
			{// array element.
				int idx = get_array_index( vm_items[ iter->pos + 1 ], iter->right_array );
				VMASSERT2( idx >= 0 && idx < sp_period->fe->repeat, EC_1001_OUTIDX, t );

				offset		+= sp_period->fe->size * idx;
			}

			totaloffset += offset;

			if ( NULL == iter->right_op || TOK_PERIOD != iter->right_op->type )
				break;

			iter = iter->right_op;
		}

		addr += totaloffset;

		if ( FMT_STR == etype )
			return (char*)addr;

		VMASSERT2( 0, EC_1003_WTNDV, t );
	}

	// sid = -1 means a constant: a constant cannot be array. ;)
	if ( -1 != t->sid && 1 == t->cls.carray )
	{// array
		int idx = get_array_index( vm_items[ t->pos + 1 ], t->right_array );
		VMASSERT2( idx >= 0 && idx < varlist[ t->sid ]->repeat, EC_1001_OUTIDX, t );

		VARITEM* vi = varlist[ t->sid ];
		char* addr = (char*)vi->realaddr;

		if ( VMCLS_S == t->cls.cls )
			return (char*)(addr + 256*idx);
	}
	else
	{
		if ( 1 == t->cls.cconst )
		{
			if ( VMCLS_S == t->cls.cls )
				return t->initval.string;
		}
		else
		{
			if ( VMCLS_S == t->cls.cls )
			{
				return read_vm_var_str( t );
			}
		}
	}

	VMASSERT2( 0, EC_1003_WTNDV, t );
	return 0;
}

void CCitrus::write_operand( VMITEM* t, VMDOUBLE v )
{
	VMASSERT( t, EC_0009_WOCE );

	if ( 1 == t->cls.cconst )
		VMASSERT( 0, EC_1002_CWTC );


	if ( TOK_VAR_FORMAT == t->type )
	{
		VARITEM* vi = varlist[ t->sid ];
		char* addr = (char*)vi->realaddr;


		// fmt[a].sub1[b].sub2[c];
		// PART: fmt[a]
		if ( 1 == t->cls.carray )
		{// array format
			int idx = get_array_index( vm_items[ t->pos + 1 ], t->right_array );
			VMASSERT2( idx >= 0 && idx < varlist[ t->sid ]->repeat, EC_1001_OUTIDX, t );

			FORMATITEM* fi = (FORMATITEM*)varlist[ t->sid ]->val.void_ptr;		
			addr += fi->size * idx;
		}


		VMITEM* iter = t->right_op;

		int totaloffset = 0;

		FORMAT_ELE_TYPE	etype;

		while ( 1 )
		{
			SPEC_PERIOD* sp_period = (SPEC_PERIOD*)iter->spec;
			int offset	= sp_period->fe->offset; // get the base offset.

			etype = sp_period->fe->ftype;

			if ( sp_period->fe->repeat > 1 )
			{// array element.
				int idx = get_array_index( vm_items[ iter->pos + 1 ], iter->right_array );
				VMASSERT2( idx >= 0 && idx < sp_period->fe->repeat, EC_1001_OUTIDX, t );

				offset		+= sp_period->fe->size * idx;
			}

			totaloffset += offset;

			if ( NULL == iter->right_op || TOK_PERIOD != iter->right_op->type )
				break;

			iter = iter->right_op;
		}

		addr += totaloffset;

		switch( etype )
		{
		case FMT_INT:
			{
				VMINT* peek = (VMINT*)addr;
				*peek = (VMINT)v;
			}
			break;
		case FMT_BYTE:
			{
				VMBYTE* peek = (VMBYTE*)addr;
				*peek = (VMBYTE)v;
			}
			break;
		case FMT_WORD:
			{
				VMWORD* peek = (VMWORD*)addr;
				*peek = (VMWORD)v;
			}
			break;
		case FMT_DWORD:
			{
				VMDWORD* peek = (VMDWORD*)addr;
				*peek = (VMDWORD)v;
			}
			break;
		case FMT_SHORT:
			{
				VMSHORT* peek = (VMSHORT*)addr;
				*peek = (VMSHORT)v;
			}
			break;
		case FMT_INT64:
			{
				VMINT64* peek = (VMINT64*)addr;
				*peek = (VMINT64)v;
			}
			break;
		case FMT_FLOAT:
			{
				VMFLOAT* peek = (VMFLOAT*)addr;
				*peek = (VMFLOAT)v;
			}
			break;
		case FMT_DOUBLE:
			{
				VMDOUBLE* peek = (VMDOUBLE*)addr;
				*peek = v;
			}
			break;
		}

		return;
	}

	
	if ( 1 == t->cls.carray && -1 != t->sid )
	{// array
		int idx = get_array_index( vm_items[ t->pos + 1 ], t->right_array );
		VMASSERT2( idx >= 0 && idx < varlist[ t->sid ]->repeat, EC_1001_OUTIDX, t );

		VARITEM* vi = varlist[ t->sid ];
		char* addr = (char*)vi->realaddr;


		if ( VMCLS_F == t->cls.cls )
		{
			VMDOUBLE* peek = (VMDOUBLE*)(addr + sizeof(VMDOUBLE)*idx);
			*peek = v;
		}
		else
		{
			VMINT* peek = (VMINT*)(addr + sizeof(VMINT)*idx);
			*peek = (VMINT)v;
		}
	}
	else
	{
		if ( VMCLS_F == t->cls.cls )
			write_vm_var_f( t, v );
		else
			write_vm_var_i( t, (VMINT)v );
	}
}


void CCitrus::write_operand_str( VMITEM* t, char* v )
{
	VMASSERT( t, EC_0009_WOCE );

	if ( 1 == t->cls.cconst )
		VMASSERT( 0, EC_1002_CWTC );

	if ( TOK_VAR_FORMAT == t->type )
	{
		VARITEM* vi = varlist[ t->sid ];
		char* addr = (char*)vi->realaddr;

		// fmt[a].sub1[b].sub2[c];
		// PART: fmt[a]
		if ( 1 == t->cls.carray )
		{// array format
			int idx = get_array_index( vm_items[ t->pos + 1 ], t->right_array );
			VMASSERT2( idx >= 0 && idx < varlist[ t->sid ]->repeat, EC_1001_OUTIDX, t );

			FORMATITEM* fi = (FORMATITEM*)varlist[ t->sid ]->val.void_ptr;		
			addr += fi->size * idx;
		}


		VMITEM* iter = t->right_op;

		int totaloffset = 0;

		FORMAT_ELE_TYPE	etype;

		while ( 1 )
		{
			SPEC_PERIOD* sp_period = (SPEC_PERIOD*)iter->spec;
			int offset	= sp_period->fe->offset; // get the base offset.

			etype = sp_period->fe->ftype;

			if ( sp_period->fe->repeat > 1 )
			{// array element.
				int idx = get_array_index( vm_items[ iter->pos + 1 ], iter->right_array );
				VMASSERT2( idx >= 0 && idx < sp_period->fe->repeat, EC_1001_OUTIDX, t );

				offset		+= sp_period->fe->size * idx;
			}

			totaloffset += offset;

			if ( NULL == iter->right_op || TOK_PERIOD != iter->right_op->type )
				break;

			iter = iter->right_op;
		}

		addr += totaloffset;

		if ( FMT_STR == etype )
			sys_str::copy( (char*)addr, 256, v );

		return;
	}

	
	if ( 1 == t->cls.carray && -1 != t->sid )
	{// array
		int idx = get_array_index( vm_items[ t->pos + 1 ], t->right_array );
		VMASSERT2( idx >= 0 && idx < varlist[ t->sid ]->repeat, EC_1001_OUTIDX, t );

		VARITEM* vi = varlist[ t->sid ];
		char* addr = (char*)vi->realaddr;

		if ( VMCLS_S == t->cls.cls )
			sys_str::copy( (char*)(addr + 256*idx), 256, v );
	}
	else
	{
		if ( VMCLS_S == t->cls.cls && 0 == t->cls.cconst )
			write_vm_var_str( t, v );
	}
}


VMINT CCitrus::get_var_sid( char* name )
{
	for (VMINT k=0; k<varlist.num; k++)
	{
		if ( TCMP( varlist[ k ]->name, name ) )
		{
			return k;
		}
	}

	return -1;
}


VMINT CCitrus::trans_integer( char* nstr, VMINT texttype )
{
	if ( 1 == texttype )
	{
		return atoi( nstr );
	}
	else if ( 2 == texttype )
	{
		return (VMINT)atof( nstr );
	}
	else if ( 3 == texttype )
	{
		VMINT v = 0;
		sscanf( nstr, "%x", &v );
		return v;
	}

	VMASSERT( 0, EC_0010_TIERR );
	return 0;
}

// 0: string, 1: integer, 2: float, 3: hexadecimal
VMINT CCitrus::get_text_type( char* str )
{
	VMINT len = sys_str::length( str );

	if ( 0 == len )
		return 0;

	if ( '\"' == str[0] )
		return 0;

	BOOL b_exist_dot = FALSE;

	if ( len > 2 && '0' == str[ 0 ] && ('x' == str[1] || 'X' == str[1] ) )
	{// hexadecimal
		return 3;
	}

	for (VMINT i=0; i<len; i++)
	{
		if ( '.' == str[ i ] )
		{
			b_exist_dot = TRUE;
			continue;
		}

		if ( 0 == isdigit( str[ i ] ) )
			return 0;
	}

	if ( b_exist_dot )
		return 2;

	return 1;
}

void CCitrus::parse_IF_scope( VMINT vm_ip )
{
	// search ENDIF
	VMINT endpos = -1;
	VMINT elsepos = -1;

	int nested_level = 0;

	for (VMINT op_idx = vm_ip+1; op_idx<vm_code.num; op_idx++)
	{
		VMITEM* vmi = vm_code[ op_idx ];

		if ( TOK_IF == vmi->type )
			nested_level++;

		if ( TOK_ELSE == vmi->type && -1 == elsepos && 0 == nested_level )
		{
			if ( -1 == elsepos )
				elsepos = op_idx;
		}
		else if ( TOK_ENDIF == vmi->type && -1 == endpos && 0 == nested_level )	
		{
			if ( -1 == endpos )
				endpos = op_idx;
		}

		if ( TOK_ENDIF == vmi->type )
			nested_level--;

		if ( -1 != endpos )
			break;
	}

	VMASSERT( endpos != -1, EC_0011_PISERR );	// must be found.

	// scope
	SPEC_IF* sp = (SPEC_IF*)vm_code[ vm_ip ]->spec;
	VMASSERT( sp, EC_0011_PISERR );


	sp->scope[ 0 ] = vm_ip + 1;		// jump here when condition is true.
	sp->scope[ 2 ] = endpos + 1;	// jump here after true section.

	// if there is 'else'
	if ( -1 != elsepos )
	{
		sp->scope[ 1 ] = elsepos + 1;		// jump here when condition is false.
		SPEC_ELSE* sp_else = (SPEC_ELSE*)vm_code[ elsepos ]->spec;
		sp_else->next = endpos + 1;
	}
}

void CCitrus::parse_WHILE_scope( VMINT vm_ip )
{
	// search ENDWHILE
	VMINT endpos = -1;

	int nested_level = 0;

	for (VMINT op_idx = vm_ip+1; op_idx<vm_code.num; op_idx++)
	{
		VMITEM* vmi = vm_code[ op_idx ];

		if ( TOK_WHILE == vmi->type )
			nested_level++;

		if ( TOK_ENDWHILE == vmi->type && -1 == endpos && 0 == nested_level )	
		{
			endpos = op_idx;
		}

		if ( TOK_ENDWHILE == vmi->type )	
			nested_level--;

		if ( -1 != endpos )
			break;
	}

	VMASSERT( endpos != -1, EC_0012_PWSERR );	// must be found.

	// scope
	SPEC_IF* sp = (SPEC_IF*)vm_code[ vm_ip ]->spec;
	VMASSERT( sp, EC_0012_PWSERR );


	sp->scope[ 0 ] = vm_ip + 1;		// jump here when condition is true.
	sp->scope[ 1 ] = endpos + 1;	// jump here after true section.

	SPEC_ENDWHILE* sp_endwhile = (SPEC_ENDWHILE*)vm_code[ endpos ]->spec;
	sp_endwhile->next = vm_ip; // pointing "while"
}

void CCitrus::parse_PROC_scope( VMINT vm_ip )
{
	// search brace_close.
	VMINT endpos = 0;
	for ( VMINT k=vm_ip+1; k<vm_code.num; k++)
	{
		if ( TOK_ENDPROC == vm_code[ k ]->type )
		{
			endpos = k;
			break;
		}
	}
	VMASSERT( endpos, EC_0013_PPSERR );	// must be found.

	// scope
	SPEC_PROC* sp = (SPEC_PROC*)vm_code[ vm_ip ]->spec;
	VMASSERT( sp, EC_0013_PPSERR );

	sp->scope[ 0 ]	= vm_ip;	// function itself
	sp->scope[ 1 ]	= endpos;	// }
	sp->endproc		= (SPEC_ENDPROC*)(vm_code[ endpos ]->spec);
}

void CCitrus::push_value( VMITEM* vmi )
{
	ALLVALUE* av = new ALLVALUE;

	if ( VMCLS_I == vmi->cls.cls )
	{
		VMINT v = (VMINT)read_operand( vmi );
		av->set_int( v );
	}
	else if ( VMCLS_F == vmi->cls.cls )
	{
		VMDOUBLE v = read_operand( vmi );
		av->set_double( v );
	}
	else if ( VMCLS_S == vmi->cls.cls )
	{			
		//av->set_string( vmi->initval.string );
		av->set_string( read_operand_str( vmi ) );
	}
	else 
		VMASSERT2( 0, EC_0014_EOPUSH, vmi );

	vm_value_stack.add( av );
}

void CCitrus::pop_value( VMITEM* vmi )
{
	VMASSERT2( vmi, EC_0015_EOPOP, vmi );
	VMASSERT2( vm_value_stack.num, EC_0015_EOPOP, vmi );

	if ( VMCLS_I == vmi->cls.cls || VMCLS_F == vmi->cls.cls )
	{
		ALLVALUE* _av = vm_value_stack.pop();	VMASSERT2( _av, EC_0015_EOPOP, vmi );

		if ( AV_INT == _av->type )
			write_operand( vmi, (VMDOUBLE)_av->int_val );
		else if ( AV_DOUBLE == _av->type )
			write_operand( vmi, _av->dbl_val );
		else
			VMASSERT2( 0, EC_0015_EOPOP, vmi );

		delete _av;
	}
	else if ( TOK_VAR_STR == vmi->type )
	{
		ALLVALUE* _av = vm_value_stack.pop();	VMASSERT2( _av, EC_0015_EOPOP, vmi );

		VMASSERT2( AV_STRING == _av->type, EC_0015_EOPOP, vmi );

		write_operand_str( vmi, _av->string );

		delete _av;
	}
	else
		VMASSERT2( 0, EC_0015_EOPOP, vmi );
}


void CCitrus::peek_value( VMITEM* vmi )
{
	VMASSERT2( vmi, EC_0016_EOPEEK, vmi );
	VMASSERT2( vm_value_stack.num, EC_0016_EOPEEK, vmi );

	if ( VMCLS_I == vmi->cls.cls || VMCLS_F == vmi->cls.cls )
	{
		ALLVALUE* _av = vm_value_stack.peek();	VMASSERT2( _av, EC_0016_EOPEEK, vmi );

		if ( AV_INT == _av->type )
			write_operand( vmi, (VMFLOAT)_av->int_val );
		else if ( AV_DOUBLE == _av->type )
			write_operand( vmi, _av->dbl_val );
		else
			VMASSERT2( 0, EC_0016_EOPEEK, vmi );
	}
	else if ( TOK_VAR_STR == vmi->type )
	{
		ALLVALUE* _av = vm_value_stack.peek();	VMASSERT2( _av, EC_0016_EOPEEK, vmi );

		VMASSERT2( AV_STRING == _av->type, EC_0016_EOPEEK, vmi );

		write_operand_str( vmi, _av->string );
	}
	else
		VMASSERT2( 0, EC_0016_EOPEEK, vmi );
}

void CCitrus::peek_value_from( VMITEM* vmi, int level )
{
	VMASSERT2( vmi, EC_0016_EOPEEK, vmi );
	VMASSERT2( vm_value_stack.num, EC_0022_EOPEEKF, vmi );
	VMASSERT2( (vm_value_stack.num-1) >= level, EC_0022_EOPEEKF, vmi );

	if ( VMCLS_I == vmi->cls.cls || VMCLS_F == vmi->cls.cls )
	{
		ALLVALUE* _av = vm_value_stack[ vm_value_stack.num-1-level];	VMASSERT2( _av, EC_0022_EOPEEKF, vmi );

		if ( AV_INT == _av->type )
			write_operand( vmi, (VMFLOAT)_av->int_val );
		else if ( AV_DOUBLE == _av->type )
			write_operand( vmi, _av->dbl_val );
		else
			VMASSERT2( 0, EC_0022_EOPEEKF, vmi );
	}
	else if ( TOK_VAR_STR == vmi->type )
	{
		ALLVALUE* _av = vm_value_stack[ vm_value_stack.num-1-level];	VMASSERT2( _av, EC_0022_EOPEEKF, vmi );

		VMASSERT2( AV_STRING == _av->type, EC_0022_EOPEEKF, vmi );

		write_operand_str( vmi, _av->string );
	}
	else
		VMASSERT2( 0, EC_0022_EOPEEKF, vmi );
}

void CCitrus::init_token_options( Tokenizer& tok )
{
	tok.ClearGroupMakerKey();
	tok.AddGroupMakerKey( '"', '"' );

	tok.ClearDelimiterKey();
	tok.AddDelimiterKey( '{' );
	tok.AddDelimiterKey( '}' );
	tok.AddDelimiterKey( ';' );
	tok.AddDelimiterKey( '(' );
	tok.AddDelimiterKey( ')' );
	tok.AddDelimiterKey( '!' );
	tok.AddDelimiterKey( ',' );
	tok.AddDelimiterKey( '/' );
	tok.AddDelimiterKey( '=' );
	tok.AddDelimiterKey( '+' );
	tok.AddDelimiterKey( '-' );
	tok.AddDelimiterKey( '*' );
	tok.AddDelimiterKey( '%' );
	tok.AddDelimiterKey( '<' );
	tok.AddDelimiterKey( '>' );
	tok.AddDelimiterKey( '@' );
	tok.AddDelimiterKey( '\"' );
	tok.AddDelimiterKey( '[' );
	tok.AddDelimiterKey( ']' );
	tok.AddDelimiterKey( '.' );
	tok.AddDelimiterKey( '&' );
	tok.AddDelimiterKey( '$' );
}


void CCitrus::vm_make_tokens( char* pathname, sys_list<DEFITEM*>* deflist_parent )
{
	Tokenizer tok;

	init_token_options( tok );

	BOOL	b_comment			= FALSE;
	VMINT	debug_line_counter	= 1;
	BOOL	b_in_verbatim_mode	= FALSE;
	BOOL	b_stop_parsing		= FALSE;


	sys_list<DEFITEM*>		deflist_vault;
	sys_list<DEFITEM*>*		deflist = &deflist_vault;

	if ( deflist_parent )
		deflist = deflist_parent;


	void (_stdcall *vbt_handler)(const char* text);
	vbt_handler = NULL;


	FILE* fp = fopen( pathname, "rt" );
	VMASSERT1( fp, EC_1005_FNF": %s", pathname );

	char drv[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char parentpath[_MAX_PATH];
	_splitpath_s( pathname, drv, _MAX_DRIVE, dir, _MAX_DIR, NULL, 0, NULL, 0 );
	sprintf( parentpath, "%s%s", drv, dir );


	while ( fp && !feof(fp) && FALSE == b_stop_parsing )
	{
		VMINT	len				= 0;
		char*	token			= NULL;
		BOOL	b_record_vmt	= TRUE;

		char	aline[256];
		if ( NULL == fgets( aline, 256, fp ) )
			break;

		tok.SetLine( aline, 256 );

		while ( (token = tok.GetNext( &len )) && FALSE == b_stop_parsing )
		{
			char* next = tok.PeekPtr();

			if ( FALSE == b_comment && TCMP( token, "/" ) && '*' == *next )
			{
				// comment start; does not process.
				b_comment = TRUE;
				tok.GetNext( &len );
			}
			else if ( TCMP( token, "*" ) && '/' == *next )
			{
				// comment end
				b_comment = FALSE;
				tok.GetNext( &len );
				continue;
			}

			if ( b_comment )
				continue;

			if ( TCMP( token, "/" ) && '/' == *next )
			{
				// comment line; does not process.
				break;
			}

			VMTOKEN* vmt = new VMTOKEN;
			vmt->debug_line	= debug_line_counter;

			if ( TCMP( token, "/" ) && '=' == *next )
			{
				sys_str::copy( vmt->tok, 256, "/=" );
				tok.GetNext( &len );
			}
			else if ( TCMP( token, "+" ) && '=' == *next )
			{
				sys_str::copy( vmt->tok, 256, "+=" );
				tok.GetNext( &len );
			}
			else if ( TCMP( token, "+" ) && '+' == *next )
			{
				sys_str::copy( vmt->tok, 256, "++" );
				tok.GetNext( &len );
			}
			else if ( TCMP( token, "-" ) && '=' == *next )
			{
				sys_str::copy( vmt->tok, 256, "-=" );
				tok.GetNext( &len );
			}
			else if ( TCMP( token, "-" ) && '-' == *next )
			{
				sys_str::copy( vmt->tok, 256, "--" );
				tok.GetNext( &len );
			}
			else if ( TCMP( token, "*" ) && '=' == *next )
			{
				sys_str::copy( vmt->tok, 256, "*=" );
				tok.GetNext( &len );
			}
			else if ( TCMP( token, "%" ) && '=' == *next )
			{
				sys_str::copy( vmt->tok, 256, "%=" );
				tok.GetNext( &len );
			}
			else if ( TCMP( token, "<" ) && '=' == *next )
			{
				sys_str::copy( vmt->tok, 256, "<=" );
				tok.GetNext( &len );
			}
			else if ( TCMP( token, ">" ) && '=' == *next )
			{
				sys_str::copy( vmt->tok, 256, ">=" );
				tok.GetNext( &len );
			}
			else if ( TCMP( token, "=" ) && '=' == *next )
			{
				sys_str::copy( vmt->tok, 256, "==" );
				tok.GetNext( &len );
			}
			else if ( TCMP( token, "!" ) && '=' == *next )
			{
				sys_str::copy( vmt->tok, 256, "!=" );
				tok.GetNext( &len );
			}
			else if ( TCMP ( token, "." ) )
			{
				VMTOKEN* test = vm_tok_list.peek();

				if ( get_text_type( test->tok ) == 1 ) // 1 == integer.  We checks only a part before the period, so integer.
				{// period for number
					sys_str::copy( vmt->tok, 256, "." );
					VMTOKEN* last_vmt = vm_tok_list.pop();

					char* remains = tok.PeekNext( &len );
					
					if ( ',' == *next || ' ' == *next || NULL == remains || sys_str::icompare( remains, ";" ) )
						sprintf( vmt->tok, "%s.0", last_vmt->tok );
					else
						sprintf( vmt->tok, "%s.%s", last_vmt->tok, remains );

					delete last_vmt;

					if ( ',' != *next && ' ' != *next && FALSE == sys_str::icompare( remains, ";" ) )
					{// digest the next message.
						tok.GetNext( &len );
					}
				}
				else
				{// period for FORMAT
					sys_str::copy( vmt->tok, 256, "." );
				}
			}
			else
			{
				if ( TCMP( token, "#STOP" ) )
				{
					b_stop_parsing = TRUE;
					continue;
				}
				else if ( TCMP( token, "#ENTRY" ) )
				{
					char* entryname = tok.GetNext( &len );
					if ( entryname )
						sys_str::copy( vm_entry_name, 256, entryname );
					continue;
				}
				else if ( TCMP( token, "#DEFINE" ) )
				{// add to define list.
					DEFITEM*	t = new DEFITEM;

					char* varname = tok.GetNext( &len );
					sys_str::copy( t->name, 256, varname );

					char* subtext = tok.GetNext( &len );
					VMASSERT( subtext, EC_1006_DENT );
						
					sys_str::copy( t->substitution, 256, subtext );

					deflist->add( t );

					continue;
				}
				else if ( TCMP( token, "#INCLUDE" ) )
				{// include text.
					char* fname = tok.GetNext( &len );
					VMASSERT( fname, EC_1007_IEFNF );

					fname[ sys_str::length( fname ) - 1 ] = NULL;
					char pathname[256];
					sys_str::copy( pathname, 256, fname+1 );

					char pathonly[256];
					sys_str::getpathonly( pathname, pathonly );
					if ( NULL == pathonly[0] )
					{
						char incpath[256];
						strcpy( incpath, parentpath );
						strcat( incpath, pathname );
						vm_make_tokens( incpath, deflist );
					}
					else
					{
						char incpath[256];
						strcpy( incpath, parentpath );
						strcat( incpath, pathname );
						vm_make_tokens( incpath, deflist );
					}

					continue;
				}
				else
				{// lookup deflist.
					BOOL b_found_def = FALSE;

					for (VMINT k=0; k<deflist->num; k++)
					{
						if ( TCMP( (*deflist)[ k ]->name, token ) )
						{
							sys_str::copy( vmt->tok, 256, (*deflist)[ k ]->substitution );
							b_found_def = TRUE;
							break;
						}
					}

					// otherwise:
					if ( FALSE == b_found_def )
						sys_str::copy( vmt->tok, 256, token );
				}


				// verbatim handler check--------------------------------------------
				if ( TCMP( vmt->tok, "<" ) )
				{
					BOOL b_is_markup_sign = FALSE;

					char* peeknext = tok.PeekNext( &len );
					char* peekptr  = tok.PeekPtr();

					for (;*peekptr; peekptr++)
					{
						if ( '>' == *peekptr )
						{
							b_is_markup_sign = TRUE;
							break;
						}
					}
				
					if ( b_is_markup_sign )
					{
						if ( '/' == peeknext[ 0 ] )
							b_in_verbatim_mode	= FALSE;
						else
							b_in_verbatim_mode	= TRUE;


						BOOL b_found = FALSE;

						if ( vbt_handler )
						{
							// search handlers.
							for (VMINT k=0; k<vm_vhandler.num; k++)
							{
								if ( TCMP( peeknext, vm_vhandler[ k ]->close_keyword ) )
								{
									// verbatim mode finished.
									tok.LoadDefaultKey();

									init_token_options( tok );

									tok.GetNext( &len );
									tok.GetNext( &len );
									tok.GetNext( &len );

									vbt_handler		= NULL;
									b_record_vmt	= FALSE;
									b_found			= TRUE;

									break;
								}
							}
						}
						else
						{
							for (VMINT k=0; k<vm_vhandler.num; k++)
							{		
								if ( TCMP( peeknext, vm_vhandler[ k ]->open_keyword ) )
								{
									tok.ClearGroupMakerKey();
									tok.ClearSeparatorKey();
									tok.ClearDelimiterKey();

									tok.AddDelimiterKey( '<' );
									tok.AddDelimiterKey( '>' );
									tok.AddSeparatorKey( '\n' );
									tok.AddSeparatorKey( '\r' );

									tok.GetNext( &len );
									tok.GetNext( &len );

									vbt_handler		= vm_vhandler[ k ]->handler;
									b_record_vmt	= FALSE;
									b_found			= TRUE;

									break;
								}
							}

							if ( FALSE == b_found )
							{
								// handler not found !
								// digest useless remains.
								tok.GetNext( &len );
								tok.GetNext( &len );
								tok.GetNext( &len );

								b_record_vmt = FALSE;
							}
						}
					}
				}
				else if ( vbt_handler )
				{
					vbt_handler( vmt->tok );
				}
			}

			if ( FALSE == b_in_verbatim_mode && b_record_vmt )
				vm_tok_list.add( vmt );
		}

		debug_line_counter++;
	}

	fclose( fp );

	// remove: deflist
	if ( FALSE == deflist_parent )
	{
		while ( ! deflist->isempty() )
			delete deflist->pop();
	}
}

int CCitrus::vm_alignment( int size, int repeat )
{
	if ( size <= vm_datapack )
		return size * repeat;

	int inc = size * repeat;
	int tilt = (inc % vm_datapack);
	if ( tilt > 0 )
	{
		inc -= tilt;
		inc += vm_datapack;
	}
	return inc;
}

// calc format element's base offset
int CCitrus::calc_format_element_offsets( FORMATITEM* fi, VMINT debug_line, int* totaloffset )
{
	int offset = 0;

	for (VMINT k=0; k<fi->list.num; k++)
	{
		FORMAT_ELE* fe = fi->list[ k ];

		fe->offset	= offset;

		if ( FMT_INT == fe->ftype )
		{
			fe->size	= sizeof( VMINT );
			offset += vm_alignment( sizeof(VMINT), fe->repeat );
		}
		else if ( FMT_INT64 == fe->ftype )
		{
			fe->size	= sizeof( VMINT64 );
			offset += vm_alignment( sizeof(VMINT64), fe->repeat );
		}
		else if ( FMT_BYTE == fe->ftype )
		{
			fe->size	= sizeof( VMBYTE );
			offset += vm_alignment( sizeof(VMBYTE), fe->repeat );
		}
		else if ( FMT_SHORT == fe->ftype )
		{
			fe->size	= sizeof( VMSHORT );
			offset += vm_alignment( sizeof(VMSHORT), fe->repeat );
		}
		else if ( FMT_WORD == fe->ftype )
		{
			fe->size	= sizeof( VMWORD );
			offset += vm_alignment( sizeof(VMWORD), fe->repeat );
		}
		else if ( FMT_DWORD == fe->ftype )
		{
			fe->size	= sizeof( VMDWORD );
			offset += vm_alignment( sizeof(VMDWORD), fe->repeat );
		}
		else if ( FMT_FLOAT == fe->ftype )
		{
			fe->size	= sizeof( VMFLOAT );
			offset += vm_alignment( sizeof(VMFLOAT), fe->repeat );
		}
		else if ( FMT_DOUBLE == fe->ftype )
		{
			fe->size	= sizeof( VMDOUBLE );
			offset += vm_alignment( sizeof(VMDOUBLE), fe->repeat );
		}
		else if ( FMT_STR == fe->ftype )
		{
			fe->size	= 256;
			offset += vm_alignment( 256, fe->repeat );
		}
		else if ( FMT_FORMAT == fe->ftype )
		{
			for (int q=0; q<fe->repeat; q++)
			{
				fe->size = calc_format_element_offsets( fe->link, debug_line, NULL );
				offset += fe->size;
			}
		}
	}

	if ( totaloffset )
		*totaloffset += offset;

	return offset;
}

FORMATITEM*	CCitrus::search_format_item( char* testname )
{
	for (VMINT k=0; k<vm_format_list.num; k++)
	{
		FORMATITEM*	fi = vm_format_list[ k ];

		if ( TCMP( fi->name, testname ) )
			return fi;
	}

	return NULL;
}

VMITEM* CCitrus::process_unknown_token( char* parenttok, VMINT& ti, char* token, VMINT* type_out, VMINT debug_line )
{
	VMINT sid = get_var_sid( token );
	VMINT type = get_text_type( token );

	// format instanciate---------------------------------------------------------
	if ( 1 != type && 2 != type )
	{
		if ( -1 == sid )
		{// new appearance. make item and its instance.
			FORMATITEM* fi = search_format_item( token ); // token = FORMAT name (not instance name)

			if ( fi )
			{
				ti++;
				char* instname = vm_tok_list[ ti ]->tok; // operand A: instance name
				VMITEM* _t = make_item( TOK_VAR_FORMAT, instname, -1, I_NOP, debug_line ); // make an instance information.

				int totaloffset = 0;
				calc_format_element_offsets( fi, debug_line, &totaloffset );

//				printf("DEBUG: sizeof(%s) = %d\n", instname, totaloffset);

				int n_repeat = 1;

				ti++;
				char* chk = vm_tok_list[ ti ]->tok; // repeat number
				if ( TCMP( chk, "[" ) )
				{// array
					ti++;
					char* tok = vm_tok_list[ ti ]->tok;

					VMINT type = get_text_type( tok );

					if ( 1 == type || 2 == type || 3 == type )
					{
						_t->cls.carray	= 1; // property change must be placed before make_vm_var_xx
						n_repeat = trans_integer( tok, type );
						_t->sid = make_vm_var_format( _t, fi, totaloffset, n_repeat );
					}

					ti++;
					tok = vm_tok_list[ ti ]->tok;
					VMASSERT2( TCMP( tok, "]" ), EC_1008_FMTE, _t );
				}
				else
					_t->sid = make_vm_var_format( _t, fi, totaloffset, 1 );	// link INSTANCE name to FORMAT name.

				fi->size	= totaloffset;

				if ( type_out )
					*type_out = _t->type;

				return _t;
			}
		}
		else if ( TOK_VAR_FORMAT == varlist[ sid ]->tok_type )
		{ 
			// make item only; don't instanciate. duplication.
			VMITEM* _t = make_item( TOK_VAR_FORMAT, varlist[ sid ]->name, sid, I_NOP, debug_line );

			// copy properties.
			_t->cls = varlist[ sid ]->cls;

			// Array mark***
			// above "copy properties" clears _t->cls's carrayidx.
			// so check the bit again.
			if ( b_in_array_brace_at_unknown )
				_t->cls.carrayidx = 1;

			if ( type_out )
				*type_out = _t->type;

			return _t;
		}
	}


	// constant----------------------------------------------------------------
	if ( 1 == type || 3 == type )
	{
		VMITEM* _t = make_item( TOK_CONST_INT, "CONST_INT", -1, I_NOP, debug_line );
		_t->initval.set_int( trans_integer( token, type ) );

		// array mark
//		if ( b_in_array_brace_at_unknown )
//			_t->cls.carrayidx = 1;

		if ( type_out )
			*type_out = _t->type;

		return _t;
	}	
	else if ( 2 == type )
	{
		VMITEM* _t = make_item( TOK_CONST_DOUBLE, "CONST_DOUBLE", -1, I_NOP, debug_line );
		_t->initval.set_double( atof( token ) );

		// NOTE: array mark: double cannot be used as a array index.

		if ( type_out )
			*type_out = _t->type;

		return _t;
	}
	else
	{
		if ( -1 != sid )
		{ // keyword is a variable which already has been defined. Duplicate the item.
			TOKTYPE tok_type = varlist[ sid ]->tok_type;
			VMITEM* _t = make_item( tok_type, token, sid, I_NOP, debug_line );

			// copy properties.
			_t->cls = varlist[ sid ]->cls;

			// Array mark***
			// above "copy properties" clears _t->cls's carrayidx.
			// so check the bit again.
			if ( b_in_array_brace_at_unknown )
				_t->cls.carrayidx = 1;

			if ( type_out )
				*type_out = tok_type;

			return _t;
		}
		else
		{// is the "constant-string"  or  not-supported keyword.
			VMITEM* _t = make_item( TOK_CONST_STR, "CONST_STR", -1, I_NOP, debug_line );
			
			char* last_char = token + sys_str::length( token ) - 1;

			// NOTE: array mark: CONST_STR cannot be used as a array index.

			// check if the token is a constant-string.
			if ( '\"' != token[0] || '\"' != *last_char )
			{
				char aline[1024] = {0,};
				for(int k=0; k<vm_tok_list.num; k++)
				{
					if(debug_line == vm_tok_list[k]->debug_line)
					{
						strcat_s(aline, vm_tok_list[k]->tok);
						strcat_s(aline, " ");
					}
				}
					
				char emsg[256];
//				sprintf( emsg, EC_1009_NSKUV": \"%s %s\"", parenttok, token );
				sprintf(emsg, EC_1009_NSKUV": \"%s\"", aline);
				VMASSERT2( 0, emsg, _t ); 
				// we dont need to remove _t;  if an error occurs, here will never reach.
			}

			*last_char = NULL;

			// new-line filtering:
			char filtered[256];
			int idx		= 0;
			int idx_w	= 0;

			// removal of double-quotes
			while ( 1 )
			{
				char ch		= token[1+idx];
				char chn	= token[1+idx+1];

				if ( '\\' == ch && 'n' == chn )
				{
					filtered[ idx_w++ ] = '\n';
					idx += 2;
				}
				else
				{
					filtered[ idx_w++ ] = ch;
					idx++;
				}

				if ( NULL == ch )
					break;
			}

			_t->initval.set_string( filtered );

			if ( type_out )
				*type_out = 0;

			return _t;
		}
	}

	return NULL;
}


/***********************************************************************************************/
/*                                          PASS 0                                             */
/***********************************************************************************************/

VMITEM* CCitrus::find_left_operand( int idx, sys_list<STATEMENTITEM*>& statement, BOOL b_array_open )
{
	for (int k=idx-1; k>=0; k--)
	{
		int vmidx = statement[k]->idx;

		VMITEM* test_vi = vm_items[ vmidx ];

		if ( TOK_ARRAY_OPEN == test_vi->type )
			b_array_open = FALSE;

		if ( 0 == test_vi->cls.clinkable || 1 == test_vi->cls.ckeyword )
			continue;

		if ( b_array_open )
		{ 
			if ( 1 == test_vi->cls.carrayidx )
				return test_vi;
		}
		else
		{
			if ( 0 == test_vi->cls.carrayidx )
				return test_vi;
		}
	}

	return NULL;
}

VMITEM* CCitrus::find_left_operator( int idx, sys_list<STATEMENTITEM*>& statement, BOOL b_array_open )
{
	for (int k=idx-1; k>=0; k--)
	{
		int vmidx = statement[k]->idx;

		VMITEM* test_vi = vm_items[ vmidx ];

		if ( TOK_ARRAY_OPEN == test_vi->type )
			b_array_open = FALSE;

		if ( 0 == test_vi->cls.clinkable || 0 == test_vi->cls.ckeyword )
			continue;

		if ( b_array_open )
		{ 
			if ( 1 == test_vi->cls.carrayidx )
				return test_vi;
		}
		else
		{
			if ( 0 == test_vi->cls.carrayidx )
				return test_vi;
		}
	}

	return NULL;
}

VMITEM* CCitrus::find_right_operand( int idx, sys_list<STATEMENTITEM*>& statement, BOOL b_array_open )
{
//	if ( TOK_ARRAY_OPEN == vm_items[ statement[idx]->idx + 1 ]->type )
//		b_array_open = TRUE;

	for (int k=idx+1; k<statement.num; k++)
	{
		int vmidx = statement[k]->idx;

		VMITEM* test_vi = vm_items[ vmidx ];

		if ( TOK_ARRAY_CLOSE == test_vi->type )
			b_array_open = FALSE;

		if ( 0 == test_vi->cls.clinkable || 1 == test_vi->cls.ckeyword )
			continue;

		if ( b_array_open )
		{ 
			if ( 1 == test_vi->cls.carrayidx )
				return test_vi;
		}
		else
		{
			if ( 0 == test_vi->cls.carrayidx )
				return test_vi;
		}
	}

	return NULL;
}

VMITEM* CCitrus::find_right_operator( int idx, sys_list<STATEMENTITEM*>& statement, BOOL b_array_open )
{
//	if ( TOK_ARRAY_OPEN == vm_items[ statement[idx]->idx + 1 ]->type )
//		b_array_open = TRUE;

	for (int k=idx+1; k<statement.num; k++)
	{
		int vmidx = statement[k]->idx;

		VMITEM* test_vi = vm_items[ vmidx ];

		if ( TOK_ARRAY_CLOSE == test_vi->type )
			b_array_open = FALSE;

		if ( 0 == test_vi->cls.clinkable || 0 == test_vi->cls.ckeyword )
			continue;

		if ( b_array_open )
		{ 
			if ( 1 == test_vi->cls.carrayidx )
				return test_vi;
		}
		else
		{
			if ( 0 == test_vi->cls.carrayidx )
				return test_vi;
		}
	}

	return NULL;
}

VMITEM* CCitrus::find_right_array( int idx, sys_list<STATEMENTITEM*>& statement )
{
	for (int k=idx+1; k<statement.num; k++)
	{
		int vmidx = statement[k]->idx;

		VMITEM* test_vi = vm_items[ vmidx ];

		if ( 0 == test_vi->cls.clinkable || 1 == test_vi->cls.ckeyword )
			continue;

		if ( 1 == test_vi->cls.carrayidx )
			return test_vi;
	}

	return NULL;
}


void CCitrus::sort_statement_by_priority( sys_list<STATEMENTITEM*>& statement, sys_list<VMITEM*>& output, BOOL b_filterout_arrayidx )
{
	//OutputDebugStringA("\nUnsorted statements:\n");
	//for (VMINT k=0; k<statement.num; k++)
	//{
	//	VMITEM* tk = vm_items[ statement[ k ]->idx ];
	//	OutputDebugStringA(tk->name);
	//}
	//OutputDebugStringA("\n\n");

	while ( 1 )
	{
		VMINT op_pri = -1;
		VMINT op_idx = 0;

		VMINT foundidx = -1;

		// right-to-left scanning.
		for (VMINT k=statement.num-1; k>=0; k--)
		{
			VMITEM* tk = vm_items[ statement[ k ]->idx ];

			if ( FALSE == tk->b_op || TRUE == statement[ k ]->processed )
				continue;

			if ( b_filterout_arrayidx && 1 == tk->cls.carrayidx )
				continue;

			// When two instructions have the same priority,
			//    left-side placed one is the first.
			// Scans from right to left. If there's the same prio. overwrite the last record.
			if ( tk->pri > op_pri )
			{
				op_idx		= statement[ k ]->idx;
				op_pri		= tk->pri;
				foundidx	= k;
			}
		}

		if(-1 == foundidx)
			break;

		statement[ foundidx ]->processed = TRUE;

		output.add( vm_items[ op_idx ] );
	}


	//OutputDebugStringA("\nSorted statements:\n");
	//for (VMINT k=0; k<output.num; k++)
	//{
	//	VMITEM* tk = output[k];
	//	OutputDebugStringA(tk->name);
	//}
	//OutputDebugStringA("\n\n");
}


void CCitrus::adjust_operand_links( sys_list<VMITEM*>& sorteditems )
{
	for (int k=0; k<sorteditems.num; k++)
	{
		VMITEM* vi = sorteditems[ k ];

		// unlink already used operands before.
		switch ( vi->type )
		{
		case TOK_OPR_ADD:		
		case TOK_OPR_SUBTRACT:	
		case TOK_OPR_MULTIPLY:	
		case TOK_OPR_DIVIDE:	
		case TOK_OPR_MOD:
		case TOK_OPR_ADD_EQUAL:		
		case TOK_OPR_SUBTRACT_EQUAL:	
		case TOK_OPR_MULTIPLY_EQUAL:	
		case TOK_OPR_DIVIDE_EQUAL:	
		case TOK_OPR_MOD_EQUAL:
			{
				if ( vi->left && FALSE == vi->left->referred )
					vi->left->referred = TRUE;
				else
					vi->left = NULL;

				if ( vi->right && FALSE == vi->right->referred )
					vi->right->referred = TRUE;
				else
					vi->right = NULL;
			}
			break;
		case TOK_MATH_SQRT:			
		case TOK_MATH_EXP:			
		case TOK_MATH_SIN:			
		case TOK_MATH_COS:
		case TOK_CALL:
		case TOK_CALLPROC:
			{
				if ( vi->right && FALSE == vi->right->referred )
					vi->right->referred = TRUE;
				else
					vi->right = NULL;
			}
			break;

		case TOK_OPR_ADDADD:
		case TOK_OPR_SUBSUB:
			{
				if ( vi->left && FALSE == vi->left->referred )
					vi->left->referred = TRUE;
			}
			break;
		}

		// unlink math siblings:
		if ( vi->left )
		{
			switch ( vi->left->type )
			{
			case TOK_MATH_SQRT:			
			case TOK_MATH_EXP:			
			case TOK_MATH_SIN:			
			case TOK_MATH_COS:
				vi->left = NULL;
				break;
			}
		}

		if ( vi->right )
		{
			switch ( vi->right->type )
			{
			case TOK_MATH_SQRT:			
			case TOK_MATH_EXP:			
			case TOK_MATH_SIN:			
			case TOK_MATH_COS:
				vi->right = NULL;
				break;
			}
		}
	}
}


// 1. setting scope, priority
void CCitrus::vm_pass_1()
{
	sys_list<STATEMENTITEM*>	statement;
	sys_list<STATEMENTITEM*>	array_idx_statement;

	// priority setting and arrange runtime-------------------------------------------------
	for (VMINT vm_ip = 0; vm_ip<vm_items.num; vm_ip++)
	{
		VMITEM* vmi = vm_items[ vm_ip ];

		// compose a statement.
		if ( TOK_STATEMENT != vmi->type )
		{
			STATEMENTITEM* si = new STATEMENTITEM;
			si->idx			= vm_ip;
			si->processed	= FALSE;
			statement.add( si );
			continue;
		}

		if ( 0 == statement.num )
			continue;

		BOOL b_array_open = FALSE;
		sys_list<VMITEM*>	array_list;
		VMITEM*				last_array_open = NULL;

		// calc operator's property level.
		VMINT	pri_bias = 0;
		for (VMINT k=0; k<statement.num; k++)
		{
			VMITEM* vmi = vm_items[ statement[ k ]->idx ];

			switch( vmi->type )
			{
			case TOK_OPR_ADD:				vmi->pri = 10 + pri_bias;	break;
			case TOK_OPR_SUBTRACT:			vmi->pri = 10 + pri_bias;	break;
			case TOK_OPR_MOD:				vmi->pri = 10 + pri_bias;	break;
			case TOK_OPR_MULTIPLY:			vmi->pri = 11 + pri_bias;	break;
			case TOK_OPR_DIVIDE:			vmi->pri = 11 + pri_bias;	break;
			case TOK_MATH_SQRT:				vmi->pri = 12 + pri_bias;	break;
			case TOK_MATH_EXP:				vmi->pri = 12 + pri_bias;	break;
			case TOK_MATH_SIN:				vmi->pri = 12 + pri_bias;	break;
			case TOK_MATH_COS:				vmi->pri = 12 + pri_bias;	break;
			case TOK_OPR_SUBTRACT_UNARY:	vmi->pri = 15 + pri_bias;	break;
			case TOK_PRI_INC:				pri_bias += 1000;			break;
			case TOK_PRI_DEC:				pri_bias -= 1000;			break;
			case TOK_ARRAY_OPEN:			b_array_open	= TRUE;		
											array_list.add( vmi );		
											last_array_open	= vmi;		break;
			case TOK_ARRAY_CLOSE:			b_array_open	= FALSE;		
											last_array_open	= NULL;		break;
			}


			if ( 1 == vmi->cls.carrayidx && last_array_open )
			{
				SPEC_ARRAY_OPEN* sp = (SPEC_ARRAY_OPEN*)last_array_open->spec;
				sp->expr.add( vmi );
			}

			if ( 0 == vmi->cls.clinkable )
				continue;

			switch ( vmi->type )
			{
				case TOK_OPR_SUBTRACT_UNARY:
					vmi->right = find_right_operand( k, statement, b_array_open );
					vmi->right_op = find_right_operator( k, statement, b_array_open );
					break;
				case TOK_OPR_ADDADD:
				case TOK_OPR_SUBSUB:
					vmi->left = find_left_operand( k, statement, b_array_open );
					vmi->left_op = find_left_operator( k, statement, b_array_open );
					break;
				default:
					vmi->left = find_left_operand( k, statement, b_array_open );
					vmi->right = find_right_operand( k, statement, b_array_open );
					vmi->left_op = find_left_operator( k, statement, b_array_open );
					vmi->right_op = find_right_operator( k, statement, b_array_open );
					break;
			}

			vmi->right_array = find_right_array( k, statement );
		}


		// compose runtime--------------------------------
		sys_list<VMITEM*> sorteditems;
		sort_statement_by_priority( statement, sorteditems, TRUE );

		if ( sorteditems.num > 0 )
		{
			adjust_operand_links( sorteditems );

			while ( ! sorteditems.isempty() )
			{
				// add the selected op. by its priority.
				VMITEM* vmi = sorteditems.qpop();
				// position information for runtime code.
				vmi->cpos = vm_code.num;
				vm_code.add( vmi );
			}
		}


		// remove the statement.
		while ( ! statement.isempty() )
			delete statement.pop();


		for (int q=0; q<array_list.num; q++)
		{
			VMASSERT( TOK_ARRAY_OPEN == array_list[ q ]->type, EC_0007_NONAOI );

			SPEC_ARRAY_OPEN* sp_ao = (SPEC_ARRAY_OPEN*)array_list[ q ]->spec;

			for (int k=0; k<sp_ao->expr.num; k++)
			{
				STATEMENTITEM* si = new STATEMENTITEM;
				si->idx			= sp_ao->expr[ k ]->pos;
				si->processed	= FALSE;
				array_idx_statement.add( si );
			}

			// clear expr
			while ( ! sp_ao->expr.isempty() )
				sp_ao->expr.pop();

			sys_list<VMITEM*> temp;
			sort_statement_by_priority( array_idx_statement, temp, FALSE );
			adjust_operand_links( temp );

			while ( ! temp.isempty() )
				sp_ao->expr.add( temp.qpop() );

			// remove the statement.
			while ( ! array_idx_statement.isempty() )
				delete array_idx_statement.pop();
		}
	}


//  SCOPES --------------------------------------------------------------------------

	// labels
	for (VMINT vm_ip = 0; vm_ip<vm_code.num; vm_ip++)
	{
		VMITEM* vmi = vm_code[ vm_ip ];

		if ( TOK_LABEL == vmi->type )
		{
			SPEC_LABEL* sp = (SPEC_LABEL*)vmi->spec;
			sp->pos = vm_ip + 1;	// next statement position.

			vmi->b_op = FALSE;
			continue;
		}

		if ( TOK_IF == vmi->type )
		{
			parse_IF_scope( vm_ip );
			continue;
		}

		if ( TOK_WHILE == vmi->type )
		{
			parse_WHILE_scope( vm_ip );
			continue;
		}

		if ( TOK_PROC == vmi->type )
		{
			parse_PROC_scope( vm_ip );
			vmi->b_op = FALSE;
			continue;
		}
	}


// jumps-------------------------------------------------------------
	for (VMINT vm_ip = 0; vm_ip<vm_code.num; vm_ip++)
	{
		VMITEM* vmi = vm_code[ vm_ip ];

		if ( NULL == RTFTBL[vmi->rtf] )
			continue;

		if ( TOK_CALL == vmi->type )
		{
			SPEC_CALL* sp_call = (SPEC_CALL*)vmi->spec;

			// function link
			for (VMINT k=0; k<proclist.num; k++)
			{
				SPEC_PROC* sp_proc = (SPEC_PROC*)proclist[ k ];
				if ( TCMP( sp_proc->name, sp_call->name ) )
				{
					sp_call->pos	= sp_proc->scope[ 0 ]; // function
					sp_call->proc	= sp_proc;
					sp_proc->endproc->call	= sp_call;	// uplink
					sp_proc->endproc->proc	= sp_proc;
					break;
				}
			}

			if ( -1 == sp_call->pos )
			{
				VMASSERT2( sp_call->proc, EC_1040_NREP, vmi );
			}
			else
			{
				VMASSERT2( sp_call->proc, EC_1038_FNF, vmi );
			}

			continue;
		}


		if ( TOK_GOTO == vmi->type )
		{
			SPEC_GOTO* sp_goto = (SPEC_GOTO*)vmi->spec;

			for (VMINT k=0; k<labellist.num; k++)
			{
				SPEC_LABEL* sp_label = (SPEC_LABEL*)labellist[ k ];
				if ( sys_str::icompare( sp_label->name, sp_goto->name ) )
				{
					sp_goto->pos = sp_label->pos;
					break;
				}
			}
			continue;
		}
	}


	// remove labels.
	while( !labellist.isempty() )
		delete labellist.pop();

	// remove token list.
	while( !vm_tok_list.isempty() )
		delete vm_tok_list.pop();
}


VMDOUBLE CCitrus::vm_evaluate_expression( sys_list<VMITEM*>& rtcodes )
{
	for (int i=0; i<rtcodes.num; i++)
	{
		VMITEM* vmi = rtcodes[ i ];
		if ( RTFTBL[vmi->rtf] )
		{
//			RUNTIMEFUNC func = RTFTBL[vmi->rtf];
			VMINT r = (this->*RTFTBL[vmi->rtf])( vmi );
		}
	}

	// empty the stack.
	while ( ! vm_calc_stack.isempty() )
		vm_calc_stack.pop();

	return vm_r0;
}


void exp_encdec( char* src, int slen, char* out )
{
	for (int k=0; k<slen; k++)
	{
		out[ k ] = src[ k ];
		out[ k ] ^= 'H';
		out[ k ] ^= 'B';
	}
}

void CCitrus::export_bin( char* pathname )
{
	DataStream ds;

	char pathname_bin[256];

	sys_str::copy( pathname_bin, 256, pathname );
	sys_str::cat( pathname_bin, 256, ".bin" );

	ds.Init( pathname_bin );
	
	if ( FALSE == ds.CreateToWrite() )
		return;

	sys_str::getpathonly( pathname, vm_script_path );

	vm_make_tokens( pathname, NULL );		// tokenization

	// puts a header
	DI32 sig = 'HBS1';
	ds.PutData<DI32>( &sig, 1 );

	// HBS file structure
	//-------------------------------------------------------------
	//  case of id == 0
	//  --> id(1)|line(2)
	//  case of id > 0
	//  --> id(1)|line(2)|len(2)|data...

//	FILE* fp = fopen( "c:/testout.txt", "wt" );

	for (int k=0; k<vm_tok_list.num; k++)
	{
		VMTOKEN* tok = vm_tok_list[ k ];
//		fprintf( fp, "Tok(%d, line=%d) = %s(%d, len=%d)\n", 
//						k, tok->debug_line, tok->tok, 
//						get_keywordid( tok->tok ),
//						sys_str::length( tok->tok ) );

		unsigned char kid = get_keywordid( tok->tok );
		unsigned short line = (unsigned short)tok->debug_line;

		if ( kid > 0 )
		{// a reserved keyword: puts an id only.
			ds.PutData<DI8>( &kid, 1 );
			ds.PutData<DI16>( &line, 1 );
		}
		else if ( 0 == kid )
		{// not-a-keyword
			ds.PutData<DI8>( &kid, 1 );
			ds.PutData<DI16>( &line, 1 );

			unsigned short len = (unsigned short)sys_str::length( tok->tok );
			len++; // including a null.

			ds.PutData<DI16>( &len, 1 );

			exp_encdec( tok->tok, len, tok->tok ); 
			ds.PutBlock( (void*)tok->tok, len );
		}
	}

//	fclose(fp);

	// remove token list.
	while( !vm_tok_list.isempty() )
		delete vm_tok_list.pop();

	ds.Close();
}


void CCitrus::import_bin( char* pathname )
{
	DataStream ds;

	ds.Init( pathname );
	
	if ( FALSE == ds.OpenToRead() )
		return;

	sys_str::getpathonly( pathname, vm_script_path );

	// puts a header
	DI32 sig = 0;
	ds.GetData<DI32>( &sig );

	if ( 'HBS1' != sig )
		return;

	// HBS file structure
	//-------------------------------------------------------------
	//  case of id == 0
	//  --> id(1)|line(2)
	//  case of id > 0
	//  --> id(1)|line(2)|len(2)|data...

	while ( ! ds.IsEnd() )
	{
		VMTOKEN* tok = new VMTOKEN;

		DI8		kid		= 0;
		DI16	line	= 0;

		ds.GetData<DI8>( &kid );
		if ( kid > 0 )
		{
			DI16 line = 0;
			ds.GetData<DI16>( &line );

			tok->debug_line	= line;
			sys_str::copy( tok->tok, 256, get_keywordname( kid ) );
		}
		else
		{ // not-a-keyword
			DI16 line = 0;
			ds.GetData<DI16>( &line );

			tok->debug_line	= line;
			
			DI16 len = 0;
			ds.GetData<DI16>( &len );

			ds.GetBlock( (void*)tok->tok, len );

			exp_encdec( tok->tok, len, tok->tok ); 
		}

		vm_tok_list.add( tok );
	}

	ds.Close();
}


int CCitrus::getfunc( char* funcname )
{
	for (int k=0; k<vm_code.num; k++)
	{
		VMITEM* _t = vm_code[ k ];

		if ( TOK_PROC == _t->type )
		{
			SPEC_PROC* sp = (SPEC_PROC*)_t->spec;

			if ( sys_str::icompare( sp->name, funcname ) )
			{
				return k;
			}
		}
	}

	return -1;
}

// RTF return -1: exit, 0: continue, others: jump
int CCitrus::call( int start_ip, int bclearstack )
{
	if(start_ip < 0)
		return 0;

	VMINT vm_ip = 0;

	if ( start_ip > 0 )
	{ // changes the entry function.
		SPEC_CALL* sp	= (SPEC_CALL*)vm_code[ 0 ]->spec;
		sp->pos			= start_ip;
		sp->proc		= (SPEC_PROC*)vm_code[ start_ip ]->spec;
		sp->name[0]		= '!';	// mark for the original main entry has been changed.
	}

	while ( vm_ip<vm_code.num )
	{
		VMITEM* vmi = vm_code[ vm_ip ];
/*
	printf( "debug: %d (%s)\n", vmi->type, vmi->name );
	Sleep(100);
*/
		if ( RTFTBL[vmi->rtf] )
		{
			VMINT r = (this->*RTFTBL[vmi->rtf])( vmi );

			vm_exit_code = r;

			// -1: exit, 1: main entry function exit.
			if ( -1 == r || 
				(1 == r && TOK_ENDPROC == vmi->type) ||
				(1 == r && TOK_RETURN == vmi->type))
			{
				break;
			}
			else if ( r > 0 )
			{   // make jump
				vm_ip = r;
				continue;
			}
			else 
				vm_ip++;
		}
		else
			vm_ip++;

		// It's possible runtime code to have NULL functions for "ENDIF" cases.
		// So it's not an error.
	}


	if(bclearstack)
		clearstack();

	return (int)vm_r0;
}



void CCitrus::make_entry_call( char* entryname )
{
	VMITEM* _t = make_item( TOK_CALL, "CALL", -1, I_OP, -1 );

	SPEC_CALL* sp = new SPEC_CALL;
	sys_str::copy( sp->name, 256, entryname );

	sp->pos		= -1;
	sp->proc	= NULL;

	_t->spec = sp;

	make_item( TOK_STATEMENT, ";", -1, I_NOP, -1 );
}


void CCitrus::init( char* script, int b_bin, void (_stdcall *usercb)(VMINT, VMINT), char* entryname )
{
	if ( usercb )
		_vm_callback = usercb;

	memset( RTFTBL, 0, sizeof( RTFTBL ) );
	RTFTBL[ TOK_OPR_ADD				] = &CCitrus::I_ADD;
	RTFTBL[ TOK_OPR_ADDADD			] = &CCitrus::I_ADDADD;
	RTFTBL[ TOK_OPR_ADD_EQUAL		] = &CCitrus::I_ADD_EQUAL;
	RTFTBL[ TOK_OPR_SUBTRACT		] = &CCitrus::I_SUB;
	RTFTBL[ TOK_OPR_SUBSUB			] = &CCitrus::I_SUBSUB;
	RTFTBL[ TOK_OPR_SUBTRACT_EQUAL	] = &CCitrus::I_SUB_EQUAL;
	RTFTBL[ TOK_OPR_MULTIPLY		] = &CCitrus::I_MUL;
	RTFTBL[ TOK_OPR_MULTIPLY_EQUAL	] = &CCitrus::I_MUL_EQUAL;
	RTFTBL[ TOK_OPR_MOD				] = &CCitrus::I_MOD;
	RTFTBL[ TOK_OPR_MOD_EQUAL		] = &CCitrus::I_MOD_EQUAL;
	RTFTBL[ TOK_OPR_DIVIDE			] = &CCitrus::I_DIV;
	RTFTBL[ TOK_OPR_DIVIDE_EQUAL	] = &CCitrus::I_DIV_EQUAL;
	RTFTBL[ TOK_OPR_EQUAL			] = &CCitrus::I_EQUAL;
	RTFTBL[ TOK_MATH_SQRT			] = &CCitrus::I_SQRT;
	RTFTBL[ TOK_MATH_EXP			] = &CCitrus::I_EXP;
	RTFTBL[ TOK_MATH_COS			] = &CCitrus::I_COS;
	RTFTBL[ TOK_MATH_SIN			] = &CCitrus::I_SIN;
	RTFTBL[ TOK_SLEEP				] = &CCitrus::I_SLEEP;
	RTFTBL[ TOK_ECHO				] = &CCitrus::I_ECHO;
	RTFTBL[ TOK_ECHOLN				] = &CCitrus::I_ECHOLN;
	RTFTBL[ TOK_MSG					] = &CCitrus::I_MSG;
	RTFTBL[ TOK_GOTO				] = &CCitrus::I_GOTO;
	RTFTBL[ TOK_ELSE				] = &CCitrus::I_ELSE;
	RTFTBL[ TOK_PUSH				] = &CCitrus::I_PUSH;
	RTFTBL[ TOK_POP					] = &CCitrus::I_POP;
	RTFTBL[ TOK_PEEK				] = &CCitrus::I_PEEK;
	RTFTBL[ TOK_IF					] = &CCitrus::I_IF;
	RTFTBL[ TOK_CALL				] = &CCitrus::I_CALL;
	RTFTBL[ TOK_EVENT				] = &CCitrus::I_EVENT; // 'EVENT' in script world.
	RTFTBL[ TOK_ENDPROC				] = &CCitrus::I_ENDPROC;
	RTFTBL[ TOK_END					] = &CCitrus::I_END;
	RTFTBL[ TOK_GETPORTSIZE			] = &CCitrus::I_GETPORTSIZE;
	RTFTBL[ TOK_MAPPORT				] = &CCitrus::I_MAPPORT;
	RTFTBL[ TOK_PUTPORT				] = &CCitrus::I_PUTPORT;
	RTFTBL[ TOK_READPORT			] = &CCitrus::I_READPORT;
	RTFTBL[ TOK_WHILE				] = &CCitrus::I_WHILE;
	RTFTBL[ TOK_ENDWHILE			] = &CCitrus::I_ENDWHILE;
	RTFTBL[ TOK_BREAK				] = &CCitrus::I_BREAK;
	RTFTBL[ TOK_RETURN				] = &CCitrus::I_RETURN;
	RTFTBL[ TOK_GETOFFSET			] = &CCitrus::I_GETOFFSET;
	RTFTBL[ TOK_READSTACK			] = &CCitrus::I_READSTACK;
	RTFTBL[ TOK_POPAWAY				] = &CCitrus::I_POPAWAY;
	RTFTBL[ TOK_LOADMODULE			] = &CCitrus::I_LOADMODULE;
	RTFTBL[ TOK_GETPROC				] = &CCitrus::I_GETPROC;
	RTFTBL[ TOK_CALLPROC			] = &CCitrus::I_CALLPROC;	// 'FUNCTION' in script world.
	RTFTBL[ TOK_UNLOADMODULE		] = &CCitrus::I_UNLOADMODULE;
	RTFTBL[ TOK_GETRET				] = &CCitrus::I_GETRET;
	RTFTBL[ TOK_GETHOSTRET			] = &CCitrus::I_GETHOSTRET;
	RTFTBL[ TOK_PROC				] = &CCitrus::I_PROC;

	sys_str::getpathonly( script, vm_script_path );

	if ( b_bin )
		import_bin( script );
	else
		vm_make_tokens( script, NULL );		// tokenization

	if ( entryname )
		sys_str::copy( vm_entry_name, 256, entryname );

	if ( FALSE == TCMP( vm_entry_name, "NONE" ) )
		make_entry_call( vm_entry_name );

	_register_bind_function();

	vm_pass_0();							// lexical process
	vm_pass_1();							// scope, label, priority
}

void CCitrus::uninit()
{
	// label, vm_tok_list are removed at vm_pass_1()
	// deflist is removed at IGVM_make_tokens()

	// remove: format list
	while ( ! vm_format_list.isempty() )
		delete vm_format_list.pop();

	// remove: vm_items, proclist
	while ( ! vm_items.isempty() )
	{
		VMITEM* vmi = vm_items.pop();
		if ( TOK_CALL == vmi->type )
		{
			SPEC_CALL* sp_call = (SPEC_CALL*)vmi->spec;
			while ( ! sp_call->params.isempty() )
				sp_call->params.pop();
			while ( ! sp_call->params_byref.isempty() )
				sp_call->params_byref.pop();
		}
		else if ( TOK_PROC == vmi->type )
		{	
			SPEC_PROC* sp_proc = (SPEC_PROC*)vmi->spec;
			while ( ! sp_proc->params.isempty() )
				sp_proc->params.pop();
		}
		else if ( TOK_ARRAY_OPEN == vmi->type )
		{
			SPEC_ARRAY_OPEN* sp_ao = (SPEC_ARRAY_OPEN*)vmi->spec;
			while ( ! sp_ao->expr.isempty() )
				sp_ao->expr.pop();
		}

		if ( vmi->spec )
			delete vmi->spec;
		delete vmi;
	}

	// remove: vm_value_stack
	while ( ! vm_value_stack.isempty() )
		delete vm_value_stack.pop();

	// remove: varlist
	while ( ! varlist.isempty() )
	{
		VARITEM* vi = varlist.pop();
		if ( vi->realaddr )
			delete vi->realaddr;
		delete vi;
	}

	// remove: vm_code
	while ( ! vm_code.isempty() )
		vm_code.pop();

	// remove: vm_callstack
	VMASSERT( -1 == vm_exit_code || 0 == vm_callstack.num, EC_0018_USCS );
	while ( ! vm_callstack.isempty() )
		delete vm_callstack.pop();

	// remove: vm_calc_stack
	VMASSERT( -1 == vm_exit_code || 0 == vm_calc_stack.num, EC_0019_USCCS );
	while ( ! vm_calc_stack.isempty() )
		vm_calc_stack.pop();
	
	// remove: vm_vhandler
	while ( ! vm_vhandler.isempty() )
		vm_vhandler.pop();

	CCitrus::uninit_block_port();
}

void CCitrus::clearstack()
{
	while ( ! vm_value_stack.isempty() )
		delete vm_value_stack.pop();
}

void CCitrus::push_i( VMINT v )
{
	// push the returned value.
	ALLVALUE* av = new ALLVALUE;
	av->set_int( v );
	vm_value_stack.add( av );
}

void CCitrus::push_f( VMDOUBLE v )
{
	// push the returned value.
	ALLVALUE* av = new ALLVALUE;
	av->set_double( v );
	vm_value_stack.add( av );
}

void CCitrus::push_s( char* v )
{
	// push the returned value.
	ALLVALUE* av = new ALLVALUE;
	av->set_string( v );
	vm_value_stack.add( av );
}

int CCitrus::pop_i()
{
	ALLVALUE* _av = vm_value_stack.pop();
	VMASSERT( AV_INT == _av->type, EC_1028_PIWT );
	VMINT rv = _av->int_val;
	delete _av;
	return rv;
}

double CCitrus::pop_f()
{
	ALLVALUE* _av = vm_value_stack.pop();
	VMASSERT( AV_DOUBLE == _av->type, EC_1029_PFWT );
	VMDOUBLE rv = _av->dbl_val;
	delete _av;
	return rv;
}

void CCitrus::pop_s( char* str )
{
	ALLVALUE* _av = vm_value_stack.pop();
	VMASSERT( AV_STRING == _av->type, EC_1030_PSWT );
	sys_str::copy( str, 256, _av->string );
	delete _av;
}

int CCitrus::get_i( const char* varname )
{
	VMINT sid = get_var_sid( (char*)varname );
	VMASSERT1( -1 != sid, EC_1031_NSVAR": %s", varname );
	VMASSERT( VMCLS_I == varlist[ sid ]->cls.cls, EC_1032_ICT );
	return varlist[ sid ]->val.int_val;
}

double CCitrus::get_f( const char* varname )
{
	VMINT sid = get_var_sid( (char*)varname );
	VMASSERT1( -1 != sid, EC_1031_NSVAR": %s", varname );
	VMASSERT( VMCLS_F == varlist[ sid ]->cls.cls, EC_1032_ICT );
	return varlist[ sid ]->val.dbl_val;
}

const char* CCitrus::get_s( const char* varname )
{
	VMINT sid = get_var_sid( (char*)varname );
	VMASSERT1( -1 != sid, EC_1031_NSVAR": %s", varname );
	VMASSERT( VMCLS_S == varlist[ sid ]->cls.cls, EC_1032_ICT );
	return (const char*)varlist[ sid ]->val.string;
}

void CCitrus::set_verbatim_handler( const char* open_keyword, const char* close_keyword, 
										void (_stdcall *vhandler)(const char* text) )
{
	VHANDLER*	v = new VHANDLER;

	sys_str::copy( v->open_keyword, 64, (char*)open_keyword );
	sys_str::copy( v->close_keyword, 64, (char*)close_keyword );
	v->handler = vhandler;

	vm_vhandler.add( v );
}


void CCitrus::init_block_port( VMINT n_port )
{
	for (VMINT k=0; k<n_port; k++)
	{
		PORTITEM* portitem = new PORTITEM;
		portitem->size = 0;
		portitem->ptr  = NULL;
		vm_blocks.add( portitem );
	}
}

void CCitrus::uninit_block_port()
{
	while ( !vm_blocks.isempty() )
		delete vm_blocks.pop();
}

void CCitrus::register_block( VMINT port, void* voidptr, VMINT size )
{
	VMASSERT( port > -1 && port < vm_blocks.num, EC_1033_WPNUM );

	PORTITEM* portitem = vm_blocks[ port ];

	VMASSERT( 0 == portitem->size, EC_1034_PNEMPT );
	portitem->size		= size;
	portitem->ptr		= (unsigned char*)voidptr;
}

void CCitrus::bind_i( int* var, const char* varname )
{
	VMINT sid = make_vm_var_bind_i( varname, var, 1 );
	VMASSERT1( -1 != sid, EC_1031_NSVAR": %s", varname );
	VMASSERT( VMCLS_I == varlist[ sid ]->cls.cls, EC_1032_ICT );
}

void CCitrus::bind_f( double* var, const char* varname )
{
	VMINT sid = make_vm_var_bind_f( varname, var, 1 );
	VMASSERT1( -1 != sid, EC_1031_NSVAR": %s", varname );
	VMASSERT( VMCLS_F == varlist[ sid ]->cls.cls, EC_1032_ICT );
}

void CCitrus::bind_str( const char* var, const char* varname )
{
	VMINT sid = make_vm_var_bind_str( varname, var, 1 );
	VMASSERT1( -1 != sid, EC_1031_NSVAR": %s", varname );
	VMASSERT( VMCLS_S == varlist[ sid ]->cls.cls, EC_1032_ICT );
}

void CCitrus::bind_function( int (_stdcall *_bindfunc)(), const char* varname )
{
	BINDCALLITEM* bci = new BINDCALLITEM;

	bci->bindfunc	= _bindfunc;
	sys_str::copy( bci->varname, 256, (char*)varname );

	vm_bindcall_list.add( bci );
}

void CCitrus::_register_bind_function()
{
	while ( ! vm_bindcall_list.isempty() )
	{
		BINDCALLITEM* bci = vm_bindcall_list.qpop();

		VMITEM* _t = make_item( TOK_VAR_INT, (char*)bci->varname, -1, I_OP, -1 );
#pragma warning(push)
#pragma warning(disable:4311)
		_t->sid  = make_vm_var_i( _t, 0, 1 );
		write_vm_var_i( _t, (int)bci->bindfunc );
#pragma warning(pop)

		delete bci;
	}
}
