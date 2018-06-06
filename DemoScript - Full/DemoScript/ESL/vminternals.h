
#ifndef _IGVMINTERNALS_H
#define _IGVMINTERNALS_H


// IGVM SYSTEM PRECISIONS--------------------------------------------------
#define VMBYTE		unsigned char
#define VMSHORT		short int
#define VMWORD		unsigned short int
#define VMINT		int
#define VMDWORD		unsigned int
#define VMINT64		__int64
#define VMFLOAT		float
#define VMDOUBLE	double
// IGVM SYSTEM PRECISIONS--------------------------------------------------


struct FORMATITEM;
struct FORMAT_ELE;
struct VMITEM;

enum TOKTYPE
{
	TOK_NONE	= 0,
	TOK_PRI_INC = 1,
	TOK_PRI_DEC,

	TOK_ARRAY_OPEN,
	TOK_ARRAY_CLOSE,
	TOK_PERIOD,
	TOK_STATEMENT,

	TOK_OPR_SUBTRACT_UNARY,
	TOK_OPR_ADD,
	TOK_OPR_ADDADD,
	TOK_OPR_SUBTRACT,
	TOK_OPR_SUBSUB,
	TOK_OPR_MULTIPLY,
	TOK_OPR_DIVIDE,
	TOK_OPR_MOD,

	TOK_OPR_ADD_EQUAL,
	TOK_OPR_SUBTRACT_EQUAL,
	TOK_OPR_MULTIPLY_EQUAL,
	TOK_OPR_DIVIDE_EQUAL,
	TOK_OPR_MOD_EQUAL,

	TOK_OPR_EQUAL,			// =

	TOK_OPR_SAME,		// ==	same (coincide)
	TOK_OPR_DIFF,		// !=	different
	TOK_OPR_LEQUAL,		// <=	less or equal
	TOK_OPR_GEQUAL,		// >=	greater or equal
	TOK_OPR_LESS,		// <	less
	TOK_OPR_GREATER,	// >	greater

	TOK_CONST_INT,
	TOK_CONST_DOUBLE,
	TOK_CONST_STR,

	TOK_VAR_BYTE,
	TOK_VAR_WORD,
	TOK_VAR_DWORD,
	TOK_VAR_SHORT,
	TOK_VAR_INT,			// integer(4)
	TOK_VAR_INT64,			// integer(8)
	TOK_VAR_FLOAT,			// float precision
	TOK_VAR_DOUBLE,			// double precision
	TOK_VAR_STR,			// characters(256)
	TOK_VAR_FORMAT,

	TOK_IF,
	TOK_ELSE,
	TOK_ENDIF,

	TOK_WHILE,
	TOK_ENDWHILE,
	TOK_BREAK,

	TOK_CALL,
	TOK_CALLPROC,
	TOK_PROC,
	TOK_ENDPROC,

	TOK_PUSH,
	TOK_POP,
	TOK_PEEK,
	TOK_POPAWAY,
	TOK_READSTACK,
	TOK_RETURN,

	TOK_LOADMODULE,
	TOK_UNLOADMODULE,
	TOK_GETPROC,
	TOK_GETRET,
	TOK_GETHOSTRET,

	TOK_GETPORTSIZE,
	TOK_MAPPORT,
	TOK_PUTPORT,
	TOK_READPORT,
	TOK_GETOFFSET,

	TOK_ECHO,
	TOK_ECHOLN,
	TOK_MSG,

	TOK_GOTO,
	TOK_LABEL,
	TOK_END,

	TOK_SLEEP,
	TOK_EVENT,

	TOK_MATH_EXP,
	TOK_MATH_SQRT,
	TOK_MATH_COS,
	TOK_MATH_SIN,

	TOK_TOK_TOK
};


#define I_OP		TRUE
#define	I_NOP		FALSE


struct VMCLASSITEM
{
	VMWORD	cls:3;			// 8 classes
	VMWORD	cconst:1;		// 0: variable, 1: constant
	VMWORD	carrayidx:1;	// 0: variable, 1: array idx
	VMWORD	carray:1;		// 0: variable, 1: array
	VMWORD	ckeyword:1;		// 0: variable, 1: keyword
	VMWORD	clinkable:1;	// 0: not-linkable, 1: linkable
	VMWORD cformat:1;		// 0: not-format, 1: format
};

