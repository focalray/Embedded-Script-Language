
#include "esl.h"

VMITEM* CCitrus::process_keyword_token( int &ti, int debug_line )
{
	VMITEM* _t = NULL;

	char* token = vm_tok_list[ ti ]->tok;
	char* ptoken = token;

	if ( TCMP( token, ";" ) )
	{// statement
		_t = make_item( TOK_STATEMENT, token, -1, I_NOP, debug_line );
	}
	else if ( TCMP( token, "/" ) )		// A / B
	{
		_t = make_item( TOK_OPR_DIVIDE, token, -1, I_OP, debug_line );
	}
	else if ( TCMP( token, "/=" ) )		// A /= B
	{// divide-equal
		_t = make_item( TOK_OPR_DIVIDE_EQUAL, token, -1, I_OP, debug_line );
	}
	else if ( TCMP( token, "+" ) )		// A + B
	{
		_t = make_item( TOK_OPR_ADD, token, -1, I_OP, debug_line );
	}
	else if ( TCMP( token, "++" ) )		// A++
	{
		_t = make_item( TOK_OPR_ADDADD, token, -1, I_OP, debug_line );
	}
	else if ( TCMP( token, "+=" ) )		// A += B
	{
		_t = make_item( TOK_OPR_ADD_EQUAL, token, -1, I_OP, debug_line );
	}
	else if ( TCMP( token, "-" ) )		// A - B
	{
		_t = make_item( TOK_OPR_SUBTRACT, token, -1, I_OP, debug_line );
	}
	else if ( TCMP( token, "--" ) )		// A--
	{
		_t = make_item( TOK_OPR_SUBSUB, token, -1, I_OP, debug_line );
	}
	else if ( TCMP( token, "-=" ) )		// A -= B
	{
		_t = make_item( TOK_OPR_SUBTRACT_EQUAL, token, -1, I_OP, debug_line );
	}
	else if ( TCMP( token, "*" ) )		// A * B
	{
		_t = make_item( TOK_OPR_MULTIPLY, token, -1, I_OP, debug_line );
	}
	else if ( TCMP( token, "*=" ) )		// A *= B
	{
		_t = make_item( TOK_OPR_MULTIPLY_EQUAL, token, -1, I_OP, debug_line );
	}
	else if ( TCMP( token, "%" ) )		// A % B
	{
		_t = make_item( TOK_OPR_MOD, token, -1, I_OP, debug_line );
	}
	else if ( TCMP( token, "%=" ) )		// A %= B
	{
		_t = make_item( TOK_OPR_MOD_EQUAL, token, -1, I_OP, debug_line );
	}
	else if ( TCMP( token, "(" ) )
	{
		_t = make_item( TOK_PRI_INC, token, -1, I_NOP, debug_line );
		_t->cls.clinkable = 0; // not linkable
	}
	else if ( TCMP( token, ")" ) )
	{
		_t = make_item( TOK_PRI_DEC, token, -1, I_NOP, debug_line );
		_t->cls.clinkable = 0; // not linkable
	}
	else if ( TCMP( token, "[" ) )
	{
		_t = make_item( TOK_ARRAY_OPEN, token, -1, I_NOP, debug_line );
		_t->cls.clinkable = 0; // not linkable

		SPEC_ARRAY_OPEN* sp = new SPEC_ARRAY_OPEN;
		sp->result_idx	= 0;
		_t->spec = sp;

		b_in_array_brace_at_unknown = TRUE;
	}
	else if ( TCMP( token, "]" ) )
	{
		b_in_array_brace_at_unknown = FALSE;

		_t = make_item( TOK_ARRAY_CLOSE, token, -1, I_NOP, debug_line );
		_t->cls.clinkable = 0; // not linkable
	}
	else if ( TCMP( token, "." ) )
	{// for FORMAT
		ti++;
		char* name_FE = vm_tok_list[ ti ]->tok; // operand A: FE

		VMITEM* last_vi = NULL;
		int isearch = vm_items.num - 1;
		while ( 1 )
		{
			last_vi = vm_items[ isearch ];
			if ( TOK_VAR_FORMAT == last_vi->type || TOK_PERIOD == last_vi->type )
				break;
			isearch--;
		}
		// VMASSERT( last_vi );
		VMASSERT( TOK_VAR_FORMAT == last_vi->type || TOK_PERIOD == last_vi->type, EC_0017_FAERR );


		_t = make_item( TOK_PERIOD, token, -1, I_OP, debug_line );

		SPEC_PERIOD* sp = new SPEC_PERIOD;
		sp->fe = NULL;

		FORMATITEM* fi = NULL;

		if ( TOK_VAR_FORMAT == last_vi->type )
		{
			fi = (FORMATITEM*)varlist[ last_vi->sid ]->val.void_ptr;
		}
		else 
		{ 
			SPEC_PERIOD* sp_period = (SPEC_PERIOD*)last_vi->spec;
			fi = (FORMATITEM*)sp_period->fe->link;
		}

		// search format element for offset information.
		for (int k=0; k<fi->list.num; k++)
		{
			FORMAT_ELE* fe = fi->list[ k ];

			if ( TCMP( fe->name, name_FE ) )
			{
				sp->fe		= fe;
				break;
			}
		}
		VMASSERT( sp->fe, EC_1008_FMTE );

		_t->spec = sp;
	}
	else if ( TCMP( token, "STRUCT" ) )
	{
		FORMATITEM* fi = new FORMATITEM;

		// format name
		ti++;
		sys_str::copy( fi->name, 256, vm_tok_list[ ti ]->tok );

		// skip {
		ti++;

		while ( 1 )
		{
			FORMAT_ELE*	fe = new FORMAT_ELE;

			fe->link	= NULL;
			fe->repeat	= 1;

			ti++;
			char* type_str = vm_tok_list[ ti ]->tok;
			ti++;
			char* name_str = vm_tok_list[ ti ]->tok;

			sys_str::copy( fe->name, 64, name_str );

			if ( TCMP( type_str, "INT" ) )
				fe->ftype	= FMT_INT;
			else if ( TCMP( type_str, "INT64" ) )
				fe->ftype	= FMT_INT64;
			else if ( TCMP( type_str, "WORD" ) )
				fe->ftype	= FMT_WORD;
			else if ( TCMP( type_str, "DWORD" ) )
				fe->ftype	= FMT_DWORD;
			else if ( TCMP( type_str, "SHORT" ) )
				fe->ftype	= FMT_SHORT;
			else if ( TCMP( type_str, "BYTE" ) )
				fe->ftype	= FMT_BYTE;
			else if ( TCMP( type_str, "FLOAT" ) )
				fe->ftype	= FMT_FLOAT;
			else if ( TCMP( type_str, "DOUBLE" ) )
				fe->ftype	= FMT_DOUBLE;
			else if ( TCMP( type_str, "STRING" ) )
				fe->ftype	= FMT_STR;
			else
			{// search format list: nested format.
				BOOL b_found = FALSE;
				for (VMINT k=0; k<vm_format_list.num; k++)
				{
					FORMATITEM*	fi = vm_format_list[ k ];

					if ( TCMP( fi->name, type_str ) )
					{// already defined format. use it.
						fe->ftype	= FMT_FORMAT;
						fe->link	= fi;
						b_found = TRUE;
						break;
					}
				}

				char emsg[256];
				sprintf( emsg, EC_1008_FMTE": unknown type( %s )", type_str );
				VMASSERT( b_found, emsg );
			}


			ti++; // ';' or '[]'

			// array test
			if ( TCMP( vm_tok_list[ ti ]->tok, "[" ) )
			{// array
				ti++;
				token = vm_tok_list[ ti ]->tok;

				VMINT type = get_text_type( token );

				if ( 1 == type || 2 == type || 3 == type )
					fe->repeat = trans_integer( token, type );

				ti++; // ']'
				ti++; // ';'
			}

			fi->list.add( fe );

			char* next = PEEKNEXT( ti );
			if ( TCMP( next, "}" ) )
			{
				ti++;
				break;
			}
		}

		vm_format_list.add( fi );

		return (VMITEM*)1;
	}
	else if ( TCMP( token, "PUSH" ) )
	{// PUSH <VAR|CONST>
		_t = make_item( TOK_PUSH, token, -1, I_OP, debug_line );

		SPEC_PUSH* sp = new SPEC_PUSH;
		sp->opr	= NULL;

		ti++;
		char* opr = vm_tok_list[ ti ]->tok; // operand A

		sp->opr = process_unknown_token( ptoken, ti, opr, NULL, debug_line );
		VMASSERT2( sp->opr, EC_0014_EOPUSH, _t );

		_t->spec = sp;
	}
	else if ( TCMP( token, "POP" ) )
	{// POP <VAR>
		_t = make_item( TOK_POP, token, -1, I_OP, debug_line );

		SPEC_POP* sp = new SPEC_POP;
		sp->opr	= NULL;

		ti++;
		char* opr = vm_tok_list[ ti ]->tok; // operand A
		VMASSERT2( 0 == get_text_type( opr ), EC_0015_EOPOP, _t ); // must be a string.

		VMINT utype = 0;
		sp->opr = process_unknown_token( ptoken, ti, opr, &utype, debug_line );
		VMASSERT2( sp->opr && TOK_CONST_INT != utype && TOK_CONST_DOUBLE != utype && TOK_CONST_STR != utype, EC_0015_EOPOP, _t );

		_t->spec = sp;
	}
	else if ( TCMP( token, "PEEK" ) )
	{// PEEK <VAR>
		_t = make_item( TOK_PEEK, token, -1, I_OP, debug_line );

		SPEC_PEEK* sp = new SPEC_PEEK;
		sp->opr = NULL;

		ti++;
		char* opr = vm_tok_list[ ti ]->tok; // operand A
		VMASSERT2( 0 == get_text_type( opr ), EC_0016_EOPEEK, _t ); // must be a string.

		VMINT utype = 0;
		sp->opr = process_unknown_token( ptoken, ti, opr, &utype, debug_line );
		VMASSERT2( sp->opr && TOK_CONST_INT != utype && TOK_CONST_DOUBLE != utype && TOK_CONST_STR != utype, EC_0016_EOPEEK, _t );

		_t->spec = sp;
	}
	else if ( TCMP( token, "POPAWAY" ) )
	{// POPAWAY <CONST-INT>
		_t = make_item( TOK_POPAWAY, token, -1, I_OP, debug_line );

		SPEC_POPAWAY* sp = new SPEC_POPAWAY;
		sp->level = 0;

		ti++;
		char* opr1 = vm_tok_list[ ti ]->tok;		// operand A
		int type = get_text_type( opr1 );
		VMASSERT2( 0 != type, EC_0021_EPAW, _t );	// must be a number.
		sp->level = trans_integer( opr1, type );
		VMASSERT2( sp->level >= 0, EC_0021_EPAW, _t );			// must be >= 0

		_t->spec = sp;
	}
	else if ( TCMP( token, "READSTACK" ) )
	{// READSTACK <LEVEL-INT> <VAR>
		_t = make_item( TOK_READSTACK, token, -1, I_OP, debug_line );

		SPEC_READSTACK* sp = new SPEC_READSTACK;
		sp->level = 0;
		sp->opr   = NULL;

		ti++;
		char* opr1 = vm_tok_list[ ti ]->tok;		// operand A
		int type = get_text_type( opr1 );
		VMASSERT2( 0 != type, EC_0020_ERS, _t );	// must be a number.
		sp->level = trans_integer( opr1, type );
		VMASSERT2( sp->level >= 0, EC_0020_ERS, _t );			// must be >= 0

		ti++;
		char* opr2 = vm_tok_list[ ti ]->tok; // operand B
		VMASSERT2( 0 == get_text_type( opr2 ), EC_0020_ERS, _t ); // must be a string.

		VMINT utype = 0;
		sp->opr = process_unknown_token( ptoken, ti, opr2, &utype, debug_line );
		VMASSERT2( sp->opr && TOK_CONST_INT != utype && TOK_CONST_DOUBLE != utype && TOK_CONST_STR != utype, EC_0020_ERS, _t );

		_t->spec = sp;
	}
	else if ( TCMP( token, "LOADMODULE" ) )
	{// LOADMODULE <PATHNAME> <INT-VAR>
		_t = make_item( TOK_LOADMODULE, token, -1, I_OP, debug_line );

		SPEC_LOADMODULE* sp = new SPEC_LOADMODULE;
		sp->modname[0]	= NULL;
		sp->hnd			= NULL;

		ti++;
		char* opr = vm_tok_list[ ti ]->tok; // operand A
		VMASSERT2( 0 == get_text_type( opr ), EC_0023_ELDMD, _t ); // must be a string.
		opr[ sys_str::length( opr ) - 1 ] = NULL;
		sys_str::copy( sp->modname, 256, opr+1 );

		ti++;
		char* opr2 = vm_tok_list[ ti ]->tok; // operand B
		VMASSERT2( 0 == get_text_type( opr2 ), EC_0023_ELDMD, _t ); // must be a string.
		VMINT utype = 0;
		sp->hnd = process_unknown_token( ptoken, ti, opr2, &utype, debug_line );
		VMASSERT2( sp->hnd && TOK_CONST_INT != utype && TOK_CONST_DOUBLE != utype && TOK_CONST_STR != utype, EC_0023_ELDMD, _t );

		_t->spec = sp;
	}
	else if ( TCMP( token, "UNLOADMODULE" ) )
	{// UNLOADMODULE <INT-VAR>
		_t = make_item( TOK_UNLOADMODULE, token, -1, I_OP, debug_line );

		SPEC_UNLOADMODULE* sp = new SPEC_UNLOADMODULE;
		sp->hnd	= NULL;

		ti++;
		char* opr2 = vm_tok_list[ ti ]->tok; // operand B
		VMASSERT2( 0 == get_text_type( opr2 ), EC_0026_EULDMD, _t ); // must be a string.
		VMINT utype = 0;
		sp->hnd = process_unknown_token( ptoken, ti, opr2, &utype, debug_line );
		VMASSERT2( sp->hnd && TOK_CONST_INT != utype && TOK_CONST_DOUBLE != utype && TOK_CONST_STR != utype, EC_0026_EULDMD, _t );

		_t->spec = sp;
	}
	else if ( TCMP( token, "GETHOSTRET" ) )
	{// GETHOSTRET <INT-VAR>
		_t = make_item( TOK_GETHOSTRET, token, -1, I_OP, debug_line );

		SPEC_GETHOSTRET* sp = new SPEC_GETHOSTRET;
		sp->ret	= NULL;

		ti++;
		char* opr2 = vm_tok_list[ ti ]->tok; // operand B
		VMASSERT2( 0 == get_text_type( opr2 ), EC_0027_EGHRET, _t ); // must be a string.
		VMINT utype = 0;
		sp->ret = process_unknown_token( ptoken, ti, opr2, &utype, debug_line );
		VMASSERT2( sp->ret && TOK_CONST_INT != utype && TOK_CONST_DOUBLE != utype && TOK_CONST_STR != utype, EC_0027_EGHRET, _t );

		_t->spec = sp;
	}
	else if ( TCMP( token, "GETRET" ) )
	{// GETRET <DOUBLE-VAR>
		_t = make_item( TOK_GETRET, token, -1, I_OP, debug_line );

		SPEC_GETRET* sp = new SPEC_GETRET;
		sp->ret	= NULL;

		ti++;
		char* opr2 = vm_tok_list[ ti ]->tok; // operand B
		VMASSERT2( 0 == get_text_type( opr2 ), EC_0027_EGRET, _t ); // must be a string.
		VMINT utype = 0;
		sp->ret = process_unknown_token( ptoken, ti, opr2, &utype, debug_line );
		VMASSERT2( sp->ret && TOK_CONST_INT != utype && TOK_CONST_DOUBLE != utype && TOK_CONST_STR != utype, EC_0027_EGRET, _t );

		_t->spec = sp;
	}
	else if ( TCMP( token, "GETPROC" ) )
	{// GETPROC <MODHANDLE:INT-VAR> <PROCNAME> <INT-VAR>
		_t = make_item( TOK_GETPROC, token, -1, I_OP, debug_line );

		SPEC_GETPROC* sp = new SPEC_GETPROC;
		sp->hnd			= NULL;
		sp->modhnd		= NULL;
		sp->prcname[0]	= 0;

		ti++;
		char* opr0 = vm_tok_list[ ti ]->tok; // operand B
		VMASSERT2( 0 == get_text_type( opr0 ), EC_0024_EGPRC, _t ); // must be a string.
		VMINT utype = 0;
		sp->modhnd = process_unknown_token( ptoken, ti, opr0, &utype, debug_line );
		VMASSERT2( sp->modhnd && TOK_CONST_INT != utype && TOK_CONST_DOUBLE != utype && TOK_CONST_STR != utype, EC_0024_EGPRC, _t );

		ti++;
		char* opr1 = vm_tok_list[ ti ]->tok; // operand A
		VMASSERT2( 0 == get_text_type( opr1 ), EC_0024_EGPRC, _t ); // must be a string.
		opr1[ sys_str::length( opr1 ) - 1 ] = NULL;
		sys_str::copy( sp->prcname, 256, opr1+1 );

		ti++;
		char* opr2 = vm_tok_list[ ti ]->tok; // operand B
		VMASSERT2( 0 == get_text_type( opr2 ), EC_0024_EGPRC, _t ); // must be a string.
		sp->hnd = process_unknown_token( ptoken, ti, opr2, &utype, debug_line );
		VMASSERT2( sp->hnd && TOK_CONST_INT != utype && TOK_CONST_DOUBLE != utype && TOK_CONST_STR != utype, EC_0024_EGPRC, _t );

		_t->spec = sp;
	}
	else if ( TCMP( token, "RETURN" ) )
	{// RETURN <VAR|CONST>
		_t = make_item( TOK_RETURN, token, -1, I_OP, debug_line );
	}
	else if ( TCMP( token, "GETPORTSIZE" ) )
	{// GETPORTSIZE <VAR|CONST> <VAR>
		_t = make_item( TOK_GETPORTSIZE, token, -1, I_OP, debug_line );

		SPEC_GETPORTSIZE* sp = new SPEC_GETPORTSIZE;
		sp->opr1 = NULL;
		sp->opr2 = NULL;

		ti++;
		char* opr1 = vm_tok_list[ ti ]->tok; // operand A
		sp->opr1 = process_unknown_token( ptoken, ti, opr1, NULL, debug_line );

		ti++;
		char* opr2 = vm_tok_list[ ti ]->tok; // operand B
		VMASSERT2( 0 == get_text_type( opr2 ), EC_1010_GPSERR, _t ); // must be a string.

		VMINT utype = 0;
		sp->opr2 = process_unknown_token( ptoken, ti, opr2, &utype, debug_line );

		VMASSERT2( sp->opr2 && TOK_CONST_INT != utype && TOK_CONST_DOUBLE != utype && TOK_CONST_STR != utype, EC_1010_GPSERR, _t );

		_t->spec = sp;
	}
	else if ( TCMP( token, "GETOFFSET" ) )
	{
		_t = make_item( TOK_GETOFFSET, token, -1, I_OP, debug_line );

		SPEC_GETOFFSET* sp = new SPEC_GETOFFSET;
		sp->offset	= 0;
		sp->opr2	= NULL;

		ti++;
		char* opr1 = vm_tok_list[ ti ]->tok; // operand A

		FORMATITEM* fi = search_format_item( opr1 ); // token = FORMAT name (not instance name)

		ti++; // skip '.'

		ti++;
		char* ele_name = vm_tok_list[ ti ]->tok;

		sp->offset = -1;

		for (int k=0; k<fi->list.num; k++)
		{
			if ( TCMP( fi->list[ k ]->name, ele_name ) )
			{
				sp->offset = fi->list[ k ]->offset;
				break;
			}
		}
		VMASSERT2( -1 != sp->offset, EC_1011_NMOERR, _t );

		ti++;
		char* opr2 = vm_tok_list[ ti ]->tok; // operand B
		VMASSERT2( 0 == get_text_type( opr2 ), EC_1012_GOERR, _t ); // must be a string.

		VMINT utype = 0;
		sp->opr2 = process_unknown_token( ptoken, ti, opr2, &utype, debug_line );
		VMASSERT2( sp->opr2 && TOK_CONST_INT != utype && TOK_CONST_DOUBLE != utype && TOK_CONST_STR != utype, EC_1012_GOERR, _t );

		_t->spec = sp;
	}
	else if ( TCMP( token, "#PORTPACK" ) )
	{// PORTPACK <CONST>
		ti++;
		char* portpack = vm_tok_list[ ti ]->tok; // operand A

		VMINT type = get_text_type( portpack );
		VMASSERT( 0 != type, EC_1013_PPOCV );

		vm_datapack = trans_integer( portpack, type );

		return (VMITEM*)1;
	}
	else if ( TCMP( token, "MAPPORT" ) )
	{// MAPPORT <VAR|CONST> <FMT-VAR>
		_t = make_item( TOK_MAPPORT, token, -1, I_OP, debug_line );

		SPEC_MAPPORT* sp = new SPEC_MAPPORT;
		sp->opr1	= NULL;
		sp->opr2	= NULL;

		ti++;
		char* opr1 = vm_tok_list[ ti ]->tok; // operand A
		sp->opr1 = process_unknown_token( ptoken, ti, opr1, NULL, debug_line );

		ti++;
		char* opr2 = vm_tok_list[ ti ]->tok; // operand B
		VMASSERT2( 0 == get_text_type( opr2 ), EC_1014_MPERR, _t ); // must be a string.

		VMINT utype = 0;
		sp->opr2 = process_unknown_token( ptoken, ti, opr2, &utype, debug_line );
		VMASSERT2( sp->opr2 && TOK_VAR_FORMAT == utype, EC_1014_MPERR, _t );

		_t->spec = sp;
	}
	else if ( TCMP( token, "PUTPORT" ) )
	{// PUTPORT <VAR|CONST> <FMT-VAR>
		_t = make_item( TOK_PUTPORT, token, -1, I_OP, debug_line );

		SPEC_PUTPORT* sp = new SPEC_PUTPORT;
		sp->opr1	= NULL;
		sp->opr2	= NULL;

		ti++;
		char* opr1 = vm_tok_list[ ti ]->tok; // operand A
		sp->opr1 = process_unknown_token( ptoken, ti, opr1, NULL, debug_line );

		ti++;
		char* opr2 = vm_tok_list[ ti ]->tok; // operand B
		VMASSERT2( 0 == get_text_type( opr2 ), EC_1014_PPERR, _t ); // must be a string.

		VMINT utype = 0;
		sp->opr2 = process_unknown_token( ptoken, ti, opr2, &utype, debug_line );
		VMASSERT2( sp->opr2 && TOK_VAR_FORMAT == utype, EC_1014_PPERR, _t );

		_t->spec = sp;
	}
	else if ( TCMP( token, "READPORT" ) )
	{// READPORT <VAR|CONST> <FMT-VAR|VAR|CONST> <VAR>
		_t = make_item( TOK_READPORT, token, -1, I_OP, debug_line );

		SPEC_READPORT* sp = new SPEC_READPORT;
		sp->opr1	= NULL;
		sp->opr2	= NULL;
		sp->opr3	= NULL;

		VMINT utype = 0;

		ti++;
		char* opr1 = vm_tok_list[ ti ]->tok; // operand A
		sp->opr1 = process_unknown_token( ptoken, ti, opr1, NULL, debug_line );

		ti++;
		char* opr2 = vm_tok_list[ ti ]->tok; // operand B
		//			VMASSERT2( 0 == get_text_type( opr2 ), "Error on READPORT.", _t ); // must be a string.
		sp->opr2 = process_unknown_token( ptoken, ti, opr2, &utype, debug_line );

		ti++;
		char* opr3 = vm_tok_list[ ti ]->tok; // operand C
		VMASSERT2( 0 == get_text_type( opr3 ), EC_1015_RPERR, _t ); // must be a string.
		sp->opr3 = process_unknown_token( ptoken, ti, opr3, &utype, debug_line );
		VMASSERT2( sp->opr3 && TOK_CONST_INT != utype && TOK_CONST_DOUBLE != utype && TOK_CONST_STR != utype, EC_1015_RPERR, _t );

		_t->spec = sp;
	}
	else if ( TCMP( token, "ECHO" ) )
	{
		_t = make_item( TOK_ECHO, token, -1, I_OP, debug_line );
	}
	else if ( TCMP( token, "ECHOLN" ) )
	{
		_t = make_item( TOK_ECHOLN, token, -1, I_OP, debug_line );
	}
	else if ( TCMP( token, "MSG" ) )
	{
		_t = make_item( TOK_MSG, token, -1, I_OP, debug_line );
	}
	else if ( TCMP( token, "BYTE" ) )
	{// parse BYTE
		_t = make_item( TOK_VAR_BYTE, token, -1, I_NOP, debug_line );

		// var name
		ti++;
		token = vm_tok_list[ ti ]->tok;
		sys_str::copy( _t->name, 256, token );

		ti++;
		token = vm_tok_list[ ti ]->tok;

		if ( TCMP( token, ";" ) )
		{// uninitialized var;  set to zero.
			_t->sid = make_vm_var_i( _t, 0, 1 );
			ti--;
		}
		else if ( TCMP( token, "[" ) )
		{// array
			ti++;
			token = vm_tok_list[ ti ]->tok;

			VMINT type = get_text_type( token );

			if ( 1 == type || 2 == type || 3 == type )
			{
				_t->cls.carray	= 1; // property change must be placed before make_vm_var_xx
				_t->sid = make_vm_var_i( _t, 0, trans_integer( token, type ) );
			}

			ti++;
			token = vm_tok_list[ ti ]->tok;
			VMASSERT2( TCMP( token, "]" ), EC_1016_BERR, _t );
		}
		else
		{
			VMASSERT2( TCMP( token, "=" ), EC_1016_BERR, _t );

			ti++;
			token = vm_tok_list[ ti ]->tok;

			VMINT sign = 1;
			if ( TCMP( token, "-" ) )
			{
				sign = -1;
				ti++;
				token = vm_tok_list[ ti ]->tok;
			}

			VMINT type = get_text_type( token );

			if ( 1 == type || 2 == type || 3 == type )
			{// init value
				_t->sid = make_vm_var_i( _t, sign * trans_integer( token, type ), 1 ); 
			}
			else
			{
				VMASSERT2( 0, EC_2004_TMISM, _t );
			}
		}
	}
	else if ( TCMP( token, "WORD" ) )
	{// parse WORD
		_t = make_item( TOK_VAR_WORD, token, -1, I_NOP, debug_line );

		// var name
		ti++;
		token = vm_tok_list[ ti ]->tok;
		sys_str::copy( _t->name, 256, token );

		ti++;
		token = vm_tok_list[ ti ]->tok;

		if ( TCMP( token, ";" ) )
		{// uninitialized var;  set to zero.
			_t->sid = make_vm_var_i( _t, 0, 1 );
			ti--;
		}
		else if ( TCMP( token, "[" ) )
		{// array
			ti++;
			token = vm_tok_list[ ti ]->tok;

			VMINT type = get_text_type( token );

			if ( 1 == type || 2 == type || 3 == type )
			{
				_t->cls.carray	= 1; // property change must be placed before make_vm_var_xx
				_t->sid = make_vm_var_i( _t, 0, trans_integer( token, type ) );
			}

			ti++;
			token = vm_tok_list[ ti ]->tok;
			VMASSERT2( TCMP( token, "]" ), EC_1017_EWRD, _t );
		}
		else
		{
			VMASSERT2( TCMP( token, "=" ), EC_1017_EWRD, _t );

			ti++;
			token = vm_tok_list[ ti ]->tok;

			VMINT sign = 1;
			if ( TCMP( token, "-" ) )
			{
				sign = -1;
				ti++;
				token = vm_tok_list[ ti ]->tok;
			}

			VMINT type = get_text_type( token );

			if ( 1 == type || 2 == type || 3 == type )
			{// init value
				_t->sid = make_vm_var_i( _t, sign * trans_integer( token, type ), 1 ); 
			}
			else
			{
				VMASSERT2( 0, EC_2004_TMISM, _t );
			}
		}
	}
	else if ( TCMP( token, "DWORD" ) )
	{// parse DWORD
		_t = make_item( TOK_VAR_DWORD, token, -1, I_NOP, debug_line );

		// var name
		ti++;
		token = vm_tok_list[ ti ]->tok;
		sys_str::copy( _t->name, 256, token );

		ti++;
		token = vm_tok_list[ ti ]->tok;

		if ( TCMP( token, ";" ) )
		{// uninitialized var;  set to zero.
			_t->sid = make_vm_var_i( _t, 0, 1 );
			ti--;
		}
		else if ( TCMP( token, "[" ) )
		{// array
			ti++;
			token = vm_tok_list[ ti ]->tok;

			VMINT type = get_text_type( token );

			if ( 1 == type || 2 == type || 3 == type )
			{
				_t->cls.carray	= 1; // property change must be placed before make_vm_var_xx
				_t->sid = make_vm_var_i( _t, 0, trans_integer( token, type ) );
			}

			ti++;
			token = vm_tok_list[ ti ]->tok;
			VMASSERT2( TCMP( token, "]" ), EC_1017_EDWRD, _t );
		}
		else
		{
			VMASSERT2( TCMP( token, "=" ), EC_1017_EDWRD, _t );

			ti++;
			token = vm_tok_list[ ti ]->tok;

			VMINT sign = 1;
			if ( TCMP( token, "-" ) )
			{
				sign = -1;
				ti++;
				token = vm_tok_list[ ti ]->tok;
			}

			VMINT type = get_text_type( token );

			if ( 1 == type || 2 == type || 3 == type )
			{// init value
				_t->sid = make_vm_var_i( _t, sign * trans_integer( token, type ), 1 ); 
			}
			else
			{
				VMASSERT2( 0, EC_2004_TMISM, _t );
			}
		}
	}
	else if ( TCMP( token, "SHORT" ) )
	{// parse SHORT
		_t = make_item( TOK_VAR_SHORT, token, -1, I_NOP, debug_line );

		// var name
		ti++;
		token = vm_tok_list[ ti ]->tok;
		sys_str::copy( _t->name, 256, token );

		ti++;
		token = vm_tok_list[ ti ]->tok;

		if ( TCMP( token, ";" ) )
		{// uninitialized var;  set to zero.
			_t->sid = make_vm_var_i( _t, 0, 1 );
			ti--;
		}
		else if ( TCMP( token, "[" ) )
		{// array
			ti++;
			token = vm_tok_list[ ti ]->tok;

			VMINT type = get_text_type( token );

			if ( 1 == type || 2 == type || 3 == type )
			{
				_t->cls.carray	= 1; // property change must be placed before make_vm_var_xx
				_t->sid = make_vm_var_i( _t, 0, trans_integer( token, type ) );
			}

			ti++;
			token = vm_tok_list[ ti ]->tok;
			VMASSERT2( TCMP( token, "]" ), EC_1017_ESHRT, _t );
		}
		else
		{
			VMASSERT2( TCMP( token, "=" ), EC_1017_ESHRT, _t );

			ti++;
			token = vm_tok_list[ ti ]->tok;

			VMINT sign = 1;
			if ( TCMP( token, "-" ) )
			{
				sign = -1;
				ti++;
				token = vm_tok_list[ ti ]->tok;
			}

			VMINT type = get_text_type( token );

			if ( 1 == type || 2 == type || 3 == type )
			{// init value
				_t->sid = make_vm_var_i( _t, sign * trans_integer( token, type ), 1 ); 
			}
			else
			{
				VMASSERT2( 0, EC_2004_TMISM, _t );
			}
		}
	}
	else if ( TCMP( token, "INT" ) )
	{// parse INT
		_t = make_item( TOK_VAR_INT, token, -1, I_NOP, debug_line );

		// var name
		ti++;
		token = vm_tok_list[ ti ]->tok;
		sys_str::copy( _t->name, 256, token );

		ti++;
		token = vm_tok_list[ ti ]->tok;

		if ( TCMP( token, ";" ) )
		{// uninitialized var;  set to zero.
			_t->sid = make_vm_var_i( _t, 0, 1 );
			ti--;
		}
		else if ( TCMP( token, "[" ) )
		{// array
			ti++;
			token = vm_tok_list[ ti ]->tok;

			VMINT type = get_text_type( token );

			if ( 1 == type || 2 == type || 3 == type )
			{
				_t->cls.carray	= 1; // property change must be placed before make_vm_var_xx
				_t->sid = make_vm_var_i( _t, 0, trans_integer( token, type ) );
			}

			ti++;
			token = vm_tok_list[ ti ]->tok;
			VMASSERT2( TCMP( token, "]" ), EC_1018_EINT, _t );
		}
		else
		{
			VMASSERT2( TCMP( token, "=" ), EC_1018_EINT, _t );

			ti++;
			token = vm_tok_list[ ti ]->tok;

			VMINT sign = 1;
			if ( TCMP( token, "-" ) )
			{
				sign = -1;
				ti++;
				token = vm_tok_list[ ti ]->tok;
			}

			VMINT type = get_text_type( token );

			if ( 1 == type || 2 == type || 3 == type )
			{// init value
				_t->sid = make_vm_var_i( _t, sign * trans_integer( token, type ), 1 ); 
			}
			else
			{
				VMASSERT2( 0, EC_2004_TMISM, _t );
			}
		}
	}
	else if ( TCMP( token, "INT64" ) )
	{// parse INT64
		_t = make_item( TOK_VAR_INT64, token, -1, I_NOP, debug_line );

		// var name
		ti++;
		token = vm_tok_list[ ti ]->tok;
		sys_str::copy( _t->name, 256, token );

		ti++;
		token = vm_tok_list[ ti ]->tok;

		if ( TCMP( token, ";" ) )
		{// uninitialized var;  set to zero.
			_t->sid = make_vm_var_i( _t, 0, 1 );
			ti--;
		}
		else if ( TCMP( token, "[" ) )
		{// array
			ti++;
			token = vm_tok_list[ ti ]->tok;

			VMINT type = get_text_type( token );

			if ( 1 == type || 2 == type || 3 == type )
			{
				_t->cls.carray	= 1; // property change must be placed before make_vm_var_xx
				_t->sid = make_vm_var_i( _t, 0, trans_integer( token, type ) );
			}

			ti++;
			token = vm_tok_list[ ti ]->tok;
			VMASSERT2( TCMP( token, "]" ), EC_1018_EINT64, _t );
		}
		else
		{
			VMASSERT2( TCMP( token, "=" ), EC_1018_EINT64, _t );

			ti++;
			token = vm_tok_list[ ti ]->tok;

			VMINT sign = 1;
			if ( TCMP( token, "-" ) )
			{
				sign = -1;
				ti++;
				token = vm_tok_list[ ti ]->tok;
			}

			VMINT type = get_text_type( token );

			if ( 1 == type || 2 == type || 3 == type )
			{// init value
				_t->sid = make_vm_var_i( _t, sign * trans_integer( token, type ), 1 ); 
			}
			else
			{
				VMASSERT2( 0, EC_2004_TMISM, _t );
			}
		}
	}
	else if ( TCMP( token, "FLOAT" ) )
	{// parse float
		_t = make_item( TOK_VAR_FLOAT, token, -1, I_NOP, debug_line );

		// var name
		ti++;
		token = vm_tok_list[ ti ]->tok;
		sys_str::copy( _t->name, 256, token );

		ti++;
		token = vm_tok_list[ ti ]->tok;

		if ( TCMP( token, ";" ) )
		{// uninitialized var;  set to zero.
			_t->sid = make_vm_var_f( _t, 0.0, 1 );
			ti--;
		}
		else if ( TCMP( token, "[" ) )
		{// array
			ti++;
			token = vm_tok_list[ ti ]->tok;

			VMINT type = get_text_type( token );

			if ( 1 == type || 2 == type )
			{// init value
				_t->cls.carray	= 1; // property change must be placed before make_vm_var_xx
				_t->sid = make_vm_var_f( _t, 0.0, trans_integer( token, type ) );
			}

			ti++;
			token = vm_tok_list[ ti ]->tok;
			VMASSERT2( TCMP( token, "]" ), EC_1019_EFLT, _t );
		}
		else
		{
			VMASSERT2( TCMP( token, "=" ), EC_1019_EFLT, _t );

			ti++;
			token = vm_tok_list[ ti ]->tok;

			VMFLOAT sign = 1.0;
			if ( TCMP( token, "-" ) )
			{
				sign = -1.0;
				ti++;
				token = vm_tok_list[ ti ]->tok;
			}

			VMINT type = get_text_type( token );

			if ( 1 == type || 2 == type )
			{// init value
				_t->sid = make_vm_var_f( _t, sign * atof( token ), 1 );
			}
			else
			{
				VMASSERT2( 0, EC_2004_TMISM, _t );
			}
		}
	}
	else if ( TCMP( token, "DOUBLE" ) )
	{// parse DOUBLE
		_t = make_item( TOK_VAR_DOUBLE, token, -1, I_NOP, debug_line );

		// var name
		ti++;
		token = vm_tok_list[ ti ]->tok;
		sys_str::copy( _t->name, 256, token );

		ti++;
		token = vm_tok_list[ ti ]->tok;

		if ( TCMP( token, ";" ) )
		{// uninitialized var;  set to zero.
			_t->sid = make_vm_var_f( _t, 0.0, 1 );
			ti--;
		}
		else if ( TCMP( token, "[" ) )
		{// array
			ti++;
			token = vm_tok_list[ ti ]->tok;

			VMINT type = get_text_type( token );

			if ( 1 == type || 2 == type )
			{// init value
				_t->cls.carray	= 1; // property change must be placed before make_vm_var_xx
				_t->sid = make_vm_var_f( _t, 0.0, trans_integer( token, type ) );
			}

			ti++;
			token = vm_tok_list[ ti ]->tok;
			VMASSERT2( TCMP( token, "]" ), EC_1020_EDBL, _t );
		}
		else
		{
			VMASSERT2( TCMP( token, "=" ), EC_1020_EDBL, _t );

			ti++;
			token = vm_tok_list[ ti ]->tok;

			VMDOUBLE sign = 1.0;
			if ( TCMP( token, "-" ) )
			{
				sign = -1.0;
				ti++;
				token = vm_tok_list[ ti ]->tok;
			}

			VMINT type = get_text_type( token );

			if ( 1 == type || 2 == type )
			{// init value
				_t->sid = make_vm_var_f( _t, sign * atof( token ), 1 );
			}
			else
			{
				VMASSERT2( 0, EC_2004_TMISM, _t );
			}
		}
	}
	else if ( TCMP( token, "STRING" ) )
	{// parse string
		_t = make_item( TOK_VAR_STR, token, -1, I_NOP, debug_line );

		// var name
		ti++;
		token = vm_tok_list[ ti ]->tok;
		sys_str::copy( _t->name, 256, token );

		// test next 
		ti++;
		token = vm_tok_list[ ti ]->tok;

		if ( TCMP( token, ";" ) )
		{// uninitialized var;  set to zero.
			_t->sid = make_vm_var_str( _t, "", 1 );
		}
		else if ( TCMP( token, "[" ) )
		{// array
			ti++;
			token = vm_tok_list[ ti ]->tok;

			VMINT type = get_text_type( token );

			if ( 1 == type || 2 == type )
			{// init value
				_t->cls.carray	= 1; // property change must be placed before make_vm_var_xx
				_t->sid = make_vm_var_str( _t, "", trans_integer( token, type ) );
			}

			ti++;
			token = vm_tok_list[ ti ]->tok;
			VMASSERT2( TCMP( token, "]" ), EC_1021_ESTR, _t );
		}
		else
		{
			VMASSERT2( TCMP( token, "=" ), EC_1021_ESTR, _t );

			ti++;
			token = vm_tok_list[ ti ]->tok;

			VMINT type = get_text_type( token );

			// removes the last double quote.
			token[ sys_str::length( token ) - 1 ] = NULL;

			if ( 0 == type )
			{// init value
				_t->sid = make_vm_var_str( _t, token+1, 1 );
			}
			else
			{
				VMASSERT2( 0, EC_2004_TMISM, _t );
			}
		}
	}

	return _t;
}


