/*
	Citrus Embedded Script Language

	Copyright by 2007-2011 HaangilSoft. All rights reserved.
	Jinhong Kim <Redperf@gmail.com>
*/

#ifndef _ESL_H_
#define _ESL_H_

/* Include files **************************************************************/
#include "syslib/tokenizer.h"
#include "syslib/sysstr.h"
#include "syslib/allvalue.h"
#include "syslib/datastream.h"
#include "vminternals.h"
#include "eclist.h"
#include "math.h"

/* #define Directive **********************************************************/

/* Reference class ************************************************************/

/* Class declaration **********************************************************/

class CCitrus
{
public:
	CCitrus();
	~CCitrus();

	int			getfunc( char* funcname );
	int			call( int start_ip = 0, int bclearstack = 1);

	void		init( char* script, int b_bin, void (_stdcall *usercb)(int, int), char* entryname );
	void		uninit();

	void		clearstack();
	void		push_i( int v );
	void		push_f( double v );
	void		push_s( char* v );
	int			pop_i();
	double		pop_f();
	void		pop_s( char* str );

	int			get_i( const char* varname );
	double		get_f( const char* varname );
	const char*	get_s( const char* varname );

	void		bind_i( int* var, const char* varname );
	void		bind_f( double* var, const char* varname );
	void		bind_str( const char* var, const char* varname );
	void		bind_function( int (_stdcall *_bindfunc)(), const char* varname );

	void		set_verbatim_handler( const char* open_keyword, const char* close_keyword, 
				void (_stdcall *vhandler)(const char* text) );

	void		init_block_port( int n_port );
	void		uninit_block_port();
	void		register_block( int port, void* voidptr, int size );

	void		export_bin( char* pathname );
	void		import_bin( char* pathname );

protected:
	sys_list<FORMATITEM*>	vm_format_list;
	sys_list<VMTOKEN*>		vm_tok_list;
	sys_list<VMITEM*>		vm_items;
	sys_list<CALLSTACKITEM*> vm_callstack;
	sys_list<ALLVALUE*>		vm_value_stack;
	sys_list<VMDOUBLE>		vm_calc_stack;

	sys_list<VARITEM*>		varlist;
	sys_list<SPEC_ROOT*>		labellist;
	sys_list<SPEC_ROOT*>		proclist;

	sys_list<VMITEM*>			vm_code;
	sys_list<VHANDLER*>		vm_vhandler;
	sys_list<PORTITEM*>		vm_blocks;

	sys_list<VMINT>			vm_nested_stack;
	sys_list<BINDCALLITEM*>	vm_bindcall_list;

	VMDOUBLE				vm_r0;
	VMINT					vm_datapack;
	VMINT					vm_exit_code;
	VMINT					vm_procret;
	char					vm_script_path[256];
	char					vm_entry_name[256];
	BOOL					b_in_array_brace_at_unknown;

	typedef VMINT (CCitrus::*RUNTIMEFUNC)( VMITEM* code );
	RUNTIMEFUNC	RTFTBL[ TOK_TOK_TOK ];

	static void (_stdcall *_vm_callback)(VMINT, VMINT);


protected:
	VMINT I_ADDADD( VMITEM* code );
	VMINT I_ADD( VMITEM* code );
	VMINT I_SUBSUB( VMITEM* code );
	VMINT I_SUB( VMITEM* code );
	VMINT I_MUL( VMITEM* code );
	VMINT I_DIV( VMITEM* code );
	VMINT I_MOD( VMITEM* code );
	VMINT I_EQUAL( VMITEM* code );
	VMINT I_ADD_EQUAL( VMITEM* code );
	VMINT I_SUB_EQUAL( VMITEM* code );
	VMINT I_MUL_EQUAL( VMITEM* code );
	VMINT I_DIV_EQUAL( VMITEM* code );
	VMINT I_MOD_EQUAL( VMITEM* code );

