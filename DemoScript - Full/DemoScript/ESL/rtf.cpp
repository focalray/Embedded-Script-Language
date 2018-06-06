
#include "esl.h"

VMINT CCitrus::I_SQRT( VMITEM* code )
{
	VMITEM* t_r = code->right;

	VMDOUBLE rv = 0.0;

	if ( vm_calc_stack.num )
		rv = vm_calc_stack.pop();
	else
		rv = read_operand( t_r );

	vm_calc_stack.add( sqrt( rv ) );

	return 0;
}

VMINT CCitrus::I_EXP( VMITEM* code )
{
	VMITEM* t_r = code->right;

	VMDOUBLE rv = 0.0;

	if ( vm_calc_stack.num )
		rv = vm_calc_stack.pop();
	else
		rv = read_operand( t_r );

	vm_calc_stack.add( exp( rv ) );

	return 0;
}

VMINT CCitrus::I_COS( VMITEM* code )
{
	VMITEM* t_r = code->right;

	VMDOUBLE rv = 0.0;

	if ( vm_calc_stack.num )
		rv = vm_calc_stack.pop();
	else
		rv = read_operand( t_r );

	vm_calc_stack.add( cos( rv ) );

	return 0;
}

VMINT CCitrus::I_SIN( VMITEM* code )
{
	VMITEM* t_r = code->right;

	VMDOUBLE rv = 0.0;

	if ( vm_calc_stack.num )
		rv = vm_calc_stack.pop();
	else
		rv = read_operand( t_r );

	vm_calc_stack.add( sin( rv ) );

	return 0;
}

VMINT CCitrus::I_SLEEP( VMITEM* code )
{
	VMITEM* vmi = code;
	VMASSERT2( vmi, EC_1000_VAD, code );
	SPEC_SLEEP* sp = (SPEC_SLEEP*)vmi->spec;

	// read parameter
	VMINT v = (VMINT)read_operand( sp->opr );

	Sleep( v );

	return 0;
}


VMINT CCitrus::I_ECHO( VMITEM* code )
{
	VMITEM* vmi_r = code->right;

	if(NULL == vmi_r)
		return 0;

	if ( VMCLS_S == vmi_r->cls.cls )
	{
		printf( read_operand_str( vmi_r ) );
	}
	else 
	{
		if ( vmi_r->referred )
		{// the operand has been calculated by some operator. Take vm_r0 instead.
			char txt[256];
			sprintf( txt, "%.2f", vm_r0 );
			printf( txt );

			// remove calc stack
			while ( ! vm_calc_stack.isempty() )
				vm_calc_stack.pop();
		}
		else 
		{
			char txt[256];
			sprintf( txt, "%.2f",	read_operand( vmi_r ) );
			printf( txt );
		}
	}

	return 0;
}

VMINT CCitrus::I_ECHOLN( VMITEM* code )
{
	I_ECHO( code );
	printf( "\n" );
	return 0;
}


VMINT CCitrus::I_MSG( VMITEM* code )
{
	VMITEM* vmi_r = code->right;

	if ( TOK_CONST_STR == vmi_r->type )
	{
		MessageBoxA( 0, vmi_r->initval.string, "HBVM", MB_OK );
	}
	else
	{
		if ( vmi_r->referred )
		{// the operand has been calculated by some operator. Take vm_r0 instead.
			char txt[256];
			sprintf( txt, "%.2f", vm_r0 );
			MessageBoxA( 0, txt, "HBVM", MB_OK );
		}
		else 
		{
			char txt[256];
			sprintf( txt, "%.2f",	read_operand( vmi_r ) );
			MessageBoxA( 0, txt, "HBVM", MB_OK );
		}
	}

	return 0;
}

VMINT CCitrus::I_GOTO( VMITEM* code )
{
	SPEC_GOTO* sp = (SPEC_GOTO*)code->spec;
	return sp->pos;							// unconditional jump.
}


