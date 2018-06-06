
#include "ESL.h"

VMINT CCitrus::I_ADDADD( VMITEM* code )
{
	// ADDADD
	// A++

	VMITEM* t_l = code->left;

	VMASSERT2( t_l, EC_2001_WOPR, code );
	VMASSERT2( 1 != t_l->cls.cconst, EC_2002_CCLVAL, code );

	VMDOUBLE lv = 0.0;

	lv = read_operand( t_l );

	vm_r0 = lv+1;

	write_operand( t_l, vm_r0 ); 

	return 0;
}

VMINT CCitrus::I_ADD( VMITEM* code )
{
	// ADD
	// C + D

	VMITEM* t_l = code->left;
	VMITEM* t_r = code->right;

	double lv = 0.0;
	double rv = 0.0;

	if ( t_l && t_r )
	{// add both directly and push it.
		lv = read_operand( t_l );
		rv = read_operand( t_r );
	}
	else if ( t_l )
	{
		lv = read_operand( t_l );
		rv = vm_calc_stack.pop();
	}
	else if ( t_r )
	{
		rv = read_operand( t_r );
		lv = vm_calc_stack.pop();
	}
	else
	{// both value are on the stack.
		VMASSERT2( vm_calc_stack.num == 2, EC_2001_WOPR, code );

		lv = vm_calc_stack.pop();
		rv = vm_calc_stack.pop();
	}

	vm_r0 = lv + rv;
	vm_calc_stack.add( vm_r0 );

	return 0;
}

VMINT CCitrus::I_SUBSUB( VMITEM* code )
{
	// SUBSUB
	// A--

	VMITEM* t_l = code->left;

	VMASSERT2( t_l, EC_2001_WOPR, code );
	VMASSERT2( 1 != t_l->cls.cconst, EC_2002_CCLVAL, code );

	VMDOUBLE lv = 0.0;

	lv = read_operand( t_l );

	write_operand( t_l, lv-1 ); 

	vm_r0 = lv-1;

	return 0;
}

VMINT CCitrus::I_SUB( VMITEM* code )
{
	// SUBTRACT
	// C - D

	VMITEM* t_l = code->left;
	VMITEM* t_r = code->right;

	double lv = 0.0;
	double rv = 0.0;

	if ( t_l && t_r )
	{// sub both directly and push it.
		lv = read_operand( t_l );
		rv = read_operand( t_r );
	}
	else if ( t_l )
	{
		lv = read_operand( t_l );
		rv = vm_calc_stack.pop();
	}
	else if ( t_r )
	{
		rv = read_operand( t_r );
		lv = vm_calc_stack.pop();
	}
	else
	{// both value are on the stack.
		VMASSERT2( vm_calc_stack.num == 2, EC_2001_WOPR, code );

		lv = vm_calc_stack.pop();
		rv = vm_calc_stack.pop();
	}

	vm_r0 = lv - rv;
	vm_calc_stack.add( vm_r0 );

	return 0;
}

VMINT CCitrus::I_MUL( VMITEM* code )
{
	// MULTIPLY
	// C * D

	VMITEM* t_l = code->left;
	VMITEM* t_r = code->right;

	double lv = 0.0;
	double rv = 0.0;

	if ( t_l && t_r )
	{// mul both directly and push it.
		lv = read_operand( t_l );
		rv = read_operand( t_r );
	}
	else if ( t_l )
	{
		lv = read_operand( t_l );
		rv = vm_calc_stack.pop();
	}
	else if ( t_r )
	{
		rv = read_operand( t_r );
		lv = vm_calc_stack.pop();
	}
	else
	{// both value are on the stack.
		VMASSERT2( vm_calc_stack.num == 2, EC_2001_WOPR, code );

		lv = vm_calc_stack.pop();
		rv = vm_calc_stack.pop();
	}

	vm_r0 = lv * rv;
	vm_calc_stack.add( vm_r0 );

	return 0;
}

