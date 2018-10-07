#include "stdafx.h"
#include "qc_parser_runner.h"
#include "cross_utils.h"
#include "line_ast_funcs.h"
#include <boost\multiprecision\cpp_int.hpp>
#include <boost\scope_exit.hpp>

std::unordered_map<std::string , std::shared_ptr<bint>> variables;

//std::list<std::tuple<std::string , boost::variant<std::string , std::tuple<std::string , std::string>>>> pastTags;

const std::unordered_map<std::string , size_t> operators { OP_N_PREC_INIT_LIST };

template<bool check , typename T>
using if_check_then_cnst = boost::conditional_t<check , const T , T>;

template<bool check> struct qc_pcrer;

template<bool check>
void pcr_qc( qc_data& qc )
{
	auto tagParsed = false;
	const std::string& tagVal = qc.name;
	for ( auto& node : qc.children )
		boost::apply_visitor( qc_pcrer<check>( tagVal , tagParsed ) , node );
}

template<bool check>
struct qc_pcrer : boost::static_visitor<>
{
	qc_pcrer( const std::string& tagVal , bool& isTagParsed ) :
		tagVal { tagVal } , isTagParsed { isTagParsed } { }

	forceinline void operator()( qc_data& qc ) const { pcr_qc<check>( qc ); }
	void operator()( if_check_then_cnst<check , std::string>& text ) const;

	const std::string& tagVal;
	bool& isTagParsed;
};

template<> void qc_pcrer<true>::operator()( if_check_then_cnst<true , std::string>& text ) const
{
	if ( tagVal != "$" )
	{
		loop_split_with_enable_disable(
			std::string_view( text ) ,
			[ ] ( auto&& idx , auto&& iter ) { return *iter == '\n'; } ,
			[ ] ( auto&& idx , auto&& iter ) { return isStrQuote( idx , iter ); } ,
			[ ] ( auto&& idx , auto&& iter ) { return isStrQuote( idx , iter ); } ,
			[ ] ( auto&& val ) { checkStr( val , "" ); } );
	}
}

std::shared_ptr<bint> evaluate( bline_ast& ast );
std::shared_ptr<bint> evaluate( std::string& str )
{
	boost::algorithm::trim( str );
	try
	{
		auto v = std::unique_ptr<bint>( new bint( ) );
		*v = bint( str );
		return std::move( v );
	}
	catch ( ... )
	{
		try { return variables.at( str ); }
		catch ( ... )
		{
			std::vector<std::string> splits;
			split(
				str ,
				std::back_inserter( splits ) ,
				[ ] ( auto&& , auto&& iter )
				{ return ( boost::algorithm::is_space( )( *iter ) ); } );
			if ( splits.size( ) != 2 )
				throw throwVal( "Invalid expr: " + str , invalidExpr );
			else
			{
				auto p = variables.emplace( splits[ 1 ] , std::shared_ptr<bint>( new bint( 0 ) ) );
				if ( !p.second )
					throw throwVal(
						"Re-declaration of variable: " + str + "not allowed" ,
						invalidExpr );
				return p.first->second;
			}
		}
	}
}

struct evaluator_visitor :boost::static_visitor<std::shared_ptr<bint>>
{
	forceinline std::shared_ptr<bint> operator()( bline_ast& lineast ) const
	{ return evaluate( lineast ); }
	forceinline std::shared_ptr<bint> operator()( std::shared_ptr<bint>& int_ ) const
	{ return int_; }
	forceinline std::shared_ptr<bint> operator()( std::string& str ) const
	{ return evaluate( str ); }
} v;


void eval_op( std::string &op , bint &res , bint &prev , bint &post ) { OP_IF_STATEMENTS; }

