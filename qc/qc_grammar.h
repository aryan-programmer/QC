#pragma once

namespace fusion = boost::fusion;
namespace phx = boost::phoenix;
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

///////////////////////////////////////////////////////////////////////////
//  QC tree representation
///////////////////////////////////////////////////////////////////////////
struct qc_data;
enum class tags;
using qc_data_node = boost::variant<boost::recursive_wrapper<qc_data> , std::string>;

struct qc_data
{
	tags name;               // tag name
	std::vector<qc_data_node> children;        // children
};

struct qc_data_extra
{
	qc_data qc;
	bool tagParsed = false;
};

BOOST_FUSION_ADAPT_STRUCT(
	qc_data ,
	( tags , name )
	( std::vector<qc_data_node> , children )
)

///////////////////////////////////////////////////////////////////////////
//  Print out the QC tree
///////////////////////////////////////////////////////////////////////////
struct qc_parser
{
	void operator()( qc_data& qc , std::ostream& o , const std::string& toLang ) const;
private:
	void operator()( qc_data_extra& qc , std::ostream& o = std::cout , const std::string& toLang = "" ) const;
	struct qc_node_printer;
};

///////////////////////////////////////////////////////////////////////////
//  QC grammar definition
///////////////////////////////////////////////////////////////////////////
struct qc_grammar
	: qi::grammar<std::string::const_iterator , qc_data( ) , qi::locals<tags> , ascii::space_type>
{
	qc_grammar( );

	qi::rule<std::string::const_iterator , qc_data( ) , qi::locals<tags> , ascii::space_type> qc;
	qi::rule<std::string::const_iterator , qc_data_node( ) , ascii::space_type> node;
	qi::rule<std::string::const_iterator , std::string( ) , ascii::space_type> text;
	qi::rule<std::string::const_iterator , tags( ) , ascii::space_type> start_tag;
	qi::rule<std::string::const_iterator , void( tags ) , ascii::space_type> end_tag;
};