VMINT CCitrus::I_DIV( VMITEM* code )
{
	// DIVIDE
	// C / D

	VMITEM* t_l = code->left;
	VMITEM* t_r = code->right;

	double lv = 0.0;
	double rv = 0.0;

	if ( t_l && t_r )
	{// div both directly and push it.
		lv = read_operand( t_l );
		rv = read_operand( t_r );
	}
	else if ( t_l )
	{
		lv = read_operand( t_l );
		rv = vm_calc_stack.pop();
	}
	else if ( t_r )
	{
		rv = read_operand( t_r );
		lv = vm_calc_stack.pop();
	}
	else
	{// both value are on the stack.
		VMASSERT2( vm_calc_stack.num == 2, EC_2001_WOPR, code );

		lv = vm_calc_stack.pop();
		rv = vm_calc_stack.pop();
	}

	VMASSERT2( 0.0 != rv, EC_2003_DBZ, code );
	vm_r0 = lv / rv;
	vm_calc_stack.add( vm_r0 );

	return 0;
}

VMINT CCitrus::I_MOD( VMITEM* code )
{
	// MODULUS
	// C % D

	VMITEM* t_l = code->left;
	VMITEM* t_r = code->right;

	double lv = 0.0;
	double rv = 0.0;

	if ( t_l && t_r )
	{// mod both directly and push it.
		lv = read_operand( t_l );
		rv = read_operand( t_r );
	}
	else if ( t_l )
	{
		lv = read_operand( t_l );
		rv = vm_calc_stack.pop();
	}
	else if ( t_r )
	{
		rv = read_operand( t_r );
		lv = vm_calc_stack.pop();
	}
	else
	{// both value are on the stack.
		VMASSERT2( vm_calc_stack.num == 2, EC_2001_WOPR, code );

		lv = vm_calc_stack.pop();
		rv = vm_calc_stack.pop();
	}

	vm_r0 = (VMDOUBLE)((VMINT)lv % (VMINT)rv);
	vm_calc_stack.add( vm_r0 );

	return 0;
}


VMINT CCitrus::I_EQUAL( VMITEM* code )
{
	// LET =
	// A = B;

	VMITEM* t_l = code->left;
	VMITEM* t_r = code->right;

	if ( 1 == t_l->cls.carrayidx )
	{// it's an index of array.
	 // the array var is on the very left.
		t_l = t_l->left;
	}


	VMASSERT2( t_l && t_r, EC_2001_WOPR, code );
	VMASSERT2( 1 != t_l->cls.cconst, EC_2002_CCLVAL, code );

	if ( ( VMCLS_S == t_l->cls.cls && 1 != t_l->cls.cconst ) && VMCLS_S == t_r->cls.cls )
	{
		char* v = read_operand_str( t_r );
		write_operand_str( t_l, v );
		return 0;
	}
	else
	{
		if ( t_r->referred )
		{// the operand has been calculated by some operator. Take vm_r0 instead.
			// remove calc stack
			while ( ! vm_calc_stack.isempty() )
				vm_calc_stack.pop();

			write_operand( t_l, vm_r0 );

			return 0;
		}
		else
		{// let the t_r to be t_l
			// remove calc stack
			while ( ! vm_calc_stack.isempty() )
				vm_calc_stack.pop();

			VMDOUBLE rv = read_operand( t_r );
			write_operand( t_l, rv );
			return 0;
		}
	}

	VMASSERT2( 0, EC_2004_TMISM, code );

	return 0;
}


VMINT CCitrus::I_ADD_EQUAL( VMITEM* code )
{
	I_ADD( code );
	I_EQUAL( code );
	return 0;
}

VMINT CCitrus::I_SUB_EQUAL( VMITEM* code )
{
	I_SUB( code );
	I_EQUAL( code );
	return 0;
}

VMINT CCitrus::I_MUL_EQUAL( VMITEM* code )
{
	I_MUL( code );
	I_EQUAL( code );
	return 0;
}

VMINT CCitrus::I_DIV_EQUAL( VMITEM* code )
{
	I_DIV( code );
	I_EQUAL( code );
	return 0;
}

VMINT CCitrus::I_MOD_EQUAL( VMITEM* code )
{
	I_MOD( code );
	I_EQUAL( code );
	return 0;
}


