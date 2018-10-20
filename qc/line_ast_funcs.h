#pragma once
using bint = mp::cpp_int;
using bflt = mp::cpp_dec_float_100;
using variable_t = boost::variant<bint , bflt, bool>;
using shared_variable_t = std::shared_ptr<variable_t>;
using line_ast_node = boost::make_recursive_variant<
	std::vector<boost::recursive_variant_> , std::string>::type;
using line_ast = std::vector<line_ast_node>;
using bline_ast_node = boost::make_recursive_variant<
	std::vector<boost::recursive_variant_> , std::string , shared_variable_t>::type;
using bline_ast = std::vector<bline_ast_node>;

bline_ast line_ast_to_bline_ast( line_ast&& ast );

class qc_line_ast_generator : public qi::grammar<std::string_view::const_iterator , line_ast( ) , ascii::space_type>
{
public:
	using Iter = std::string_view::const_iterator;
	qc_line_ast_generator( );

	qi::rule<Iter , line_ast_node( ) , ascii::space_type>expr;
	qi::rule<Iter , std::string( ) , ascii::space_type>operator_;
	qi::rule<Iter , int( ) , ascii::space_type>integ;
	qi::rule<Iter , line_ast( ) , ascii::space_type> line;
};

