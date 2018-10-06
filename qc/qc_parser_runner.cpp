#include "stdafx.h"
#include "qc_parser_runner.h"
#include "cross_utils.h"
#include <boost\multiprecision\cpp_int.hpp>
#include <boost\ptr_container\ptr_map.hpp>

namespace mp = boost::multiprecision;
using bint = mp::cpp_int;

#define forceinline BOOST_FORCEINLINE

std::unordered_map<std::string , std::shared_ptr<bint>> variables;

//std::list<std::tuple<std::string , boost::variant<std::string , std::tuple<std::string , std::string>>>> pastTags;

const std::unordered_map<std::string , size_t> operators
{
	{ "*", 1 },
	{ "/", 1 },
	{ "%", 1 },
	{ "+", 2 },
	{ "-", 2 },
	{ "<<",3 },
	{ ">>",3 },
	{ "=", 10 },
	{ "*=", 10 },
	{ "/=", 10 },
	{ "%=", 10 },
	{ "+=", 10 },
	{ "-=", 10 },
	{ "<<=",10 },
	{ ">>=",10 },
};

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
	using namespace boost::algorithm;
	if ( tagVal != "$" )
	{
		std::vector<std::string> allSplit;

		split( text , std::back_inserter( allSplit ) ,
			( [ ] ( auto&& idx , auto&& iter )
		{ return iter[ 0 ] == '`' && ( idx == 0 ? true : iter[ -1 ] != '\\' ); } ) );

		for ( int i = 0; i < allSplit.size( ); i += 2 )
		{
			checkStr( allSplit[ i ] , "" );
		}
	}
}

#pragma region line_ast
using line_ast_node =
boost::make_recursive_variant<
	std::vector<boost::recursive_variant_> ,
	std::string>::type;
using line_ast = std::vector<line_ast_node>;
using bline_ast_node =
boost::make_recursive_variant<
	std::vector<boost::recursive_variant_> ,
	std::string ,
	std::shared_ptr<bint>>::type;
using bline_ast = std::vector<bline_ast_node>;

bline_ast line_ast_to_bline_ast( line_ast&& ast )
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