VMINT CCitrus::I_ELSE( VMITEM* code )
{
	SPEC_ELSE* sp = (SPEC_ELSE*)code->spec;
	if ( -1 != sp->next )
		return sp->next;

	VMASSERT2( 0, EC_1000_VAD, code );
	return -1;
}

VMINT CCitrus::I_PUSH( VMITEM* code )
{
	push_value( ((SPEC_PUSH*)code->spec)->opr );
	return 0;
}

VMINT CCitrus::I_POP( VMITEM* code )
{
	pop_value( ((SPEC_POP*)code->spec)->opr );
	return 0;
}

VMINT CCitrus::I_PEEK( VMITEM* code )
{
	peek_value( ((SPEC_PEEK*)code->spec)->opr );
	return 0;
}

VMINT CCitrus::I_READSTACK( VMITEM* code )
{
	SPEC_READSTACK* sp = (SPEC_READSTACK*)code->spec;
	peek_value_from( sp->opr, sp->level );
	return 0;
}

VMINT CCitrus::I_LOADMODULE( VMITEM* code )
{
	SPEC_LOADMODULE* sp = (SPEC_LOADMODULE*)code->spec;

#pragma warning(push)
#pragma warning(disable:4311)
	VMINT hnd = (VMINT)LoadLibraryA( sp->modname );
#pragma warning(pop)
	VMASSERT2( hnd, EC_1005_FNF, code );

	write_vm_var_i( sp->hnd, hnd );

	return 0;
}

VMINT CCitrus::I_UNLOADMODULE( VMITEM* code )
{
	SPEC_UNLOADMODULE* sp = (SPEC_UNLOADMODULE*)code->spec;

#pragma warning(push)
#pragma warning(disable:4311 4312)
	HMODULE	hmod = (HMODULE)read_vm_var_i( sp->hnd );
	FreeLibrary( hmod );
#pragma warning(pop)

	return 0;
}

VMINT CCitrus::I_GETPROC( VMITEM* code )
{
	SPEC_GETPROC* sp = (SPEC_GETPROC*)code->spec;

#pragma warning(push)
#pragma warning(disable:4311 4312)
	HMODULE	hmod = (HMODULE)read_vm_var_i( sp->modhnd );
	VMINT hnd = (VMINT)GetProcAddress( hmod, sp->prcname );
#pragma warning(pop)
	VMASSERT2( hnd, EC_1039_UPRC, code );

	write_vm_var_i( sp->hnd, hnd );

	return 0;
}

VMINT CCitrus::I_GETHOSTRET( VMITEM* code )
{
	SPEC_GETRET* sp = (SPEC_GETRET*)code->spec;
	write_vm_var_i( sp->ret, vm_procret );
	return 0;
}

VMINT CCitrus::I_GETRET( VMITEM* code )
{
	SPEC_GETRET* sp = (SPEC_GETRET*)code->spec;
	write_vm_var_f( sp->ret, vm_r0 );
	return 0;
}

VMINT CCitrus::I_CALLPROC( VMITEM* code )
{
	int vm_ip = code->cpos;

	SPEC_CALLPROC* sp_callproc = (SPEC_CALLPROC*)code->spec;

#pragma warning(push)
#pragma warning(disable:4311 4312)

	void* func = (void*)read_vm_var_i( sp_callproc->prchnd );

	VMINT retcode = 0;

	unsigned int saved_esp = 0;
	__asm mov	saved_esp, esp

	// push the parameters.
	for ( VMINT q=sp_callproc->params.num-1; q>=0; q--)
	{
		int vc = sp_callproc->params[ q ]->cls.cls;

		if ( VMCLS_I == vc )
		{
			VMINT v = (VMINT)read_operand( sp_callproc->params[ q ] );
			__asm mov	esi, v
			__asm push	esi
		}
		else if ( VMCLS_F == vc )
		{
			if ( TOK_VAR_FLOAT == sp_callproc->params[ q ]->type )
			{
				VMFLOAT v = (VMFLOAT)read_operand( sp_callproc->params[ q ] );
				__asm sub esp, 4
				__asm fld dword ptr[v]
				__asm fstp dword ptr[esp]
			}
			else 
			{
				VMDOUBLE v = read_operand( sp_callproc->params[ q ] );
				__asm sub esp, 8
				__asm fld qword ptr[v]
				__asm fstp qword ptr[esp]
			}
		}
		else if ( VMCLS_S == vc )
		{
			VMINT v = (VMINT)read_operand_str( sp_callproc->params[ q ] );
			__asm mov	esi, v
			__asm push	esi
		}
	}

	__asm call	func
	__asm mov	retcode, eax
	__asm mov	esp, saved_esp

	vm_procret	= retcode;
	vm_r0		= retcode;

#pragma warning(pop)

	return 0;
}