enum VMCLASS
{
	VMCLS_N = 0,	// nothing
	VMCLS_I = 1,	// integer class  ( byte, short, int, const-int )
	VMCLS_F,		// float class	( float, double )
	VMCLS_S,		// string class
	VMCLS_O,		// other class
};

enum SPEC_TYPE
{
	SPEC_T_PROC = 1,
	SPEC_T_ENDPROC,
	SPEC_T_IF,
	SPEC_T_CALL,
	SPEC_T_CALLPROC,
	SPEC_T_GOTO,
	SPEC_T_ELSE,
	SPEC_T_SQRT,
	SPEC_T_SIN,
	SPEC_T_COS,
	SPEC_T_LABEL,
	SPEC_T_PUSH,
	SPEC_T_POP,
	SPEC_T_PEEK,
	SPEC_T_LOADMODULE,
	SPEC_T_UNLOADMODULE,
	SPEC_T_GETPROC,
	SPEC_T_GETRET,
	SPEC_T_GETHOSTRET,
	SPEC_T_READSTACK,
	SPEC_T_POPAWAY,
	SPEC_T_SLEEP,
	SPEC_T_EVENT,
	SPEC_T_GETPORTSIZE,
	SPEC_T_MAPPORT,
	SPEC_T_PUTPORT,
	SPEC_T_READPORT,
	SPEC_T_WHILE,
	SPEC_T_ENDWHILE,
	SPEC_T_GETOFFSET,
	SPEC_T_PERIOD,
	SPEC_T_ARRAY_OPEN,
};

struct SPEC_ROOT
{
	SPEC_ROOT( SPEC_TYPE t )
	{ 
		type = t;
	}
protected:
	SPEC_TYPE	type;
};


struct VMTOKEN
{
	char		tok[256];
	VMINT		debug_line;
	VMTOKEN()
	{
		tok[0]	= NULL;
	}
};


struct VMITEM
{
	TOKTYPE			type;			// item type
	char			name[256];
	VMINT			sid;			// symbolic id
	ALLVALUE		initval;
	VMINT			pri;			// priority
	BOOL			b_op;			// is an operator?
	SPEC_ROOT*		spec;
	VMITEM*			left;
	VMITEM*			right;
	VMITEM*			left_op;
	VMITEM*			right_op;
	VMITEM*			right_array;
	BOOL			referred;
	VMINT			pos;
	VMINT			cpos;
	VMINT			debug_line;
	VMCLASSITEM		cls;
	TOKTYPE		rtf;

public:
	VMITEM()
	{
		memset( this, 0, sizeof(VMITEM) );
	}
};


struct SPEC_ENDPROC;
struct SPEC_PROC : public SPEC_ROOT
{
	SPEC_PROC() : SPEC_ROOT(SPEC_T_PROC) {}
	char				name[256];
	VMINT				scope[2];
	sys_list<VMITEM*>	params;
	SPEC_ENDPROC*		endproc;
};

struct SPEC_IF : public SPEC_ROOT
{
	SPEC_IF() : SPEC_ROOT(SPEC_T_IF) {}
	VMITEM*				opr1;
	TOKTYPE				opr;
	VMITEM*				opr2;
	VMINT				scope[3];		// begin, else, end
};

struct SPEC_ELSE : public SPEC_ROOT
{
	SPEC_ELSE() : SPEC_ROOT(SPEC_T_ELSE) {}
	VMINT				next;
};

struct SPEC_WHILE : public SPEC_ROOT
{
	SPEC_WHILE() : SPEC_ROOT(SPEC_T_WHILE) {}
	VMITEM*				opr1;
	TOKTYPE				opr;
	VMITEM*				opr2;
	VMINT				scope[2];		// begin, end
};

struct SPEC_ENDWHILE : public SPEC_ROOT
{
	SPEC_ENDWHILE() : SPEC_ROOT(SPEC_T_ENDWHILE) {}
	VMINT				next;
};

