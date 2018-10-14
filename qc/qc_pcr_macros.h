#pragma once
#include <boost\preprocessor\seq\for_each.hpp>
#include <boost\preprocessor\seq\for_each_i.hpp>
#include <boost\preprocessor\variadic\to_seq.hpp>
#include <boost\preprocessor\control\expr_iif.hpp>
#include <boost\preprocessor\logical\bitor.hpp>
#include <boost\preprocessor\logical\bitand.hpp>
#include <boost\preprocessor\logical\compl.hpp>
#include <boost\preprocessor\comparison\not_equal.hpp>
#include <boost\preprocessor\comparison\equal.hpp>
#include <boost\preprocessor\stringize.hpp>

#pragma region OPS_AND_PRECEDENCES & Other Helper Macros
#define OPS_AND_PRECEDENCES \
BOOST_PP_VARIADIC_TO_SEQ(\
	(2)(*),(2)(/),(2)(%),\
	(3)(+),(3)(-),\
	(4)(<<),(4)(>>),\
	(5)(<),(5)(<=),(5)(>),(5)(>=),\
	(6)(==),(6)(!=),\
	(8)(^),\
	(10)(&&),\
	(11)(||),\
	(13)(=),\
	(13)(*=),(13)(/=),(13)(%=),\
	(13)(+=),(13)(-=),\
	(13)(<<=),(13)(>>=))

#define OPS_AND_PRECEDENCES_FOR_BFLT \
BOOST_PP_VARIADIC_TO_SEQ(\
	(2)(*),(2)(/),\
	(3)(+),(3)(-),\
	(5)(<),(5)(<=),(5)(>),(5)(>=),\
	(6)(==),(6)(!=),\
	(10)(&&),\
	(11)(||),\
	(13)(=),\
	(13)(*=),(13)(/=),\
	(13)(+=),(13)(-=))

#define OPS_AND_PRECEDENCES_FOR_BOOL \
BOOST_PP_VARIADIC_TO_SEQ(\
	(6)(==),(6)(!=),\
	(10)(&&),\
	(11)(||),\
	(13)(=))

#define OP(elem) BOOST_PP_SEQ_ELEM(1,elem)
#define PRECEDENCE(elem) BOOST_PP_SEQ_ELEM(0,elem)
#define IS_ASSIGNMENT(elem) BOOST_PP_EQUAL(PRECEDENCE(elem),13)
#define IS_AND_OR_OR(elem) BOOST_PP_BITOR(BOOST_PP_EQUAL(PRECEDENCE(elem),10),BOOST_PP_EQUAL(PRECEDENCE(elem),11))
#define IS_COMPARATIVE(elem) BOOST_PP_BITOR(BOOST_PP_EQUAL(PRECEDENCE(elem),5),BOOST_PP_EQUAL(PRECEDENCE(elem),6))

#define IS_SHIFT_OP(idx) \
	BOOST_PP_BITOR(BOOST_PP_EQUAL(idx,5),\
		BOOST_PP_BITOR(BOOST_PP_EQUAL(idx,6),\
			BOOST_PP_BITOR(\
				BOOST_PP_EQUAL(idx,BOOST_PP_SUB(BOOST_PP_SEQ_SIZE(OPS_AND_PRECEDENCES),2)),\
				BOOST_PP_EQUAL(idx,\
					BOOST_PP_SUB(BOOST_PP_SEQ_SIZE(OPS_AND_PRECEDENCES),1)))))
#pragma endregion

#pragma region LINE_AST_OP_PARSER
#define LINE_AST_OP_PARSER_SINGULAR(r, data, i, elem) \
	BOOST_PP_EXPR_IIF(BOOST_PP_NOT_EQUAL(i,0), |) string(BOOST_PP_STRINGIZE(OP(elem)))

#define LINE_AST_OP_PARSER \
	BOOST_PP_SEQ_FOR_EACH_I(LINE_AST_OP_PARSER_SINGULAR,_,BOOST_PP_SEQ_REVERSE(OPS_AND_PRECEDENCES))
#pragma endregion

#pragma region OP_IF_STATEMENTS_FOR_BINT
#define BINT_RHS(op,i) \
BOOST_PP_IIF(IS_SHIFT_OP(i),cst<::std::intmax_t>(rhs),cst<bint>(rhs))

#define OP_IF_STATEMENTS_FOR_BINT_SINGULAR_WHEN_NORMAL(op,i)\
res=shared_variable_t(new variable_t(bint(cst<bint>(lhs) OP(op) BINT_RHS(op,i))));

#define OP_IF_STATEMENTS_FOR_BINT_SINGULAR_WHEN_ASSIGN(op,i) \
lhs OP(op) BINT_RHS(op,i); res = prev;

#define OP_IF_STATEMENTS_FOR_BINT_SINGULAR_WHEN_COMPARE(op,i) \
res = shared_variable_t( new variable_t( bool( lhs OP(op) rhs ) ) );

