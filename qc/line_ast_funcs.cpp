#include "stdafx.h"
#include "line_ast_funcs.h"

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
		forceinline void operator()( bint& int_ ) const { std::cout << int_ << ' '; }
		forceinline void operator()( std::string& str ) const { std::cout << str << ' '; }
	} v;
	for ( auto& i : ast )boost::apply_visitor( v , i );
	return o;
}

