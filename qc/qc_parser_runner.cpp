#include "stdafx.h"
#include "qc_parser_runner.h"
#include "cross_utils.h"
#include "line_ast_funcs.h"
#include "qc_pcr_macros.h"
#include <boost\multiprecision\cpp_int.hpp>
#include <boost\scope_exit.hpp>

std::unordered_map<std::string , shared_variable_t> variables;

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

shared_variable_t evaluate( bline_ast& ast );
shared_variable_t evaluate( std::string& str )
{
	boost::algorithm::trim( str );
	auto v = std::unique_ptr<variable_t>( new variable_t( ) );
	try { *v = bint( str ); }
	catch ( ... )
	{
		try { *v = bflt( str ); }
		catch ( ... )
		{
			if ( str == "true" ) *v = true;
			else if ( str == "false" )*v = false;
			else goto var_expr;
		}
	}
	return std::move( v );

var_expr:
	try
	{
		return variables.at( str );
	}
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
			auto p = variables.emplace( splits[ 1 ] , std::move( v ) );
			if ( !p.second )
				throw throwVal(
					"Re-declaration of variable: " + str + " not allowed" ,
					invalidExpr );
			return p.first->second;
		}
	}
}

struct evaluator_visitor :boost::static_visitor<shared_variable_t>
{
	forceinline shared_variable_t operator()( bline_ast& lineast ) const
	{
		return evaluate( lineast );
	}
	forceinline shared_variable_t operator()( shared_variable_t& val ) const
	{
		return val;
	}
	forceinline shared_variable_t operator()( std::string& str ) const
	{
		return evaluate( str );
	}
} v;

template<typename...> class TD;

void eval_op( std::string &op , shared_variable_t& res , shared_variable_t& prev , shared_variable_t& post )
{
	boost::apply_visitor(
		[ &op , &res , &prev , &post ] ( auto& lhs , auto& rhs )
		{
			using lhs_t = std::remove_reference_t<decltype( lhs )>;
			using rhs_t = std::remove_reference_t<decltype( rhs )>;
			constexpr bool isLHSBint = std::is_same_v<lhs_t , bint>;
			constexpr bool isRHSBint = std::is_same_v<rhs_t , bint>;
			constexpr bool isLHSBflt = std::is_same_v<lhs_t , bflt>;
			constexpr bool isRHSBflt = std::is_same_v<rhs_t , bflt>;
			constexpr bool isLHSBool = std::is_same_v<lhs_t , bool>;
			constexpr bool isRHSBool = std::is_same_v<rhs_t , bool>;
			decltype( auto ) backup_rhs = rhs;
			if ( op == "=" )
			{
				*prev = *post;
				res = prev;
			}
			else if
				constexpr( isLHSBool && isRHSBool ) { OP_IF_STATEMENTS_FOR_BOOL }
			else if
				constexpr( isLHSBflt && ( isRHSBint || isRHSBflt ) )
			{
				boost::conditional_t<isRHSBint , bflt , bflt&> rhs = static_cast< boost::conditional_t<isRHSBint , bflt , bflt&> >( backup_rhs );
				OP_IF_STATEMENTS_FOR_BFLT
			}
			else if
				constexpr( isLHSBint && ( isRHSBint || isRHSBflt ) )
			{
				boost::conditional_t<isRHSBflt , bint , bint&> rhs = static_cast< boost::conditional_t<isRHSBflt , bint , bint&> >( backup_rhs );
				OP_IF_STATEMENTS_FOR_BINT
			}
			else
			{
				using namespace std::string_literals;
				throw throwVal(
					std::string( "Operation between types " ) +
					( isLHSBint ? "bint" : ( isLHSBflt ? "bflt" : "bool" ) ) + " and " +
					( isRHSBint ? "bint" : ( isRHSBflt ? "bflt" : "bool" ) ) +
					" is not supported" , invalidTypes );
			}
		} , *prev , *post );
}

void op_eval( evaluator_visitor &v , bline_ast & ast , const size_t & pos )
{
	shared_variable_t
		prev = boost::apply_visitor( v , ast[ pos - 1 ] ) ,
		post = boost::apply_visitor( v , ast[ pos + 1 ] ) ,
		res;
	auto op = boost::get<std::string>( ast[ pos ] );
	if ( !( prev&&post ) )goto end;
	eval_op( op , res , prev , post );
end:
	ast[ pos - 1 ] = std::move( res );
	ast.erase( ast.begin( ) + pos , ast.begin( ) + pos + 2 );
}

shared_variable_t evaluate( bline_ast& ast )
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


shared_variable_t fullEval( std::string&& splitStr )
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
			[ ] ( string&& val )
			{
				boost::trim( val );
				if ( val == "endl" )
					cout << std::endl;
				else if ( val[ 0 ] == '`' )
					cout << val.substr( 1 , val.size( ) - 2 );
				else
					cout << *fullEval( string( val ) );
			} );
		return shared_variable_t( new variable_t( ) );
	}
	commonReplacement( splitStr );
	string_view splitStrView = splitStr;

	qc_line_ast_generator gen;
	line_ast ast;
	auto iter = splitStrView.begin( );
	auto end = splitStrView.end( );
	auto succeed = phrase_parse( iter , end , gen , space , ast );
	bline_ast b_ast = line_ast_to_bline_ast( std::move( ast ) );
	BOOST_ASSERT( end == iter );
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