VMINT CCitrus::I_POPAWAY( VMITEM* code )
{
	SPEC_POPAWAY* sp = (SPEC_POPAWAY*)code->spec;
	for (int k=0; k<sp->level; k++)
		vm_value_stack.pop();
	return 0;
}

VMINT CCitrus::I_RETURN( VMITEM* code )
{
	VMITEM* vmi_r = code->right;

	if ( vmi_r )
	{
		if ( vmi_r->referred )
		{// the operand has been calculated by some operator. Take vm_r0 instead.
			// remove calc stack
			while ( ! vm_calc_stack.isempty() )
				vm_calc_stack.pop();
		}
		else
		{// let the t_r to be t_l
			vm_r0 = read_operand( vmi_r );
		}
	}

	CALLSTACKITEM* csi = vm_callstack.pop();
	int jumpto = csi->ip;
	SPEC_CALL* sp_call = (SPEC_CALL*)csi->call->spec;
	delete csi;

	if ( FALSE == sp_call->b_exist_byref )
		return jumpto; // skip the call-by-ref process.

	if ( sp_call->proc->params.num > 0 && (sp_call->proc->params.num == sp_call->params_byref.num) )
	{
		for (int k=0; k<sp_call->params_byref.num; k++)
		{
			if ( 1 == sp_call->params_byref[ k ] )
			{// it's call-by-ref parameter.
				int vc = sp_call->params[ k ]->cls.cls;

				if ( VMCLS_I == vc )
				{
					VMINT v = (VMINT)read_operand( sp_call->proc->params[ k ] );
					write_operand( sp_call->params[ k ], (VMDOUBLE)v );
				}
				else if ( VMCLS_F == vc )
				{
					VMDOUBLE v = read_operand( sp_call->proc->params[ k ] );
					write_operand( sp_call->params[ k ], v );
				}
			}
		}
	}

	return jumpto;	// return
}

VMINT CCitrus::I_GETPORTSIZE( VMITEM* code )
{
	SPEC_GETPORTSIZE* sp = (SPEC_GETPORTSIZE*)code->spec;

	VMINT port = (VMINT)read_operand( sp->opr1 );
	VMASSERT2( port > -1 && port < vm_blocks.num, EC_1033_WPNUM, code );

	write_operand( sp->opr2, (VMDOUBLE)vm_blocks[ port ]->size );
	return 0;
}


VMINT CCitrus::I_MAPPORT( VMITEM* code )
{
	SPEC_MAPPORT* sp = (SPEC_MAPPORT*)code->spec;

	VMINT port = (VMINT)read_operand( sp->opr1 );
	VMASSERT2( port > -1 && port < vm_blocks.num, EC_1033_WPNUM, code );

	VARITEM* vi = varlist[ sp->opr2->sid ];

	char* fmtname = vi->name;
	VMASSERT2( TOK_VAR_FORMAT == vi->tok_type, EC_1035_WTNAF, code );

	FORMATITEM* fi = (FORMATITEM*)vi->val.void_ptr;

	memcpy( vi->realaddr, vm_blocks[ port ]->ptr, fi->size * vi->repeat );

	return 0;
}


