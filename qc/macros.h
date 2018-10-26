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

#define BLOCK_TAGS_DATA \
BOOST_PP_VARIADIC_TO_SEQ(\
	(QC			)((0)		),\
	(Class		)((0)		),\
	(Structure	)((0)		),\
	(Namespace	)((0)		),\
	(Interface	)((0)		),\
	(Enum		)((0)		),\
	(Native		)((0)		),\
	(Property	)((0)		),\
	(Indexer	)((0)		),\
	(Unsafe		)((0)		),\
	(Unchecked	)((0)		),\
	(Checked	)((0)		),\
	(Get		)((0)		),\
	(Set		)((0)		),\
	(If			)((0)		),\
	(ElseIf		)((0)		),\
	(Else		)((0)		),\
	(Switch		)((0)		),\
	(FTCase		)((0)		),\
	(Case		)((0)		),\
	(Default	)((0)		),\
	(FTDefault	)((0)		),\
	(Try		)((0)		),\
	(Catch		)((0)		),\
	(Finally	)((0)		),\
	(Do			)((1)(Loop)	),\
	(Until		)((1)(Loop)	),\
	(While		)((1)(Loop)	),\
	(For		)((1)(Next)	),\
	(RevFor		)((1)(Prev)	),\
	(ForEach	)((1)(Next)	),\
	(ForEver	)((1)(Next)	),\
	(Comment	)((0)		),\
	(Function	)((0)		)\
)

#define BLOCK_TAG(v) BOOST_PP_SEQ_ELEM(0,v)
#define BLOCK_TAG_ENUM(v) BOOST_PP_CAT(BOOST_PP_CAT(_,BLOCK_TAG(v)),_)
#define BLOCK_TAG_FULL_ENUM(v) ::tags::BLOCK_TAG_ENUM(v)
#define BLOCK_TAG_FWD_END_helper0(v) /BOOST_PP_SEQ_ELEM(1,BOOST_PP_SEQ_ELEM(1,v))
#define BLOCK_TAG_FWD_END_helper1(v) /
#define BLOCK_TAG_FWD_END(v) \
	BOOST_PP_IIF(\
		BOOST_PP_SEQ_ELEM(0,BOOST_PP_SEQ_ELEM(1,v)),\
		BLOCK_TAG_FWD_END_helper0(v),\
		BLOCK_TAG_FWD_END_helper1(v)\
		)

#define DEFINE_TAGS_helper(r, data, i, elem) \
	BOOST_PP_COMMA_IF(i) BLOCK_TAG_ENUM(elem)

#define DEFINE_TAGS \
	BOOST_PP_SEQ_FOR_EACH_I(DEFINE_TAGS_helper,_,BLOCK_TAGS_DATA)

#define DEFINE_PARSER_helper(r, data, i, v) \
		BOOST_PP_EXPR_IF(i,|) \
		lit(BOOST_PP_STRINGIZE(BLOCK_TAG(v)))[\
			_val = BLOCK_TAG_FULL_ENUM(v)\
		]

#define DEFINE_PARSER \
	BOOST_PP_SEQ_FOR_EACH_I(DEFINE_PARSER_helper,_,BLOCK_TAGS_DATA)

#define DEFINE_QC_CONSTRUCTS_helper(r, data, i, v) \
	BOOST_PP_COMMA_IF(i) {BLOCK_TAG_FULL_ENUM(v),{BOOST_PP_STRINGIZE(BLOCK_TAG(v)),BOOST_PP_STRINGIZE(BLOCK_TAG_FWD_END(v))}}

#define DEFINE_QC_CONSTRUCTS \
	BOOST_PP_SEQ_FOR_EACH_I(DEFINE_QC_CONSTRUCTS_helper,_,BLOCK_TAGS_DATA)

#define DEFINE_OSTR_SWITCH_CASES_helper(r, data, i, elem) \
	case BLOCK_TAG_FULL_ENUM(elem): \
		return end ? ::constructsMap.at(lang).at(tag).second : ::constructsMap.at(lang).at(tag).first; \
		break;

#define DEFINE_OSTR_SWITCH_CASES \
	BOOST_PP_SEQ_FOR_EACH_I(DEFINE_OSTR_SWITCH_CASES_helper,_,BLOCK_TAGS_DATA)