struct SPEC_CALL : public SPEC_ROOT
{
	SPEC_CALL() : SPEC_ROOT(SPEC_T_CALL) {}
	VMINT				pos;			// proc's begin pos
	char				name[256];
	sys_list<int>		params_byref;	// call by reference
	sys_list<VMITEM*>	params;
	SPEC_PROC*			proc;
	BOOL				b_exist_byref;
};

struct SPEC_CALLPROC : public SPEC_ROOT
{
	SPEC_CALLPROC() : SPEC_ROOT(SPEC_T_CALLPROC) {}
	sys_list<VMITEM*>	params;
	VMITEM*				prchnd;
};

struct SPEC_ENDPROC : public SPEC_ROOT
{
	SPEC_ENDPROC() : SPEC_ROOT(SPEC_T_ENDPROC) {}
	SPEC_CALL*			call;
	SPEC_PROC*			proc;
};

struct SPEC_GOTO : public SPEC_ROOT
{
	SPEC_GOTO() : SPEC_ROOT(SPEC_T_GOTO) {}
	VMINT				pos;
	char				name[256];
};

struct SPEC_LABEL : public SPEC_ROOT
{
	SPEC_LABEL() : SPEC_ROOT(SPEC_T_LABEL) {}
	char		name[256];
	VMINT		pos;
};

struct SPEC_PUSH : public SPEC_ROOT
{
	SPEC_PUSH() : SPEC_ROOT(SPEC_T_PUSH) {}
	VMITEM*		opr;
};

struct SPEC_POP : public SPEC_ROOT
{
	SPEC_POP() : SPEC_ROOT(SPEC_T_POP) {}
	VMITEM*		opr;
};

struct SPEC_PEEK : public SPEC_ROOT
{
	SPEC_PEEK() : SPEC_ROOT(SPEC_T_PEEK) {}
	VMITEM*		opr;
};

struct SPEC_LOADMODULE : public SPEC_ROOT
{
	SPEC_LOADMODULE() : SPEC_ROOT(SPEC_T_LOADMODULE) {}
	char		modname[256];
	VMITEM*		hnd;
};

struct SPEC_UNLOADMODULE : public SPEC_ROOT
{
	SPEC_UNLOADMODULE () : SPEC_ROOT(SPEC_T_UNLOADMODULE) {}
	VMITEM*		hnd;
};

struct SPEC_GETRET : public SPEC_ROOT
{
	SPEC_GETRET () : SPEC_ROOT(SPEC_T_GETRET) {}
	VMITEM*		ret;
};

struct SPEC_GETHOSTRET : public SPEC_ROOT
{
	SPEC_GETHOSTRET () : SPEC_ROOT(SPEC_T_GETHOSTRET) {}
	VMITEM*		ret;
};

struct SPEC_GETPROC : public SPEC_ROOT
{
	SPEC_GETPROC() : SPEC_ROOT(SPEC_T_GETPROC) {}
	char		prcname[256];
	VMITEM*		modhnd;
	VMITEM*		hnd;
};

struct SPEC_READSTACK : public SPEC_ROOT
{
	SPEC_READSTACK() : SPEC_ROOT(SPEC_T_READSTACK) {}
	int			level;
	VMITEM*		opr;
};

struct SPEC_POPAWAY : public SPEC_ROOT
{
	SPEC_POPAWAY() : SPEC_ROOT(SPEC_T_POPAWAY) {}
	int			level;
};

struct SPEC_PERIOD : public SPEC_ROOT
{
	SPEC_PERIOD() : SPEC_ROOT(SPEC_T_PERIOD) {}
	FORMAT_ELE*		fe;
};

struct SPEC_ARRAY_OPEN : public SPEC_ROOT
{
	SPEC_ARRAY_OPEN() : SPEC_ROOT(SPEC_T_ARRAY_OPEN) {}
	VMINT					result_idx;
	sys_list<VMITEM*>		expr;
};

struct SPEC_GETPORTSIZE : public SPEC_ROOT
{
	SPEC_GETPORTSIZE() : SPEC_ROOT(SPEC_T_GETPORTSIZE) {}
	VMITEM*		opr1;	// port number
	VMITEM*		opr2;	// VMINT var
};

struct SPEC_GETOFFSET : public SPEC_ROOT
{
	SPEC_GETOFFSET() : SPEC_ROOT(SPEC_T_GETOFFSET) {}
	VMITEM*			opr2;	// VMINT var
	int				offset;
};

