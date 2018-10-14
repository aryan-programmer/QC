#include "stdafx.h"
#include "line_ast_funcs.h"
#include "qc_pcr_macros.h"

bline_ast line_ast_to_bline_ast( line_ast && ast )
{
	struct vis :public boost::static_visitor<>
	{
		forceinline vis( bline_ast_node& b_ast_node ) :b_ast_node( b_ast_node ) { }
		forceinline void operator()( line_ast&& lineast ) const
		{ b_ast_node = line_ast_to_bline_ast( std::move( lineast ) ); }
		forceinline void operator()( std::string&& str ) const
		{ b_ast_node = std::move( str ); }
		bline_ast_node& b_ast_node;
	};
	bline_ast b_ast;
	b_ast.resize( ast.size( ) );
	for ( size_t i = 0; i < ast.size( ); i++ )
		boost::apply_visitor( vis( b_ast[ i ] ) , std::move( ast[ i ] ) );
	ast.clear( );
	return std::move( b_ast );
}

std::ostream& operator<<( std::ostream& o , line_ast& ast )
{
	0;
	struct :boost::static_visitor<>
	{
		forceinline void operator()( line_ast& lineast ) const
		{ std::cout << std::endl << lineast; }
		forceinline void operator()( std::string& str ) const { std::cout << str << ' '; }
	} v;
	for ( auto& i : ast )boost::apply_visitor( v , i );
	return o;
}

std::ostream & operator<<( std::ostream & o , bline_ast & ast )
{
	bool f = true;
	for ( auto& node : ast )
	{
		o << '[' << ast;
		if ( f == true ) { f = false; o << ']'; continue; }
		o << ",";
	}
	return o;
}

std::ostream & operator<<( std::ostream & o , bline_ast_node & ast )
{
	struct __________________ :boost::static_visitor<>
	{
		__________________( std::ostream & o ) :o { o } { }
		forceinline void operator()( bline_ast& ast ) { o << ast; }
		forceinline void operator()( std::string& val ) { o << val; }
		forceinline void operator()( shared_variable_t& var ) { o << *var; }
		std::ostream & o;
	}v( o );
	boost::apply_visitor( v , ast );
	return o;
}

qc_line_ast_generator::qc_line_ast_generator( ) :qc_line_ast_generator::base_type( line , "line" )
{
	using qi::lit;
	using qi::lexeme;
	using qi::on_error;
	using qi::fail;
	using ascii::alnum;
	using ascii::string;
	using ascii::space;
	using ascii::char_;
	using namespace qi::labels;

	using phoenix::construct;
	using phoenix::val;

	expr.name( "expr" );
	operator_.name( "operator" );
	line.name( "line" );

	operator_ %= LINE_AST_OP_PARSER;
	expr %= lit( '(' ) >> line >> ')' | lexeme[ +( alnum | space | char_( '.' ) ) ];
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