VMINT CCitrus::I_PUTPORT( VMITEM* code )
{
	SPEC_PUTPORT* sp = (SPEC_PUTPORT*)code->spec;

	VMINT port = (VMINT)read_operand( sp->opr1 );
	VMASSERT2( port > -1 && port < vm_blocks.num, EC_1033_WPNUM, code );

	VARITEM* vi = varlist[ sp->opr2->sid ];

	char* fmtname = vi->name;
	VMASSERT2( TOK_VAR_FORMAT == vi->tok_type, EC_1035_WTNAF, code );

	FORMATITEM* fi = (FORMATITEM*)vi->val.void_ptr;

	memcpy( vm_blocks[ port ]->ptr, vi->realaddr, fi->size * vi->repeat );

	return 0;
}

VMINT CCitrus::I_GETOFFSET( VMITEM* code )
{
	SPEC_GETOFFSET* sp = (SPEC_GETOFFSET*)code->spec;

	// normal VMINT-var
	VMASSERT2( VMCLS_I == sp->opr2->cls.cls, EC_1036_MSTINT, code );
	write_operand( sp->opr2, (VMDOUBLE)sp->offset );

	return 0;
}


VMINT CCitrus::I_READPORT( VMITEM* code )
{// READPORT <port> <offset> <output>
	SPEC_READPORT* sp = (SPEC_READPORT*)code->spec;

	VMINT port = (VMINT)read_operand( sp->opr1 );
	VMASSERT2( port > -1 && port < vm_blocks.num, EC_1033_WPNUM, code );

	unsigned char* blk = NULL;

	VMASSERT( TOK_CONST_INT == sp->opr2->type || TOK_VAR_INT == sp->opr2->type, EC_1037_MSTINTVAR );
	VMINT offset = (VMINT)read_operand( sp->opr2 );
	blk = (vm_blocks[ port ]->ptr + offset);

	VARITEM* vi = varlist[ sp->opr3->sid ];

	switch (vi->tok_type)
	{
	case TOK_VAR_INT:
		write_operand( sp->opr3, (VMDOUBLE)*(VMINT*)blk );
		break;
	case TOK_VAR_INT64:
		write_operand( sp->opr3, (VMDOUBLE)*(VMINT64*)blk );
		break;
	case TOK_VAR_WORD:
		write_operand( sp->opr3, (VMDOUBLE)*(VMWORD*)blk );
		break;
	case TOK_VAR_DWORD:
		write_operand( sp->opr3, (VMDOUBLE)*(VMDWORD*)blk );
		break;
	case TOK_VAR_SHORT:
		write_operand( sp->opr3, (VMDOUBLE)*(VMSHORT*)blk );
		break;
	case TOK_VAR_BYTE:
		write_operand( sp->opr3, (VMDOUBLE)*(VMBYTE*)blk );
		break;
	case TOK_VAR_FLOAT:
		write_operand( sp->opr3, (VMDOUBLE)*(VMFLOAT*)blk );
		break;
	case TOK_VAR_DOUBLE:
		write_operand( sp->opr3, *(VMDOUBLE*)blk );
		break;
	case TOK_VAR_STR:
		write_vm_var_str( sp->opr3, (char*)blk );
		break;
	}

	return 0;
}