struct SPEC_MAPPORT : public SPEC_ROOT
{
	SPEC_MAPPORT() : SPEC_ROOT(SPEC_T_MAPPORT) {}
	VMITEM*		opr1;	// port number
	VMITEM*		opr2;	// fmt var
};

struct SPEC_PUTPORT : public SPEC_ROOT
{
	SPEC_PUTPORT() : SPEC_ROOT(SPEC_T_PUTPORT) {}
	VMITEM*		opr1;	// port number
	VMITEM*		opr2;	// fmt var
};

struct SPEC_READPORT : public SPEC_ROOT
{
	SPEC_READPORT() : SPEC_ROOT(SPEC_T_READPORT) {}
	VMITEM*		opr1;	// port number
	VMITEM*		opr2;	// fmt var
	VMITEM*		opr3;	// var
};

struct SPEC_SLEEP : public SPEC_ROOT
{
	SPEC_SLEEP() : SPEC_ROOT(SPEC_T_SLEEP) {}
	VMITEM*		opr;
};

struct SPEC_EVENT: public SPEC_ROOT
{
	SPEC_EVENT() : SPEC_ROOT(SPEC_T_EVENT) {}
	VMITEM*		funcid;
	VMITEM*		param;
};


struct VARITEM
{
	char		name[256];
	ALLVALUE	val;
	VMINT		repeat;			// array size
	TOKTYPE		tok_type;
	void*		realaddr;
	void*		bind;
	VMCLASSITEM	cls;
};

struct DEFITEM
{
	char		name[256];
	char		substitution[256];
};

struct VHANDLER
{
	char	open_keyword[64];
	char	close_keyword[64];
	void (_stdcall *handler)(const char* text);
};

struct PORTITEM
{
	VMINT				size;
	unsigned char*	ptr;
};

enum FORMAT_ELE_TYPE
{
	FMT_INT = 1,
	FMT_INT64,
	FMT_BYTE,
	FMT_SHORT,
	FMT_WORD,
	FMT_DWORD,
	FMT_FLOAT,
	FMT_DOUBLE,
	FMT_STR,
	FMT_FORMAT,
};


struct FORMAT_ELE
{
	FORMAT_ELE_TYPE			ftype;
	int						size;
	int						offset;
	int						repeat;
	char					name[64];
	FORMATITEM*				link;
};

struct FORMATITEM
{
	char					name[256];
	sys_list<FORMAT_ELE*>	list;
	int						size;
};


struct STATEMENTITEM
{
	VMINT		idx;
	BOOL		processed;
};

struct CALLSTACKITEM
{
	int			ip;
	VMITEM*		call;
};

struct BINDCALLITEM
{
	int (_stdcall *bindfunc)();
	char varname[256];
};


#define PEEKNEXT( cur )		((cur < vm_tok_list.num-1) ? vm_tok_list[ cur+1 ]->tok : NULL)
#define TCMP( a, b )		sys_str::icompare( a, b )
#define VMASSERT( test, text )	if ( !(test) ) \
				{ \
				char msg[256];	sprintf( msg, "%s (%s:%d)\nExpr: %s", text, __FILE__, __LINE__, #test );\
					MessageBoxA( 0, msg, "Citrus", MB_ICONWARNING ); exit(-1); }
#define VMASSERT1( test, fmttxt, var )	if ( !(test) ) \
				{ \
				char fmt[256];  sprintf( fmt, "%s (%%s:%%d)\nExpr: %%s", fmttxt );\
				char msg[256];	sprintf( msg, fmt, var, __FILE__, __LINE__, #test );\
					MessageBoxA( 0, msg, "Citrus", MB_ICONWARNING ); exit(-1); }
#define VMASSERT2( test, text, vmitem )	if ( !(test) ) \
				{ \
				char msg[256];	sprintf( msg, "%s\n\nSCRIPT: \"%s\" LINE: %d\n\nSRC: %s LINE: %d\nExpr: %s",\
										text, vmitem->name, vmitem->debug_line,	__FILE__, __LINE__, #test );\
					MessageBoxA( 0, msg, "Citrus", MB_ICONWARNING ); exit(-1); }

#endif
