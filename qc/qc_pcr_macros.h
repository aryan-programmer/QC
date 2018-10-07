#pragma once
#include <boost\preprocessor\seq\for_each.hpp>
#include <boost\preprocessor\seq\for_each_i.hpp>
#include <boost\preprocessor\variadic\to_seq.hpp>
#include <boost\preprocessor\control\expr_iif.hpp>
#include <boost\preprocessor\logical\or.hpp>
#include <boost\preprocessor\comparison\not_equal.hpp>
#include <boost\preprocessor\comparison\equal.hpp>
#include <boost\preprocessor\stringize.hpp>

#define OP_AND_PRECs BOOST_PP_VARIADIC_TO_SEQ(\
	(1)(*),(1)(/),(1)(%),\
	(2)(+),(2)(-),\
	(3)(<<),(3)(>>),\
	(10)(=),\
	(10)(*=),(10)(/=),(10)(%=),\
	(10)(+=),(10)(-=),\
	(10)(<<=),(10)(>>=))

#define OP(elem) BOOST_PP_SEQ_ELEM(1,elem)

#define PREC(elem) BOOST_PP_SEQ_ELEM(0,elem)

#define IS_SHIFT_OP(idx) \
	BOOST_PP_OR(BOOST_PP_EQUAL(idx,5),\
		BOOST_PP_OR(BOOST_PP_EQUAL(idx,6),\
			BOOST_PP_OR(\
				BOOST_PP_EQUAL(idx,13),\
				BOOST_PP_EQUAL(idx,14))))


#define LINE_AST_OP_PARSER_SINGULAR(r, data, i, elem) \
	BOOST_PP_EXPR_IIF(BOOST_PP_NOT_EQUAL(i,0), |) string(BOOST_PP_STRINGIZE(OP(elem)))

#define LINE_AST_OP_PARSER \
	BOOST_PP_SEQ_FOR_EACH_I(LINE_AST_OP_PARSER_SINGULAR,_,OP_AND_PRECs)


#define OP_IF_STATEMENTS_SINGULAR(r, data, i, elem) \
	BOOST_PP_EXPR_IIF(BOOST_PP_NOT_EQUAL(i,0), else)\
	if(op == BOOST_PP_STRINGIZE(OP(elem)))\
	res = prev OP(elem) \
			BOOST_PP_IIF(IS_SHIFT_OP(i),static_cast<boost::int64_t>(post), post);

#define OP_IF_STATEMENTS \
	BOOST_PP_SEQ_FOR_EACH_I(OP_IF_STATEMENTS_SINGULAR,_,OP_AND_PRECs)


#define OP_N_PREC_INIT_LIST_SINGULAR(r, data, elem) \
	{BOOST_PP_STRINGIZE(OP(elem)), PREC(elem)},

#define OP_N_PREC_INIT_LIST \
	BOOST_PP_SEQ_FOR_EACH(OP_N_PREC_INIT_LIST_SINGULAR,_,OP_AND_PRECs)