VMINT CCitrus::I_IF( VMITEM* code )
{
	SPEC_IF* sp = (SPEC_IF*)code->spec;

	VMITEM* p1 = sp->opr1;
	VMITEM* p2 = sp->opr2;

	BOOL b_cond = FALSE;

	if ( TOK_OPR_SAME == sp->opr )
	{
		if ( TOK_VAR_STR == p1->type )
			b_cond = TCMP( p1->initval.string, p2->initval.string );
		else
			b_cond = read_operand( p1 ) == read_operand( p2 );
	}
	else if ( TOK_OPR_DIFF == sp->opr )
	{
		if ( TOK_VAR_STR == p1->type )
			b_cond = !TCMP( p1->initval.string, p2->initval.string );
		else
			b_cond = read_operand( p1 ) != read_operand( p2 );
	}
	else if ( TOK_OPR_LEQUAL == sp->opr )
	{
		VMASSERT2( TOK_VAR_STR != p1->type && TOK_VAR_STR != p2->type, EC_1000_VAD, code );
		b_cond = read_operand( p1 ) <= read_operand( p2 );
	}
	else if ( TOK_OPR_GEQUAL == sp->opr )
	{
		VMASSERT2( TOK_VAR_STR != p1->type && TOK_VAR_STR != p2->type, EC_1000_VAD, code );
		b_cond = read_operand( p1 ) >= read_operand( p2 );
	}
	else if ( TOK_OPR_LESS == sp->opr )
	{
		VMASSERT2( TOK_VAR_STR != p1->type && TOK_VAR_STR != p2->type, EC_1000_VAD, code );
		b_cond = read_operand( p1 ) < read_operand( p2 );
	}
	else if ( TOK_OPR_GREATER == sp->opr )
	{
		VMASSERT2( TOK_VAR_STR != p1->type && TOK_VAR_STR != p2->type, EC_1000_VAD, code );
		b_cond = read_operand( p1 ) > read_operand( p2 );
	}
	else
		VMASSERT2( 0, EC_1000_VAD, code );

	if ( b_cond )
	{
		return sp->scope[0];  // go ahead
	}
	else
	{
		if ( -1 == sp->scope[1] )
			return sp->scope[2]; // unconditional jump to the outer-brace(close)
		else
			return sp->scope[1]; // unconditional jump to the else section.
	}

	VMASSERT2( 0, EC_1000_VAD, code );
	return -1;
}

VMINT CCitrus::I_WHILE( VMITEM* code )
{
	SPEC_WHILE* sp = (SPEC_WHILE*)code->spec;

	VMITEM* p1 = sp->opr1;
	VMITEM* p2 = sp->opr2;

	BOOL b_cond = FALSE;

	if ( TOK_OPR_SAME == sp->opr )
	{
		if ( TOK_VAR_STR == p1->type )
			b_cond = TCMP( p1->initval.string, p2->initval.string );
		else
			b_cond = read_operand( p1 ) == read_operand( p2 );
	}
	else if ( TOK_OPR_DIFF == sp->opr )
	{
		if ( TOK_VAR_STR == p1->type )
			b_cond = !TCMP( p1->initval.string, p2->initval.string );
		else
			b_cond = read_operand( p1 ) != read_operand( p2 );
	}
	else if ( TOK_OPR_LEQUAL == sp->opr )
	{
		VMASSERT2( TOK_VAR_STR != p1->type && TOK_VAR_STR != p2->type, EC_1000_VAD, code );
		b_cond = read_operand( p1 ) <= read_operand( p2 );
	}
	else if ( TOK_OPR_GEQUAL == sp->opr )
	{
		VMASSERT2( TOK_VAR_STR != p1->type && TOK_VAR_STR != p2->type, EC_1000_VAD, code );
		b_cond = read_operand( p1 ) >= read_operand( p2 );
	}
	else if ( TOK_OPR_LESS == sp->opr )
	{
		VMASSERT2( TOK_VAR_STR != p1->type && TOK_VAR_STR != p2->type, EC_1000_VAD, code );
		b_cond = read_operand( p1 ) < read_operand( p2 );
	}
	else if ( TOK_OPR_GREATER == sp->opr )
	{
		VMASSERT2( TOK_VAR_STR != p1->type && TOK_VAR_STR != p2->type, EC_1000_VAD, code );
		b_cond = read_operand( p1 ) > read_operand( p2 );
	}
	else
		VMASSERT2( 0, EC_1000_VAD, code );

	// register the nested position.
	vm_nested_stack.add( sp->scope[1] );

	if ( b_cond )
	{
		return sp->scope[0]; // go ahead
	}
	else
	{
		// here, VM doesn't jump to the ENDWHILE VMITEM for optimization(or code reduction).
		// but we should care of that the pushed vm_nested_stack at while should be popped for stack-consistency.
		// so we pop-up the stack at here instead of ENDWHILE.
		vm_nested_stack.pop(); // throw out.
		return sp->scope[1]; // unconditional jump to ENDWHILE
	}

	VMASSERT2( 0, EC_1000_VAD, code );
	return -1;
}