	VMINT I_SQRT( VMITEM* code );
	VMINT I_EXP( VMITEM* code );
	VMINT I_COS( VMITEM* code );
	VMINT I_SIN( VMITEM* code );
	VMINT I_SLEEP( VMITEM* code );
	VMINT I_ECHO( VMITEM* code );
	VMINT I_ECHOLN( VMITEM* code );
	VMINT I_MSG( VMITEM* code );
	VMINT I_GOTO( VMITEM* code );
	VMINT I_ELSE( VMITEM* code );
	VMINT I_PUSH( VMITEM* code );
	VMINT I_POP( VMITEM* code );
	VMINT I_PEEK( VMITEM* code );
	VMINT I_READSTACK( VMITEM* code );
	VMINT I_LOADMODULE( VMITEM* code );
	VMINT I_UNLOADMODULE( VMITEM* code );
	VMINT I_GETPROC( VMITEM* code );
	VMINT I_GETHOSTRET( VMITEM* code );
	VMINT I_GETRET( VMITEM* code );
	VMINT I_CALLPROC( VMITEM* code );
	VMINT I_POPAWAY( VMITEM* code );
	VMINT I_RETURN( VMITEM* code );
	VMINT I_GETPORTSIZE( VMITEM* code );
	VMINT I_MAPPORT( VMITEM* code );
	VMINT I_PUTPORT( VMITEM* code );
	VMINT I_GETOFFSET( VMITEM* code );
	VMINT I_READPORT( VMITEM* code );
	VMINT I_IF( VMITEM* code );
	VMINT I_WHILE( VMITEM* code );
	VMINT I_ENDWHILE( VMITEM* code );
	VMINT I_PROC( VMITEM* code );
	VMINT I_CALL( VMITEM* code );
	VMINT I_EVENT( VMITEM* code );
	VMINT I_ENDPROC( VMITEM* code );
	VMINT I_BREAK( VMITEM* code );
	VMINT I_END( VMITEM* code );


	VMITEM* make_item( TOKTYPE type, char* name, VMINT sid, BOOL b_op, VMINT debug_line );
	VMINT make_vm_var_format( VMITEM* t, FORMATITEM* fi, int size, VMINT repeat );
	VMINT make_vm_var_i( VMITEM* t, VMINT initval, VMINT repeat, BOOL b_format_inst = FALSE);
	VMINT make_vm_var_bind_i( const char* varname, int* bindvar, VMINT repeat );
	VMINT make_vm_var_f( VMITEM* t, VMDOUBLE initval, VMINT repeat, BOOL b_format_inst = FALSE);
	VMINT make_vm_var_bind_f( const char* varname, double* bindvar, VMINT repeat );
	VMINT make_vm_var_str( VMITEM* t, char* initval, VMINT repeat, BOOL b_format_inst = FALSE);
	VMINT make_vm_var_bind_str( const char* varname, const char* bindvar, VMINT repeat );
	char* read_vm_var_str( VMITEM* t );
	void write_vm_var_str( VMITEM* t, char* val );
	VMDOUBLE read_vm_var_f( VMITEM* t );
	void write_vm_var_f( VMITEM* t, VMDOUBLE val );
	VMINT read_vm_var_i( VMITEM* t );
	void write_vm_var_i( VMITEM* t, VMINT val );
	int get_array_index( VMITEM* arrayitem, VMITEM* right_array );
	VMDOUBLE read_operand( VMITEM* t );
	char* read_operand_str( VMITEM* t );
	void write_operand( VMITEM* t, VMDOUBLE v );
	void write_operand_str( VMITEM* t, char* v );
	VMINT get_var_sid( char* name );
	VMINT trans_integer( char* nstr, VMINT texttype );
	VMINT get_text_type( char* str );
	void parse_IF_scope( VMINT vm_ip );
	void parse_WHILE_scope( VMINT vm_ip );
	void parse_PROC_scope( VMINT vm_ip );
	void push_value( VMITEM* vmi );
	void pop_value( VMITEM* vmi );
	void peek_value( VMITEM* vmi );
	void peek_value_from( VMITEM* vmi, int level );
	void init_token_options( Tokenizer& tok );
	void vm_make_tokens( char* pathname, sys_list<DEFITEM*>* deflist_parent );
	int vm_alignment( int size, int repeat );
	int calc_format_element_offsets( FORMATITEM* fi, VMINT debug_line, int* totaloffset );
	FORMATITEM*	search_format_item( char* testname );
	VMITEM* process_unknown_token( char* parenttok, VMINT& ti, char* token, VMINT* type_out, VMINT debug_line );
	VMITEM* find_left_operand( int idx, sys_list<STATEMENTITEM*>& statement, BOOL b_array_open );
	VMITEM* find_left_operator( int idx, sys_list<STATEMENTITEM*>& statement, BOOL b_array_open );
	VMITEM* find_right_operand( int idx, sys_list<STATEMENTITEM*>& statement, BOOL b_array_open );
	VMITEM* find_right_operator( int idx, sys_list<STATEMENTITEM*>& statement, BOOL b_array_open );
	VMITEM* find_right_array( int idx, sys_list<STATEMENTITEM*>& statement );
	void sort_statement_by_priority( sys_list<STATEMENTITEM*>& statement, sys_list<VMITEM*>& output, BOOL b_filterout_arrayidx );
	void adjust_operand_links( sys_list<VMITEM*>& sorteditems );
	void vm_pass_0();
	void vm_pass_1();
	VMITEM* process_keyword_token( int &ti, int debug_line );
	VMDOUBLE vm_evaluate_expression( sys_list<VMITEM*>& rtcodes );
	void make_entry_call( char* entryname );
	void _register_bind_function();
};

#endif // _ESL_H_