template<typename Iter>
class qc_line_ast_generator : public qi::grammar<Iter , line_ast( ) , ascii::space_type>
{
public:
	qc_line_ast_generator( ) :qc_line_ast_generator::base_type( line , "line" )
	{
		using qi::lit;
		using qi::digit;
		using qi::lexeme;
		using qi::on_error;
		using qi::fail;
		using ascii::alnum;
		using ascii::char_;
		using ascii::string;
		using ascii::space;
		using namespace qi::labels;
		using phoenix::at_c;

		using phoenix::push_back;
		using phoenix::construct;
		using phoenix::val;

		expr.name( "expr" );
		operator_.name( "operator" );
		line.name( "line" );

		operator_ %=
			string( "*" ) | string( "/" ) | string( "%" ) |
			string( "+" ) | string( "-" ) |
			string( "<<" ) | string( ">>" ) |
			string( "=" ) | string( "*=" ) | string( "/=" ) | string( "%=" ) | string( "+=" ) | string( "-=" ) | string( "<<=" ) | string( ">>=" );
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
#pragma endregion

std::shared_ptr<bint> evaluate( bline_ast& ast );

std::shared_ptr<bint> evaluate( std::string& str )
{
	boost::algorithm::trim( str );
	try
	{
		return std::shared_ptr<bint>( new bint( str ) );
	}
	catch ( ... )
	{
		try
		{
			return variables.at( str );
		}
		catch ( ... )
		{
			std::vector<std::string> splits;
			split( str , std::back_inserter( splits ) ,
				   [ ] ( auto&& , auto&& iter )
			{
				return ( boost::algorithm::is_space( )( *iter ) );
			} );
			if ( splits.size( ) != 2 )
			{
				throw throwVal( "Invalid expr: " + str , invalidExpr );
			}
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

void eval_op( std::string &op , bint &res , bint &prev , bint &post );

std::shared_ptr<bint> evaluate( bline_ast& ast )
{
	struct visitor :boost::static_visitor<std::shared_ptr<bint>>
	{
		forceinline std::shared_ptr<bint> operator()( bline_ast& lineast ) const
		{ return evaluate( lineast ); }
		forceinline std::shared_ptr<bint> operator()( std::shared_ptr<bint>& int_ ) const
		{ return int_; }
		forceinline std::shared_ptr<bint> operator()( std::string& str ) const
		{ return evaluate( str ); }
	} v;
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
			std::shared_ptr<bint>
				prev = boost::apply_visitor( v , ast[ pos - 1 ] ) ,
				post = boost::apply_visitor( v , ast[ pos + 1 ] ) ,
				res = std::shared_ptr<bint>( new bint( 0 ) );
			if ( !( prev&&post&&res ) )
			{
				for ( size_t i = idx; i < ordered_poss.size( ); i++ )
					if ( ordered_poss[ i ] > pos )
						ordered_poss[ i ] -= 2;
				ast[ pos - 1 ] = std::move( res );
				ast.erase( ast.begin( ) + pos , ast.begin( ) + pos + 2 );
				continue;
			}
			auto op = boost::get<std::string>( ast[ pos ] );
			eval_op( op , *res , *prev , *post );
			for ( size_t i = idx; i < ordered_poss.size( ); i++ )
				if ( ordered_poss[ i ] > pos )
					ordered_poss[ i ] -= 2;
			ast[ pos - 1 ] = std::move( res );
			ast.erase( ast.begin( ) + pos , ast.begin( ) + pos + 2 );
		}
	}
	else if ( ast.size( ) == 1 );
	else
	{
		std::shared_ptr<bint>
			prev = boost::apply_visitor( v , ast[ 0 ] ) ,
			post = boost::apply_visitor( v , ast[ 2 ] ) ,
			res = std::shared_ptr<bint>( new bint( 0 ) );
		if ( !( prev&&post&&res ) )
		{
			ast[ 0 ] = std::move( res );
			ast.erase( ast.begin( ) + 1 , ast.begin( ) + 3 );
			return boost::get<std::shared_ptr<bint>>( ast[ 0 ] );
		}
		auto op = boost::get<std::string>( ast[ 1 ] );
		eval_op( op , *res , *prev , *post );
		ast[ 0 ] = std::move( res );
		ast.erase( ast.begin( ) + 1 , ast.begin( ) + 3 );
	}
	return boost::apply_visitor( v , ast[ 0 ] );
}

void eval_op( std::string &op , bint &res , bint &prev , bint &post )
{
	/**/ if ( op == "*" ) res = prev * post;
	else if ( op == "/" ) res = prev / post;
	else if ( op == "%" ) res = prev % post;
	else if ( op == "+" ) res = prev + post;
	else if ( op == "-" ) res = prev - post;
	else if ( op == "<<" ) res = prev << static_cast< boost::int64_t >( post );
	else if ( op == ">>" ) res = prev >> static_cast< boost::int64_t >( post );
	else if ( op == "=" ) res = prev = post;
	else if ( op == "*=" ) res = prev *= post;
	else if ( op == "/=" ) res = prev /= post;
	else if ( op == "%=" ) res = prev %= post;
	else if ( op == "+=" ) res = prev += post;
	else if ( op == "-=" ) res = prev -= post;
	else if ( op == "<<=" ) res = prev <<= static_cast< boost::int64_t >( post );
	else if ( op == ">>=" ) res = prev >>= static_cast< boost::int64_t >( post );
}

std::shared_ptr<bint> fullEval( std::string_view splitStr )
{
	using boost::spirit::ascii::space;
	using qi::phrase_parse;
	using namespace std;

	if ( splitStr.substr( 0 , 5 ) == "print" )
	{
		auto notPrt = splitStr.substr( 6 );
		std::vector<std::string_view> splits;
		split_with_enable_disable(
			notPrt ,
			std::back_inserter( splits ) ,
			[ ] ( auto&& , auto&& iter ) { return *iter == ','; } ,
			[ ] ( auto&& i , auto&& iter )
		{
			return *iter == '(' ||
				( *iter == '`' && ( i != 0 ? *( iter - 1 ) != '\\' : true ) );
		} ,
			[ ] ( auto&& i , auto&& iter )
		{
			return *iter == ')' ||
				( *iter == '`' && ( i != 0 ? *( iter - 1 ) != '\\' : true ) );
		} );
		for ( size_t i = 0; i < splits.size( ); i++ )
		{
			if ( splits[ i ] == "endl" )
				std::cout << std::endl;
			else if ( splits[ i ][ 0 ] == '`' )
				std::cout << splits[ i ].substr( 1 , splits[ i ].size( ) - 2 );
			else
				std::cout << *fullEval( splits[ i ] );
		}
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
	vector<string> splitStrs;
	loop_split(
		text ,
		[ ] ( auto&& i , auto&& iter )
	{ return *iter == '\n' && ( i == 0 ? true : *( iter - 1 ) != '_' ); } ,
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