void CCitrus::vm_pass_0()
{
	sys_list<TOKTYPE>		scope_level;
	int						scope_level_counter = 0;

	VMINT ti = 0;

	while ( ti<vm_tok_list.num )
	{
		char*	token = vm_tok_list[ ti ]->tok;
		char*	ptoken = token;
		VMINT	debug_line = vm_tok_list[ ti ]->debug_line;

//		OutputDebugStringA(token); OutputDebugStringA(vm_tok_list[ ti+1 ]->tok); OutputDebugStringA("\n");

		VMITEM* vi = process_keyword_token( ti, debug_line );

		if(vi)
		{
			ti++;
			continue;
		}

		else if ( TCMP( token, "IF" ) )
		{// IF <condition>
		 // <condition> can not have calculation.
			ti++;
			char* opr1 = vm_tok_list[ ti ]->tok; // operand A
			ti++;
			char* oper = vm_tok_list[ ti ]->tok; // operator 
			ti++;
			char* opr2 = vm_tok_list[ ti ]->tok; // operand B

			VMITEM* _t = make_item( TOK_IF, token, -1, I_OP, debug_line );
			SPEC_IF* sp = new SPEC_IF;
			sp->opr		= TOK_NONE;
			sp->opr1	= NULL;
			sp->opr2	= NULL;
			sp->scope[0] = sp->scope[1] = sp->scope[2] = 0;

			_t->spec = sp;

			sp->opr1 = process_unknown_token( ptoken, ti, opr1, NULL, debug_line );
			sp->opr2 = process_unknown_token( ptoken, ti, opr2, NULL, debug_line );
			VMASSERT2( sp->opr1 && sp->opr2, EC_1022_EOIF, _t );

			sp->opr1->rtf = (TOKTYPE)NULL;
			sp->opr2->rtf = (TOKTYPE)NULL;

			if ( TCMP( oper, "==" ) )
				sp->opr = TOK_OPR_SAME;
			else if ( TCMP( oper, "!=" ) )
				sp->opr = TOK_OPR_DIFF;
			else if ( TCMP( oper, "<=" ) )
				sp->opr = TOK_OPR_LEQUAL;
			else if ( TCMP( oper, ">=" ) )
				sp->opr = TOK_OPR_GEQUAL;
			else if ( TCMP( oper, "<" ) )
				sp->opr = TOK_OPR_LESS;
			else if ( TCMP( oper, ">" ) )
				sp->opr = TOK_OPR_GREATER;
			else
				VMASSERT2( 0, EC_1023_CONDERR, _t );

			sp->scope[ 0 ] = -1;
			sp->scope[ 1 ] = -1;
			sp->scope[ 2 ] = -1;

			scope_level.add( TOK_IF );
		}
		else if ( TCMP( token, "ELSE" ) )
		{
			VMITEM* vmi = make_item( TOK_ELSE, token, -1, I_OP, debug_line );
			SPEC_ELSE* sp = new SPEC_ELSE;
			sp->next = -1;

			vmi->spec = sp;
		}
		else if ( TCMP( token, "WHILE" ) )
		{// WHILE <condition>;  [BREAK;] ENDWHILE;
		 // <condition> can not have expression.
			ti++;
			char* opr1 = vm_tok_list[ ti ]->tok; // operand A
			ti++;
			char* oper = vm_tok_list[ ti ]->tok; // operator 
			ti++;
			char* opr2 = vm_tok_list[ ti ]->tok; // operand B

			VMITEM* _t = make_item( TOK_WHILE, token, -1, I_OP, debug_line );
			SPEC_WHILE* sp = new SPEC_WHILE;
			sp->opr		= TOK_NONE;
			sp->opr1	= NULL;
			sp->opr2	= NULL;
			sp->scope[0] = sp->scope[1] = 0;

			_t->spec = sp;

			sp->opr1 = process_unknown_token( ptoken, ti, opr1, NULL, debug_line );
			sp->opr2 = process_unknown_token( ptoken, ti, opr2, NULL, debug_line );
			VMASSERT2( sp->opr1 && sp->opr2, EC_1024_WERR, _t );

			sp->opr1->rtf = (TOKTYPE)NULL;
			sp->opr2->rtf = (TOKTYPE)NULL;

			if ( TCMP( oper, "==" ) )
				sp->opr = TOK_OPR_SAME;
			else if ( TCMP( oper, "!=" ) )
				sp->opr = TOK_OPR_DIFF;
			else if ( TCMP( oper, "<=" ) )
				sp->opr = TOK_OPR_LEQUAL;
			else if ( TCMP( oper, ">=" ) )
				sp->opr = TOK_OPR_GEQUAL;
			else if ( TCMP( oper, "<" ) )
				sp->opr = TOK_OPR_LESS;
			else if ( TCMP( oper, ">" ) )
				sp->opr = TOK_OPR_GREATER;
			else
				VMASSERT2( 0, EC_1023_CONDERR, _t );

			sp->scope[ 0 ] = -1;
			sp->scope[ 1 ] = -1;

			scope_level.add( TOK_WHILE );
		}
		else if ( TCMP( token, "BREAK" ) )
		{	
			make_item( TOK_BREAK, token, -1, I_OP, debug_line );
		}
		else if ( TCMP( token, "=" ) )
		{
			VMITEM* vi = make_item( TOK_OPR_EQUAL, token, -1, I_OP, debug_line );
			vi->pri = 0; // the lowest priority
		}
		else if ( TCMP( token, "GOTO" ) )
		{
			VMITEM* _t = make_item( TOK_GOTO, token, -1, I_OP, debug_line );

			ti++;
			token = vm_tok_list[ ti ]->tok;

			SPEC_GOTO* sp = new SPEC_GOTO;
			sys_str::copy( sp->name, 256, token );

			sp->pos = -1;

			_t->spec = sp;
		}
		else if ( TCMP( token, "@" ) )
		{
			VMITEM* _t = make_item( TOK_LABEL, token, -1, I_OP, debug_line ); // the OP will be turned off when processing 'scope'
			_t->cls.clinkable = 0; // not linkable

			ti++;
			char* token = vm_tok_list[ ti ]->tok;

			SPEC_LABEL* sp = new SPEC_LABEL;
			sys_str::copy( sp->name, 256, token );
			sp->pos = -1;

			labellist.add( sp );

			_t->spec = sp;
		}
		else if ( TCMP( token, "SIN" ) )
		{
			VMITEM* _t = make_item( TOK_MATH_SIN, token, -1, I_OP, debug_line );
			// *** The Functions such as SIN, COS, or SQRT etc. that requires the input for them,
			// *** the operand may be calculated on run-time like the arithmetic operators.
			// *** The functions does not have to hold a SPEC object pointing the runtime-calculate operands.
			// *** DEAL WITH THE FUNCTIONS AS AN ARITHMETIC OPERATOR.
			// *** The SPEC object is used for the parameter which will not be calculated.
			// ***
			// *** FUNCTIONS DOES NOT PROCESS ';' 
			// *** ';' MUST LEAVE AS IT IS; TO MAKE A SIGNAL OF STATEMENT.
		}
		else if ( TCMP( token, "COS" ) )
		{
			VMITEM* _t = make_item( TOK_MATH_COS, token, -1, I_OP, debug_line );
		}
		else if ( TCMP( token, "SQRT" ) )
		{
			VMITEM* _t = make_item( TOK_MATH_SQRT, token, -1, I_OP, debug_line );
		}
		else if ( TCMP( token, "EXP" ) )
		{
			VMITEM* _t = make_item( TOK_MATH_EXP, token, -1, I_OP, debug_line );
		}
		else if ( TCMP( token, "SLEEP" ) )
		{// SLEEP <VAR|CONST>
			VMITEM* _t = make_item( TOK_SLEEP, token, -1, I_OP, debug_line );

			ti++;
			char* opr = vm_tok_list[ ti ]->tok; // operand A

			SPEC_SLEEP* sp = new SPEC_SLEEP;
			sp->opr = NULL;

			sp->opr = process_unknown_token( ptoken, ti, opr, NULL, debug_line );
			VMASSERT2( sp->opr, EC_1025_EOSLP, _t );

			sp->opr->rtf = (TOKTYPE)NULL;

			_t->spec = sp;
		}
		else if ( TCMP( token, "CALL" ) )
		{// CALL cannot invoke "format" instance.
			// CALL <name> [args...]
			VMITEM* _t = make_item( TOK_CALL, token, -1, I_OP, debug_line );

			ti++;
			token = vm_tok_list[ ti ]->tok;		// function name

			SPEC_CALL* sp = new SPEC_CALL;
		
			sys_str::copy( sp->name, 256, token );
			sp->pos				= -1;
			sp->proc			= NULL;
			sp->b_exist_byref	= FALSE;

			// add parameter list--------------------------------------
			while ( 1 )
			{
				ti++;
				token = vm_tok_list[ ti ]->tok;

				if ( TCMP( token, ";" ) )
					break;

				if ( TCMP( token, "&" ) )
				{
					sp->params_byref.add( 1 );
					sp->b_exist_byref = TRUE;

					ti++;
					token = vm_tok_list[ ti ]->tok;
				}
				else
				{
					sp->params_byref.add( 0 );
				}


				VMITEM* vmi = process_keyword_token( ti, debug_line );

				if ( NULL == vmi )
				{
					vmi = process_unknown_token( ptoken, ti, token, NULL, debug_line );
					if ( FALSE == b_in_array_brace_at_unknown )
					{
						vmi->b_op = TRUE;          // its for UNARY operator process.  
						sp->params.add( vmi );
					}
				}
			}

			ti--;

			_t->spec = sp;
		}
		else if ( TCMP( token, "EVENT" ) )
		{ // EVENT <ID> <PARAM_VAL>
			VMITEM* _t = make_item( TOK_EVENT, token, -1, I_OP, debug_line );

			SPEC_EVENT* sp = new SPEC_EVENT;
			sp->funcid	= NULL;
			sp->param	= NULL;

			VMINT utype = 0;

			ti++;
			token = vm_tok_list[ ti ]->tok;

			sp->funcid = process_unknown_token( ptoken, ti, token, &utype, debug_line );
			VMASSERT2( sp->funcid && TOK_VAR_STR != utype && TOK_CONST_STR != utype, EC_1026_EOOC, _t );
			sp->funcid->rtf = (TOKTYPE)NULL;

			ti++;
			token = vm_tok_list[ ti ]->tok;

			sp->param = process_unknown_token( ptoken, ti, token, &utype, debug_line );
			VMASSERT2( sp->param && TOK_VAR_STR != utype && TOK_CONST_STR != utype, EC_1026_EOOC, _t );
			sp->param->rtf = (TOKTYPE)NULL;

			if ( NULL == _vm_callback )
				_t->rtf = (TOKTYPE)NULL;			// if there's no callee, don't process the outcall(event).

			_t->spec = sp;
		}
		else if ( TCMP( token, "$" ) )
		{// CALLPROC cannot invoke "format" instance.
			// CALLPROC <prchnd> [args...]
			VMITEM* _t = make_item( TOK_CALLPROC, token, -1, I_OP, debug_line );

			SPEC_CALLPROC* sp = new SPEC_CALLPROC;
			sp->prchnd	= NULL;

			ti++;
			char* opr1 = vm_tok_list[ ti ]->tok; // operand A
			VMASSERT2( 0 == get_text_type( opr1 ), EC_0025_ECPRC, _t ); // must be a string.
			VMINT utype = 0;
			sp->prchnd = process_unknown_token( ptoken, ti, opr1, &utype, debug_line );
			VMASSERT2( sp->prchnd && TOK_CONST_INT != utype && TOK_CONST_DOUBLE != utype && TOK_CONST_STR != utype, EC_0025_ECPRC, _t );

			// add parameter list--------------------------------------
			while ( 1 )
			{
				ti++;
				token = vm_tok_list[ ti ]->tok;

				if ( TCMP( token, ";" ) )
					break;

				if ( TCMP( token, "&" ) )
				{
//					sp->params_byref.add( 1 );
//					sp->b_exist_byref = TRUE;

					ti++;
					token = vm_tok_list[ ti ]->tok;
				}
				else
				{
//					sp->params_byref.add( 0 );
				}

				VMITEM* vmi = process_keyword_token( ti, debug_line );

				if ( NULL == vmi )
				{
					vmi = process_unknown_token( ptoken, ti, token, NULL, debug_line );
					if ( FALSE == b_in_array_brace_at_unknown )
					{
						vmi->b_op = TRUE;          // its for UNARY operator process.  
						sp->params.add( vmi );
					}
				}
			}

			ti--;

			_t->spec = sp;
		}
		else if ( TCMP( token, "FUNCTION" ) )
		{
			// PROC <name>
			VMITEM* _t = make_item( TOK_PROC, token, -1, I_OP, debug_line );  // the OP will be turned off when processing 'scope'

			ti++;
			token = vm_tok_list[ ti ]->tok;

			SPEC_PROC* sp = new SPEC_PROC;
			sp->scope[0] = sp->scope[1] = 0;
			sp->endproc  = 0;
			sys_str::copy( sp->name, 256, token );

			proclist.add( sp );

			// add parameter list--------------------------------------
			while ( 1 )
			{
				ti++;
				token = vm_tok_list[ ti ]->tok;

				if ( TCMP( token, "{" ) )
					break;

				VMITEM* vmi = process_unknown_token( ptoken, ti, token, NULL, debug_line );
				sp->params.add( vmi );
			}

			ti--;

			_t->spec = sp;

			scope_level.add( TOK_PROC );
		}
		else if ( TCMP( token, "END" ) )
		{
			make_item( TOK_END, token, -1, I_OP, debug_line );
		}
		else if ( TCMP( token, "{" ) )
		{
			scope_level_counter++;
			make_item( TOK_STATEMENT, ";", -1, I_NOP, debug_line );
		}
		else if ( TCMP( token, "}" ) )
		{
			scope_level_counter--;

			TOKTYPE type = scope_level.pop();

			switch( type )
			{
			case TOK_WHILE:
				{
					VMITEM* vmi = make_item( TOK_ENDWHILE, token, -1, I_OP, debug_line );
					SPEC_ENDWHILE* sp = new SPEC_ENDWHILE;
					sp->next = -1;
					vmi->spec = sp;
				}
			case TOK_IF:
				{// A mark for scope examination:
					// Since the scope examination is processed after the runtime code production,
					// a keyword like "ENDIF" which does nothing(NOP) should be placed as if it were an OP.
					// So, "ENDIF" is a dummy OP; 
					//-----------------------------------------------------------------------------------
					// 1. A scope can be derived from runtime code only.
					// 2. A runtime code only includes OPs.
					// 3. So, scope marks should be OP.
					make_item( TOK_ENDIF, token, -1, I_OP, debug_line );
				}
				break;
			case TOK_PROC:
				{
					// ENDPROC 
					VMITEM* _t = make_item( TOK_ENDPROC, token, -1, I_OP, debug_line );
					SPEC_ENDPROC* sp = new SPEC_ENDPROC;
					sp->call	= NULL;
					sp->proc	= NULL;
					_t->spec = sp;
				}
				break;
			}

			make_item( TOK_STATEMENT, ";", -1, I_NOP, debug_line );
		}
		else
		{
			process_unknown_token( "", ti, token, NULL, debug_line );
		}

		ti++;
	}


	// correction for unary operators.
	for (VMINT k = 0; k<vm_items.num-1; k++)
	{
		VMITEM* vmi = vm_items[ k ];

		if ( TOK_OPR_SUBTRACT == vmi->type )
		{
			if ( vm_items[ k - 1 ]->b_op || 
				TOK_PRI_INC == vm_items[ k - 1 ]->type || TOK_PRI_DEC == vm_items[ k - 1 ]->type ||
				TOK_ARRAY_OPEN == vm_items[ k - 1 ]->type || TOK_ARRAY_CLOSE == vm_items[ k - 1 ]->type )
			{
				vmi->rtf	= (TOKTYPE)NULL;
				vmi->type	= TOK_OPR_SUBTRACT_UNARY;
			}
		}
	}
}
