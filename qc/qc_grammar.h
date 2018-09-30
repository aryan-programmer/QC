#pragma once

namespace fusion = boost::fusion;
namespace phoenix = boost::phoenix;
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

///////////////////////////////////////////////////////////////////////////
//  QC tree representation
///////////////////////////////////////////////////////////////////////////
struct qc_data;

using qc_data_node =
boost::variant<boost::recursive_wrapper<qc_data> , std::string>;

struct qc_data
{
	std::string name;                           // tag name
	std::vector<qc_data_node> children;        // children
};

struct qc_data_extra
{
	qc_data qc;
	bool tagParsed = false;
};

// We need to tell fusion about our qc_data struct
// to make it a first-class fusion citizen
BOOST_FUSION_ADAPT_STRUCT(
	qc_data ,
	( std::string , name )
	( std::vector<qc_data_node> , children )
)

///////////////////////////////////////////////////////////////////////////
//  Print out the QC tree
///////////////////////////////////////////////////////////////////////////

struct qc_parser
{
	qc_parser() :indent( 0 ) { }

	void operator()( qc_data const& qc , std::ostream& o , const std::string& toLang ) const;

private:
	enum { tabsize = 4 };

	qc_parser( int indent );

	int indent;

	void operator()( qc_data_extra& qc , std::ostream& o = std::cout , const std::string& toLang = "" ) const;

	struct qc_node_printer;
};

///////////////////////////////////////////////////////////////////////////
//  QC grammar definition
///////////////////////////////////////////////////////////////////////////
struct qc_grammar
	: qi::grammar<std::string::const_iterator , qc_data() , qi::locals<std::string> , ascii::space_type>
{
	qc_grammar();

	qi::rule<std::string::const_iterator , qc_data() , qi::locals<std::string> , ascii::space_type> qc;
	qi::rule<std::string::const_iterator , qc_data_node() , ascii::space_type> node;
	qi::rule<std::string::const_iterator , std::string() , ascii::space_type> text;
	qi::rule<std::string::const_iterator , std::string() , ascii::space_type> start_tag;
	qi::rule<std::string::const_iterator , void( std::string ) , ascii::space_type> end_tag;
};
