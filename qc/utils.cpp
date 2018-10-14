#include "stdafx.h"
#include "utils.h"
#include "cross_utils.h"
#include "qc_grammar.h"
#include "qc_parser_runner.h"

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
	{ '{','}' }
};

static const std::set<char> allOpenBraces { '<','(','[','{' };
static const std::set<char> allCloseBraces { '>',')',']','}' };
static const std::set<std::string> supportedLanguages
{
	"",
	"qc",
	"C#"
};

void indentialTab( std::ostream& o ) { for ( size_t i = 0; i < indent * 4; i++ )o << " "; }

void parseLang(
	std::ostream& o , int indent , int tabsize ,
	bool& isTagParsed ,
	std::string & text ,
	const std::string & tagVal ,
	const std::string & toLang )
{
	auto isComment = tagVal == "$" , isRoot = tagVal == "|qc|";
	auto idx = ( isTagParsed || isComment ) ? 0 : getFirstNewline( text );
	if ( idx == std::string::npos )idx = text.length( );
	if ( tagVal == "class" || tagVal == "struct" || tagVal == "interface" || tagVal == "namespace" )
	{
		if ( !isTagParsed )
		{
			isTagParsed = true;
			o << tagVal << ' ' << text.substr( 0 , idx ) << "{";
		}
		else
		{
			o << text.substr( 0 , idx );
			tab( indent + tabsize , o );
		}
	}
	else if ( tagVal == "unsafe" || tagVal == "unchecked" )
	{
		if ( !isTagParsed )
		{
			isTagParsed = true;
			o << tagVal << " {";
		}
		else
		{
			o << text.substr( 0 , idx );
			tab( indent + tabsize , o );
		}
	}
	else if ( isComment || isRoot )
	{
		if ( !isTagParsed ) isTagParsed = true;
	}
	else if ( tagVal == "func" )
	{
		using namespace boost::algorithm;


		if ( !isTagParsed )
		{
			isTagParsed = true;

			auto givesIdx = boost::find_first( text , " gives " ).begin( );
			std::string retVal;
			std::copy( givesIdx + 7 , text.begin( ) + idx , std::back_inserter( retVal ) );
			std::string val = retVal + ' ' + text.substr( 0 , givesIdx - text.begin( ) );
			convStr( val , toLang );
			o << val << "{";
		}
		else tab( indent + tabsize , o );
	}
	else
	{
		if ( !isTagParsed && ( tagVal != "native" ) )
		{
			o << tagVal;
			isTagParsed = true;
			o << '(' << text.substr( 0 , idx ) << "){";
		}
		else
		{
			o << text.substr( 0 , idx );
			tab( indent + tabsize , o );
		}
	}

	if ( isComment )o << "/*" << text << "*/" << std::endl;
	else if ( isRoot )
	{
		convertTextToLang( o , text , toLang );
	}
	else
	{
		auto restText = text.substr( idx );
		convertTextToLang( o , restText , toLang );
	}
}

void commonReplacement( std::string & val )
{
	using namespace boost::algorithm;

	replace_all( val , "less or eq" , "<=" );
	replace_all( val , "more or eq" , ">=" );
	replace_all( val , "equals" , "==" );
	replace_all( val , "not equals" , "!=" );
	replace_all( val , "less" , "<" );
	replace_all( val , "more" , ">" );
	replace_all( val , "left shift" , "<<" );
	replace_all( val , "right shift" , ">>" );
	replace_all( val , "left shift set" , "<<=" );
	replace_all( val , "right shift set" , ">>=" );
}

void convStr( std::string & val , const std::string & toLang )
{
	using namespace boost::algorithm;

	replaceAngleBrace( val );
	replace_all( val , "also give" , "yield return" );
	replace_all( val , "finalize" , "yield break" );
	replace_all( val , "give" , "return" );
	replace_all( val , "gen_of" , "System.Collections.Generic.IEnumerable" );
	replace_all( val , "iter gen_of" , "System.Collections.Generic.IEnumerator" );
	replace_all( val , "gen" , "System.Collections.IEnumerable" );
	replace_all( val , "iter gen" , "System.Collections.IEnumerator" );
	commonReplacement( val );
	replace_all( val , "\b" , ";\b" );
	replace_all( val , "_;" , "" );
	replace_all( val , "\b;" , "\b" );
	if ( val[ 0 ] == ';' ) val = val.substr( 1 );
}