#define OP_IF_STATEMENTS_FOR_BINT_SINGULAR(r, data, i, elem) \
	BOOST_PP_EXPR_IIF(BOOST_PP_NOT_EQUAL(i,0), else)\
	if(op == BOOST_PP_STRINGIZE(OP(elem)))\
	{BOOST_PP_IIF(\
		BOOST_PP_BITOR(IS_AND_OR_OR(elem),IS_COMPARATIVE(elem)),\
		OP_IF_STATEMENTS_FOR_BINT_SINGULAR_WHEN_COMPARE,\
		BOOST_PP_IIF(\
			IS_ASSIGNMENT(elem),\
			OP_IF_STATEMENTS_FOR_BINT_SINGULAR_WHEN_ASSIGN, \
			OP_IF_STATEMENTS_FOR_BINT_SINGULAR_WHEN_NORMAL))(elem,i);}

#define OP_IF_STATEMENTS_FOR_BINT \
	BOOST_PP_SEQ_FOR_EACH_I(OP_IF_STATEMENTS_FOR_BINT_SINGULAR,_,OPS_AND_PRECEDENCES)
#pragma endregion

#pragma region OP_IF_STATEMENTS_FOR_BFLT
#define OP_IF_STATEMENTS_FOR_BFLT_SINGULAR_WHEN_NORMAL(op)\
res=shared_variable_t(new variable_t(bflt(cst<bflt>(lhs) OP(op) rhs)));

#define OP_IF_STATEMENTS_FOR_BFLT_SINGULAR_WHEN_ASSIGN(op) \
lhs OP(op) rhs; res = prev;

#define OP_IF_STATEMENTS_FOR_BFLT_SINGULAR_WHEN_COMPARE(op) \
res=shared_variable_t(new variable_t(bool(cst<bflt>(lhs) OP(op) rhs)));

#define OP_IF_STATEMENTS_FOR_BFLT_SINGULAR(r, data, i, elem) \
	BOOST_PP_EXPR_IIF(BOOST_PP_NOT_EQUAL(i,0), else)\
	if(op == BOOST_PP_STRINGIZE(OP(elem)))\
	{BOOST_PP_IIF(\
		BOOST_PP_BITOR(IS_AND_OR_OR(elem),IS_COMPARATIVE(elem)),\
		OP_IF_STATEMENTS_FOR_BFLT_SINGULAR_WHEN_COMPARE,\
		BOOST_PP_IIF(\
			IS_ASSIGNMENT(elem),\
			OP_IF_STATEMENTS_FOR_BFLT_SINGULAR_WHEN_ASSIGN, \
			OP_IF_STATEMENTS_FOR_BFLT_SINGULAR_WHEN_NORMAL))(elem);}

#define OP_IF_STATEMENTS_FOR_BFLT \
	BOOST_PP_SEQ_FOR_EACH_I(OP_IF_STATEMENTS_FOR_BFLT_SINGULAR,_,OPS_AND_PRECEDENCES_FOR_BFLT)
#pragma endregion

#pragma region OP_IF_STATEMENTS_FOR_BOOL
#define OP_IF_STATEMENTS_FOR_BOOL_SINGULAR_WHEN_ASSIGN(op) \
lhs OP(op) cst<bool>(rhs); res = post;

#define OP_IF_STATEMENTS_FOR_BOOL_SINGULAR_WHEN_COMPARE(op) \
res=shared_variable_t(new variable_t(bool(cst<bool>(lhs) OP(op) cst<bool>(rhs))));

#define OP_IF_STATEMENTS_FOR_BOOL_SINGULAR(r, data, i, elem) \
	BOOST_PP_EXPR_IIF(BOOST_PP_NOT_EQUAL(i,0), else)\
	if(op == BOOST_PP_STRINGIZE(OP(elem)))\
	{BOOST_PP_IIF(\
		IS_ASSIGNMENT(elem),\
		OP_IF_STATEMENTS_FOR_BOOL_SINGULAR_WHEN_ASSIGN,\
		OP_IF_STATEMENTS_FOR_BOOL_SINGULAR_WHEN_COMPARE)(elem);}

#define OP_IF_STATEMENTS_FOR_BOOL \
	BOOST_PP_SEQ_FOR_EACH_I(OP_IF_STATEMENTS_FOR_BOOL_SINGULAR,_,OPS_AND_PRECEDENCES_FOR_BOOL)
#pragma endregion

#pragma region OP_N_PREC_INIT_LIST
#define OP_N_PREC_INIT_LIST_SINGULAR(r, data, elem) \
	{BOOST_PP_STRINGIZE(OP(elem)), PRECEDENCE(elem)},

#define OP_N_PREC_INIT_LIST \
	BOOST_PP_SEQ_FOR_EACH(OP_N_PREC_INIT_LIST_SINGULAR,_,OPS_AND_PRECEDENCES)
#pragma endregion