#include "stdafx.h"
#include "qc_grammar.h"

const std::unordered_map<std::string , std::string> shorthandsAndOfficialKeywords
{
	{"C", "class" },
	{"S", "struct" },
	{"I", "interface" },
	{"P", "property" },
	{"F", "function" },
	{"W", "while" },
	{"?", "class" },
	{"!", "struct" },
	{"^", "interface" },
	{"=", "property" },
	{"()", "function" },
	{"[]", "indexer" },
	{"in", "foreach" },
	{"idxr", "indexer" },
	{"elif", "else if" },
	{"func", "function" },
	{"prop", "property" },
};


struct qc_parser::qc_node_printer : boost::static_visitor<>
{
	qc_node_printer(
		std::ostream& o ,
		const std::string& toLang ,
		const std::string& tagVal ,
		bool& isTagParsed ) :
		o { o } , toLang { toLang } , tagVal { tagVal } ,
		isTagParsed { isTagParsed }
	{ }

	void operator()( qc_data& qc ) const
	{
		if ( !isTagParsed ) { std::string s; ( *this )( s ); }
		qc_parser( )( qc , o , toLang );
	}

	void operator()( std::string& text ) const
	{
		parseLang( o , isTagParsed , text , tagVal , toLang );
	}

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
	using namespace std;

	auto& mutTagVat = qc.qc.name;
	try { mutTagVat = shorthandsAndOfficialKeywords.at( mutTagVat ); }
	catch ( ... ) { }
	const string& tagVal = qc.qc.name;

	for ( auto& node : qc.qc.children )
	{
		boost::apply_visitor( qc_node_printer( o , toLang , tagVal , qc.tagParsed ) , node );
	}


	if ( tagVal != "native"&&tagVal != "$"&&tagVal != "|qc|" )
	{
		--indentLevel;
		o << indent << '}' << endl;
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
