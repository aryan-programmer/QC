#include "stdafx.h"
#include "qc_grammar.h"

inline qc_parser::qc_parser( int indent ) : indent( indent ) { }

struct qc_parser::qc_node_printer : boost::static_visitor<>
{
	qc_node_printer(
		int indent ,
		std::ostream& o ,
		const std::string& toLang ,
		const std::string& tagVal ,
		bool& isTagParsed ) :
		indent( indent ) , o { o } , toLang { toLang } , tagVal { tagVal } ,
		isTagParsed { isTagParsed }
	{ }

	void operator()( qc_data& qc ) const
	{
		qc_parser( indent + tabsize )( qc , o , toLang );
	}

	void operator()( std::string& text ) const
	{
		parseLang( o , indent - tabsize , tabsize , isTagParsed , text , tagVal , toLang );
	}

	int indent;
	std::ostream& o;
	const std::string& toLang;
	const std::string& tagVal;
	bool& isTagParsed;
};

void qc_parser::operator()( qc_data& qc , std::ostream& o , const std::string& toLang ) const
{
	auto qce = qc_data_extra { std::move( qc ),false };
	( *this )( qce , o , toLang );
}

void qc_parser::operator()( qc_data_extra& qc , std::ostream& o , const std::string& toLang ) const
{
	const std::string& tagVal = qc.qc.name;

	for ( auto& node : qc.qc.children )
		boost::apply_visitor( qc_node_printer( indent , o , toLang , tagVal , qc.tagParsed ) , node );

	if ( tagVal == "func" )
		tab( indent - tabsize * 2 , o );
	if ( tagVal != "native"&&tagVal != "$"&&tagVal != "|qc|" )
		o << '}' << std::endl;
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

	using phoenix::construct;
	using phoenix::val;

	text %= lexeme[ +( char_ - '<' ) ];
	node %= qc | text;

	start_tag %=
		'<'
		>> !lit( '/' )
			> lexeme[ +( char_ - '>' ) ]
			> '>'
		;

	end_tag =
		( "</>" ) |
		( "</"
			> string( _r1 )
			> '>' )
		;

	qc %=
		start_tag[ _a = _1 ]
			> *node
			> end_tag( _a )
		;

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
			<< construct<std::string>( _3 , _2 )   // iterators to error-pos, end
			<< val( "\"" )
			<< std::endl
			);
}
