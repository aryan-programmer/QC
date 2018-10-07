#pragma once
#include "qc_pcr_macros.h"

namespace mp = boost::multiprecision;
using bint = mp::cpp_int;

using line_ast_node = boost::make_recursive_variant<
	std::vector<boost::recursive_variant_> , std::string>::type;
using line_ast = std::vector<line_ast_node>;
using bline_ast_node = boost::make_recursive_variant<
	std::vector<boost::recursive_variant_> , std::string , std::shared_ptr<bint>>::type;
using bline_ast = std::vector<bline_ast_node>;

bline_ast line_ast_to_bline_ast( line_ast&& ast );

std::ostream& operator<<( std::ostream& o , line_ast& ast );

template<typename Iter>
class qc_line_ast_generator : public qi::grammar<Iter , line_ast( ) , ascii::space_type>
{
public:
	qc_line_ast_generator( ) :qc_line_ast_generator::base_type( line , "line" )
	{
		using qi::lit;
		using qi::lexeme;
		using qi::on_error;
		using qi::fail;
		using ascii::alnum;
		using ascii::string;
		using ascii::space;
		using namespace qi::labels;

		using phoenix::construct;
		using phoenix::val;

		expr.name( "expr" );
		operator_.name( "operator" );
		line.name( "line" );

		operator_ %= LINE_AST_OP_PARSER;
		expr %= lit( '(' ) >> line >> ')' | lexeme[ +( alnum | space ) ];
		line %= expr >> *( operator_ >> expr );

		on_error<fail>
			(
				line
				, std::cout
				<< val( "Error! Expecting " )
				<< _4                               // what failed?
				<< val( " here: \"" )
				<< construct<std::string>( _3 , _2 )   // iterators to error-pos, end
				<< val( "\"" )
				<< std::endl
				);
	}

	qi::rule<Iter , line_ast_node( ) , ascii::space_type>expr;
	qi::rule<Iter , std::string( ) , ascii::space_type>operator_;
	qi::rule<Iter , int( ) , ascii::space_type>integ;
	qi::rule<Iter , line_ast( ) , ascii::space_type> line;
};