void replaceAngleBrace( std::string & val )
{
	boost::algorithm::replace_all( val , "{{" , "<" );
	boost::algorithm::replace_all( val , "}}" , ">" );
}

void checkStr( const std::string_view & val , const std::string & toLang )
{
	using namespace std::string_literals;
	std::list<char> closeBraces;
	size_t idx = 0;
	for ( auto first = val.begin( ) , end = val.end( );
		  first != end;
		  first++ , idx++ )
	{
		auto& chr = *first;
		if ( allOpenBraces.find( chr ) != allOpenBraces.end( ) )
		{
			// chr is an open brace, lets add it to the checking list, closeBraces.
			closeBraces.push_back( openCloseBrace.at( chr ) );
		}
		else
		{
			// chr is not an open brace, but maybe a close brace.
			if ( allCloseBraces.find( chr ) != allCloseBraces.end( ) && closeBraces.size( ) >= 1 )
			{
				auto expectedCloseBrace = closeBraces.back( );
				if ( expectedCloseBrace != chr )
				{
					// chr is a close brace, but not the correct one,
					// so the code is incorrect.
					auto loc = val.substr(
						val.find_last_of( '\n' , idx + 1 ) + 1 ,
						getFirstNewline( val , idx ) );
					throw throwVal( (
						"Expected \""s +
						expectedCloseBrace +
						"\"at \"\b" +
						std::string( loc ) +
						"\b\",\b got " +
						chr ) , braceMismatch );
				}
				else
				{
					// chr is the correct close brace.
					closeBraces.pop_back( );
				}
			}
			else
			{
				// chr is neither an open brace, nor a close brace.
				continue;
			}
		}
	}
	std::string errStr;
	for ( auto& chr : closeBraces )
	{
		auto loc = val.substr(
			val.find_last_of( '\n' , val.find_last_of( '\n' ) - 1 ) + 1 );
		errStr +=
			"Expected \""s +
			chr +
			"\"at \"\b" +
			std::string( loc ) +
			"\b\",\b got nothing.\b";
	}
	if ( errStr != "" ) throw throwVal( errStr , braceMismatch );
}

void convertTextToLang( std::ostream& o , std::string & text , const std::string & toLang )
{
	using namespace boost::algorithm;

	replace_all( text , "\\|]~" , ">" );
	replace_all( text , "\\~[|" , "<" );

	size_t idx = 0;
	loop_split(
		text ,
		[ ] ( auto&& idx , auto&& iter ) { return isStrQuote( idx , iter ); } ,
		[ &idx , &toLang , &o ] ( std::string&& val )
		{
			bool isEven = ( idx % 2 ) == 0;
			replace_all( val , "\\`" , "`" );
			if ( isEven )
			{
				replaceAngleBrace( val );
				checkStr( val , toLang );
				convStr( val , toLang );
				o << val;
			}
			else
			{
				replace_all( val , R"(")" , R"("")" );
				o << R"(@")" << val << R"(")";
			}
			idx++;
		} );
}

size_t getFirstNewline( const std::string_view & val , size_t off )
{
	using namespace boost::algorithm;
	auto pos = val.find_first_of( '\n' , off );
	while ( true )
	{
		if ( pos == 0 ) return pos;
		auto underscore = val.find_last_of( "_" , pos );
		if ( pos == ( underscore + 1 ) )pos = val.find_first_of( '\n' , pos + 1 );
		else return pos;
	}
}

