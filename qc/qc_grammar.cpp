#include "stdafx.h"
#include "qc_grammar.h"

static const std::unordered_map<std::string , std::string> languageExtensionDict
{
	{"", ".parsedqc"},
	{"C#",".cs" }
};
static const std::unordered_map<char , char> openCloseBrace
{
	{ '<','>' },
	{ '(',')' },
	{ '[',']' },
	{ '{','}' },
	{ '>','<' },
	{ ')','(' },
	{ ']','[' },
	{ '}','{' },
};

inline qc_parser::qc_parser( int indent ) : indent( indent ) { }

struct qc_parser::qc_node_printer : boost::static_visitor<>
{
	qc_node_printer(
		int indent ,
		std::ostream& o ,
		const std::string& toLang ,
		const std::string& tagVal ,
		bool& isTagParsed ) :
		indent( indent ) , o{ o } , toLang{ toLang } , tagVal{ tagVal } ,
		isTagParsed{ isTagParsed }
	{ }

	void operator()( qc_data const& qc ) const
	{
		qc_parser( indent+tabsize )( qc , o , toLang );
	}

	void operator()( std::string const& text ) const
	{
		if(toLang=="")
		{
			tab( indent , o );
			o<<"text: \""<<text<<'"'<<std::endl;
		}
		else
		{
			parseLang( o , indent , tabsize , isTagParsed , text , tagVal , toLang );
		}
	}

	int indent;
	std::ostream& o;
	const std::string& toLang;
	const std::string& tagVal;
	bool& isTagParsed;
};

void qc_parser::operator()( qc_data const& qc , std::ostream& o , const std::string& toLang ) const
{
	auto qce = qc_data_extra{ std::move( qc ),false };
	( *this )( qce , o , toLang );
}

void qc_parser::operator()( qc_data_extra& qc , std::ostream& o , const std::string& toLang ) const
{
	if(toLang=="")
	{
		tab( indent , o );
		o<<"tag: "<<qc.qc.name<<std::endl;
		tab( indent , o );
		o<<'{'<<std::endl;
	}
	const std::string& tagVal = qc.qc.name;

	for(const auto& node:qc.qc.children)
		boost::apply_visitor( qc_node_printer( indent , o , toLang , tagVal , qc.tagParsed ) , node );

	if(toLang=="")
		tab( indent , o );
	if(tagVal=="func")
		tab( indent-tabsize , o );
	if(tagVal!="native"&&tagVal!="$")
		o<<'}'<<std::endl;
}

qc_grammar::qc_grammar() :qc_grammar::base_type( qc , "qc" )
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

	text %= lexeme[ +( char_-'<' ) ];
	node %= qc|text;

	start_tag %=
		'<'
		>>!lit( '/' )
			>lexeme[ +( char_-'>' ) ]
			>'>'
		;

	end_tag =
		( "</>" )|
		( "</"
			>string( _r1 )
			>'>' )
		;

	qc %=
		start_tag[ _a = _1 ]
			>*node
			>end_tag( _a )
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
			<<val( "Error! Expecting " )
			<<_4                               // what failed?
			<<val( " here: \"" )
			<<construct<std::string>( _3 , _2 )   // iterators to error-pos, end
			<<val( "\"" )
			<<std::endl
			);
}