void op_eval( evaluator_visitor &v , bline_ast & ast , const size_t & pos )
{
	std::shared_ptr<bint>
		prev = boost::apply_visitor( v , ast[ pos - 1 ] ) ,
		post = boost::apply_visitor( v , ast[ pos + 1 ] ) ,
		res = std::shared_ptr<bint>( new bint( 0 ) );
	auto op = boost::get<std::string>( ast[ pos ] );
	if ( !( prev&&post&&res ) )goto end;
	eval_op( op , *res , *prev , *post );
end:
	ast[ pos - 1 ] = std::move( res );
	ast.erase( ast.begin( ) + pos , ast.begin( ) + pos + 2 );
}

std::shared_ptr<bint> evaluate( bline_ast& ast )
{
	if ( ast.size( ) > 3 )
	{
		std::vector<size_t> ordered_poss;
		for ( size_t i = 1; i < ast.size( ); i += 2 )
			ordered_poss.push_back( i );
		std::stable_sort(
			ordered_poss.begin( ) ,
			ordered_poss.end( ) ,
			[ &ast ] ( const size_t& pos1 , const size_t& pos2 )
			{
				return
					operators.at( boost::get<std::string>( ast[ pos1 ] ) ) <
					operators.at( boost::get<std::string>( ast[ pos2 ] ) );
			} );
		for ( size_t idx = 0; idx < ordered_poss.size( ); idx++ )
		{
			const auto& pos = ordered_poss[ idx ];
			op_eval( v , ast , pos );
			for ( size_t i = idx; i < ordered_poss.size( ); i++ )
				if ( ordered_poss[ i ] > pos )
					ordered_poss[ i ] -= 2;
		}
	}
	else if ( ast.size( ) == 1 );
	// ast.size( ) == 3
	else op_eval( v , ast , 1 );
	return boost::apply_visitor( v , ast[ 0 ] );
}

std::shared_ptr<bint> fullEval( std::string_view splitStr )
{
	using boost::spirit::ascii::space;
	using qi::phrase_parse;
	using namespace std;

	if ( splitStr.substr( 0 , 5 ) == "print" )
	{
		auto notPrt = splitStr.substr( 6 );
		loop_split_with_enable_disable(
			notPrt ,
			[ ] ( auto&& , auto&& iter ) { return *iter == ','; } ,
			[ ] ( auto&& i , auto&& iter )
			{ return *iter == '(' || isStrQuote( i , iter ); } ,
			[ ] ( auto&& i , auto&& iter )
			{ return *iter == ')' || isStrQuote( i , iter ); } ,
			[ ] ( std::string_view&& val )
			{
				if ( val == "endl" )
					std::cout << std::endl;
				else if ( val[ 0 ] == '`' )
					std::cout << val.substr( 1 , val.size( ) - 2 );
				else
					std::cout << *fullEval( val );
			} );
		return std::shared_ptr<bint>( new bint( 0 ) );
	}

	qc_line_ast_generator<std::string_view::const_iterator> gen;
	line_ast ast;
	auto iter = splitStr.begin( );
	auto end = splitStr.end( );
	auto succeed = phrase_parse( iter , end , gen , space , ast );
	bline_ast b_ast = line_ast_to_bline_ast( std::move( ast ) );
	return ::evaluate( b_ast );
}


template<>
void qc_pcrer<false>::operator()( if_check_then_cnst<false , std::string>& text ) const
{
	using boost::spirit::ascii::space;
	using qi::phrase_parse;
	using namespace std;

	loop_split(
		text ,
		[ ] ( auto&& i , auto&& iter ) { return *iter == '\n' && ( i == 0 ? true : *( iter - 1 ) != '_' ); } ,
		fullEval );
}
void pcr_file( boost::filesystem::path & filename )
{
	using namespace std;
	using namespace std::string_literals;

	bool succeeded;
	string::const_iterator iter , end;
	qc_grammar qc;
	qc_data ast;
	string storage;

	parse_file( filename , storage , iter , end , succeeded , qc , ast );
	pcr_qc<true>( ast );
	pcr_qc<false>( ast );
}