int parseArgs( boost::program_options::variables_map vm )
{
	using namespace std;
	using namespace std::string_literals;
	using boost::filesystem::current_path;
	using boost::filesystem::path;
	using boost::filesystem::directory_iterator;

	if ( !vm.count( "file" ) )
	{
		cerr << "No input file given" << endl;
		return noInpFileOrDir;
	}

	path curr = workingDir( );

	path arg;
	auto filePath = vm[ "file" ].as<std::string>( );
	arg = filePath;
	if ( !arg.is_absolute( ) )
	{
		arg = curr;
		arg /= filePath;
	}

	string toLang( ( !vm.count( "file" ) ) ? "qc" : vm[ "language" ].as<std::string>( ) );

	if ( supportedLanguages.find( toLang ) == supportedLanguages.end( ) )
	{
		auto v = throwVal( "Language \"" + toLang + "\" is not presently supported" , unsupportedLanguage ).errCode;
		return v;
	}

	if ( toLang == "" || toLang == "qc" )
	{
		try { pcr_file( arg ); }
		catch ( throwVal& val )
		{
			return val.errCode;
		}
	}
	else
	{
		cout << "Full parsing started" << endl;

		try
		{
			indent++;
			if ( is_regular_file( arg ) )
				parse_file( arg , toLang );
			else if ( is_directory( arg ) )
				traversePath( arg , toLang );
			indent--;

			cout << "Full parsing succeeded" << endl;

		}
		catch ( throwVal& val )
		{

			indent--;
			cout << "Full parsing failed" << endl;

			return val.errCode;
		}
	}
	return 0;
}

void traversePath( boost::filesystem::path &arg , std::string &toLang )
{
	using namespace std;
	using namespace std::string_literals;
	using boost::filesystem::current_path;
	using boost::filesystem::path;
	using boost::filesystem::directory_iterator;

	auto truncname = arg.filename( ).string( );

	indentialTab( cout );
	cout << truncname << " Dir. parsing started" << endl;

	indent++;
	try
	{
		for ( directory_iterator first( arg ) , end; first != end; first++ )
		{
			path narg = first->path( );
			if ( is_regular_file( narg ) )parse_file( narg , toLang );
			else if ( is_directory( narg ) )traversePath( narg , toLang );
			else
				throw throwVal( "Path is neither a file not a directory" , notFileNorDir );
		}
		indentialTab( cout );
		cout << truncname << " Dir. parsing succeeded" << endl;
		indent--;
	}
	catch ( throwVal& )
	{
		indent--;
		indentialTab( cout );
		cout << truncname << " Dir. parsing failed" << endl;
		throw;
	}
}

void parse_file(
	boost::filesystem::path& filename ,
	std::string &storage ,
	std::string::const_iterator &iter ,
	std::string::const_iterator &end ,
	bool &succeed ,
	qc_grammar &qc ,
	qc_data &ast )
{
	using namespace std;

	std::ifstream in( filename.string( ) , ios_base::in );

	if ( !in )
	{
		throw throwVal( "Could not open input file: " + filename.string( ) , unableToOpenFile );
	}

	in.unsetf( ios::skipws ); // No white space skipping!
	in.seekg( 0 , ios::end );
	storage.reserve( in.tellg( ) );
	in.seekg( 0 , ios::beg );
	copy(
		istream_iterator<char>( in ) ,
		istream_iterator<char>( ) ,
		back_inserter( storage ) );

	using boost::spirit::ascii::space;
	iter = storage.begin( );
	end = storage.end( );
	succeed = phrase_parse( iter , end , qc , space , ast );
}

void parse_file( boost::filesystem::path &filename , std::string &toLang )
{
	if ( filename.extension( ) != ".qc" ) return;

	using namespace std;
	using namespace std::string_literals;

	bool succeeded;
	string::const_iterator iter , end;
	qc_grammar qc;
	qc_data ast;
	string storage; // We will read the contents here.

	auto truncname = filename.filename( ).string( );

	indentialTab( cout );
	cout << truncname << " File parsing started" << endl;

	parse_file( filename , storage , iter , end , succeeded , qc , ast );

	if ( succeeded && iter == end )
	{
		qc_parser parser;
		std::ofstream o( filename.replace_extension( languageExtensionDict.at( toLang ) ).string( ) );
		try
		{
			parser( ast , o , toLang );
			indentialTab( cout );
			cout << truncname << " File parsing succeeded" << endl;
		}
		catch ( throwVal& )
		{
			indentialTab( cout );
			cout << truncname << " File parsing failed" << endl;
			throw;
		}
	}
	else
	{
		indentialTab( cout );
		cout << truncname << " File parsing failed" << endl;
		throw throwVal( "File parsing failed" , parsingFailed );
	}
}

inline throwVal::throwVal( const std::string& first , int errCode ) :errCode( errCode )
{
	indent++;
	indentialTab( std::cout );
	std::cerr << "Error: " << first << std::endl;
	indent--;
}
