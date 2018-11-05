#include "stdafx.h"
#include "tag_block_manager.h"
#include "qc_grammar.h"
#include <boost/phoenix/function/adapt_function.hpp>

std::string __GetEndString( tags tag )
{ return std::string( to_string( tag , "QC" , true ) ); }
BOOST_PHOENIX_ADAPT_FUNCTION( std::string , GetEndStringParser , __GetEndString , 1 );

struct qc_parser::qc_node_printer : boost::static_visitor<>
{
	qc_node_printer(
		std::ostream& o , const std::string& toLang ,
		const tags& tagVal , bool& isTagParsed , bool& isDoLoop ) :
		o { o } , toLang { toLang } , tagVal { tagVal } ,
		isTagParsed { isTagParsed } , isDoLoop { isDoLoop } { }

	void operator()( qc_data& qc ) const
	{
		if ( !isTagParsed ) { std::string s; ( *this )( s ); }
		isDoLoop = qc.name == tags::_Do_;
		qc_parser( )( qc , o , toLang );
	}

	void operator()( std::string& text ) const
	{ parseLang( o , isTagParsed , text , tagVal , toLang ); }

	std::ostream& o;
	const std::string& toLang;
	const tags& tagVal;
	bool& isTagParsed;
	bool& isDoLoop;
};

void qc_parser::operator()( qc_data& qc , std::ostream& o , const std::string& toLang ) const
{
	auto qce = qc_data_extra { std::move( qc ),false };
	( *this )( qce , o , toLang );
	qc = std::move( qce.qc );
}

void qc_parser::operator()( qc_data_extra& qc , std::ostream& o , const std::string& toLang ) const
{
	const auto& tagVal = qc.qc.name;
	bool doesDoLoopNeedParsing = false;
	for ( auto& node : qc.qc.children )
	{
		if ( doesDoLoopNeedParsing )
		{
			auto val = WriteDoLoop(
				boost::get<std::string>( node ) ,
				o ,
				toLang );
			doesDoLoopNeedParsing = false;
			qc_node_printer( o , toLang , tagVal , qc.tagParsed , doesDoLoopNeedParsing )(
				val );
		}
		else boost::apply_visitor( qc_node_printer( o , toLang , tagVal , qc.tagParsed , doesDoLoopNeedParsing ) , node );
	}

	if ( tagVal != tags::_Native_ && tagVal != tags::_QC_ )
	{
		--indentLevel;
		o << std::endl << indent << to_string( tagVal , toLang , true );
		if ( qc.qc.name != tags::_Do_ )o << std::endl;
	}
}

qc_grammar::qc_grammar( ) :qc_grammar::base_type( qc , "qc" )
{
	using qi::lit;
	using qi::lexeme;
	using qi::on_error;
	using qi::fail;
	using ascii::char_;
	using ascii::string;
	using namespace qi::labels;
	using phx::val;

	text %= lexeme[ +( char_ - '<' ) ];
	node %= qc | text;

	start_tag %= '<' >> !lit( '/' ) > ( DEFINE_PARSER ) > '>';
	end_tag = '<' > string( GetEndStringParser( _r1 ) ) > '>';
	qc %= start_tag[ _a = _1 ] > *node > end_tag( _a );

	qc.name( "xml" );
	node.name( "node" );
	text.name( "text" );
	start_tag.name( "start_tag" );
	end_tag.name( "end_tag" );

	on_error<fail>
		(
			qc
			, std::cout
			<< val( "Error! Expecting " )
			<< _4                               // what failed?
			<< val( " here: \"" )
			<< phx::construct<std::string>( _3 , _2 )   // iterators to error-pos, end
			<< val( "\"" )
			<< std::endl
			);
}