VMINT CCitrus::I_ENDWHILE( VMITEM* code )
{
	vm_nested_stack.pop(); // throw out.

	SPEC_ENDWHILE* sp = (SPEC_ENDWHILE*)code->spec;
	if ( -1 != sp->next )
		return sp->next;

	VMASSERT2( 0, EC_1000_VAD, code );
	return -1;
}

VMINT CCitrus::I_PROC( VMITEM* code )
{
	int vm_ip = code->cpos;

	SPEC_PROC* sp_proc = (SPEC_PROC*)code->spec;

	if(sp_proc->params.num)
	{
		// Substitutes the parameters.
		VMINT q = 0;
		while ( vm_value_stack.num )
		{
			ALLVALUE* av = vm_value_stack.qpop();

			if ( AV_INT == av->type )
			{
				write_operand( sp_proc->params[ q ], (VMDOUBLE)av->int_val );
			}
			else if ( AV_DOUBLE == av->type )
			{
				write_operand( sp_proc->params[ q ], av->dbl_val );
			}
			else if ( AV_STRING == av->type )
			{
				write_operand_str( sp_proc->params[ q ], av->string );
			}

			q++;
		}
	}

	return sp_proc->scope[ 0 ] + 1;
}

VMINT CCitrus::I_CALL( VMITEM* code )
{
	int vm_ip = code->cpos;

	// save the returning position.
	CALLSTACKITEM* csi = new CALLSTACKITEM;
	csi->call		= code;
	csi->ip			= vm_ip + 1;

	vm_callstack.add( csi );

	SPEC_CALL* sp_call = (SPEC_CALL*)code->spec;

	for ( VMINT q=0; q<sp_call->params.num; q++)
		push_value( sp_call->params[ q ] );

	return sp_call->pos; // jump to the procedure.
}


VMINT CCitrus::I_EVENT( VMITEM* code )
{
	SPEC_EVENT* sp = (SPEC_EVENT*)code->spec;

	VMINT funcid = (VMINT)read_operand( sp->funcid );
	VMINT param = (VMINT)read_operand( sp->param );

	VMASSERT2( _vm_callback, EC_1000_VAD, code );
	_vm_callback( funcid, param );

	return 0;
}

VMINT CCitrus::I_ENDPROC( VMITEM* code )
{
	SPEC_ENDPROC* sp_endproc = (SPEC_ENDPROC*)code->spec;

	CALLSTACKITEM* csi = vm_callstack.pop();
	int jumpto = csi->ip;
	delete csi;

	if ( NULL == sp_endproc->call )
	{// the main entry function does not have parameters:  sp->call = null.   
		return jumpto;
	}

	if ( FALSE == sp_endproc->call->b_exist_byref )
		return jumpto; // skip the call-by-ref process.

	if ( sp_endproc->proc->params.num > 0 && (sp_endproc->proc->params.num == sp_endproc->call->params_byref.num) )
	{
		for (int k=0; k<sp_endproc->call->params_byref.num; k++)
		{
			if ( 1 == sp_endproc->call->params_byref[ k ] )
			{// it's call-by-ref parameter.
				int vc = sp_endproc->call->params[ k ]->cls.cls;

				if ( VMCLS_I == vc )
				{
					VMINT v = (VMINT)read_operand( sp_endproc->proc->params[ k ] );
					write_operand( sp_endproc->call->params[ k ], (VMDOUBLE)v );
				}
				else if ( VMCLS_F == vc )
				{
					VMDOUBLE v = read_operand( sp_endproc->proc->params[ k ] );
					write_operand( sp_endproc->call->params[ k ], v );
				}
			}
		}
	}

	return jumpto;
}

VMINT CCitrus::I_BREAK( VMITEM* code )
{
	VMASSERT2( vm_nested_stack.num, EC_1000_VAD, code );
	return vm_nested_stack.pop();
}

VMINT CCitrus::I_END( VMITEM* code )
{
//	VMASSERT2( 0 == vm_callstack.num, EC_0018_USCS, code );
	return -1;
}


