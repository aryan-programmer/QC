#include "stdafx.h"
#include "utils.h"
#include "cross_utils.h"
#include "qc_parser_runner.h"
#include <sstream>
#include <boost\stacktrace.hpp>

static const std::unordered_map<std::string , std::string> languageExtensionDict
{
	{"", ".parsedqc"},
	{"CS",".cs" }
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
	"CS"
};


auto __indent::operator()( size_t indentLevel )-> _indent { return _indent { indentLevel }; }

std::ostream& operator<<( std::ostream& o , __indent::_indent&& v )
{
	for ( size_t i = 0; i < v.indentLevel; i++ )o << '\t';
	return o;
}
std::ostream& operator<<( std::ostream& o , __indent )
{
	for ( size_t i = 0; i < indentLevel; i++ )o << '\t';
	return o;
}

void parseLang(
	std::ostream& o ,
	bool& isTagParsed ,
	std::string & text ,
	const std::string & tagVal ,
	const std::string & toLang )
{
	using namespace std;
	auto
		isComment = tagVal == "$" ,
		isRoot = tagVal == "|qc|" ,
		isInterface = tagVal == "interface" ,
		isNative = tagVal == "native" ,
		isSkip = false;
	auto idx = ( isTagParsed || isComment ) ? 0 : getFirstNewline( text );
	string subVal;
	if ( idx == string::npos )
		idx = text.length( );
	subVal = text.substr( 0 , idx );
	bool wasTagParsed = isTagParsed;
	auto semicolon = ( subVal.length( ) != 0 ? ";" : "" );
	boost::trim( subVal );
	if ( !isTagParsed )
		o << indent;

	if ( tagVal == "class" || tagVal == "struct" || tagVal == "namespace" )
	{
		convStr( subVal , toLang );
		if ( !isTagParsed ) o << tagVal << ' ' << subVal;
	}
	else if ( isInterface )
	{
		if ( !isTagParsed )
		{
			convStr( subVal , toLang );
			o << "interface " << subVal;
		}
	}
	else if ( tagVal == "property" )
	{
		if ( !isTagParsed )
		{
			convStr( subVal , toLang );
			auto ofIdx = boost::find_first( subVal , " of " ).begin( );
			o <<
				string( ofIdx + 4 , subVal.end( ) ) <<
				' ' <<
				subVal.substr( 0 , ofIdx - subVal.begin( ) );
		}
	}
	else if ( tagVal == "indexer" )
	{
		if ( !isTagParsed )
		{
			convStr( subVal , toLang , false );
			auto ofIdx = boost::find_first( subVal , " of " ).begin( );
			o <<
				string( ofIdx + 4 , subVal.end( ) ) <<
				" this" <<
				subVal.substr( 0 , ofIdx - subVal.begin( ) );
		}
	}
	else if ( tagVal == "unsafe" || tagVal == "unchecked" || tagVal == "checked" || tagVal == "get" || tagVal == "set" || tagVal == "else" || tagVal == "try" || tagVal == "finally" || tagVal == "do" )
	{
		convStr( subVal , toLang );
		if ( !isTagParsed ) o << tagVal << endl << indent << '{' << endl;
		isSkip = true;
		if ( subVal != "" )
			o << indent( indentLevel + 1 ) << subVal << semicolon << endl;
	}
	else if ( isComment || isRoot || isNative ) isSkip = true;
	else if ( tagVal == "function" )
	{
		using namespace boost::algorithm;

		if ( !isTagParsed )
		{
			convStr( subVal , toLang , false );
			if ( boost::contains( subVal , " gives " ) )
			{
				auto givesIdx = boost::find_first( subVal , " gives " ).begin( );
				o <<
					string( givesIdx + 7 , subVal.end( ) ) <<
					' ' <<
					subVal.substr( 0 , givesIdx - subVal.begin( ) );
			}
			else o << "void " << subVal;
		}
	}
	else if ( tagVal == "forever" )
	{
		if ( !isTagParsed )
		{
			o << "while(true) ";
		}
	}
	else
	{
		if ( !isTagParsed )
		{
			convStr( subVal , toLang );
			o << tagVal << '(' << subVal << ")";
		}
	}

	if ( !isTagParsed )
	{
		if ( !isSkip )
			o << endl << indent << '{' << endl;
		if ( !( isComment || isRoot || isNative ) )
			++indentLevel;
		isTagParsed = true;
	}

	if ( text == "" || isspace( text[ 0 ] ) );
	else if ( isComment )o << "/*" << text << "*/" << endl;
	else if ( isNative )o << text << endl;
	else if ( isRoot ) convertTextToLang( o , text , toLang );
	else if ( isInterface )
	{
		auto _text = wasTagParsed ? "" : text.substr( idx );
		auto& vText = wasTagParsed ? text : _text;
		replaceAngleBrace( vText );
		checkStr( vText , toLang );
		convStr( vText , toLang , false );
		loop_split(
			string_view( vText ) ,
			[ ] ( auto&& i , auto&& iter )
			{ return *iter == '\n' && !( i == 0 ? false : *( iter - 1 ) == '_' ); } ,
			[ &o , &semicolon ] ( string_view subVal )
			{
				if ( boost::contains( subVal , " gives " ) )
				{
					auto givesIdx = boost::find_first( subVal , " gives " ).begin( );
					o <<
						string( givesIdx + 7 , subVal.end( ) ) <<
						' ' <<
						subVal.substr( 0 , givesIdx - subVal.begin( ) ) <<
						semicolon << endl;
				}
				else
					o << "void " << subVal << semicolon << endl;
			}
		);
	}
	else
	{
		auto _text = wasTagParsed ? "" : text.substr( idx );
		auto& vText = wasTagParsed ? text : _text;
		convertTextToLang( o , vText , toLang );
	}
}

void commonReplacement( std::string & val )
{
	using namespace boost::algorithm;

	replace_all( val , "less or eq" , "<=" );
	replace_all( val , "more or eq" , ">=" );
	replace_all( val , "not equals" , "!=" );
	replace_all( val , "equals" , "==" );
	replace_all( val , "less" , "<" );
	replace_all( val , "more" , ">" );
	replace_all( val , "lambda" , "=>" );
	replace_all( val , "left shift set" , "<<=" );
	replace_all( val , "right shift set" , ">>=" );
	replace_all( val , "left shift" , "<<" );
	replace_all( val , "right shift" , ">>" );
}

void convStr( std::string & val , const std::string & toLang , bool processNewLines )
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
	if ( processNewLines )
	{
		replace_all( val , "\n" , ";\n" );
		replace_all( val , "_;" , "" );
		replace_all( val , " ;" , ";" );
		replace_all( val , "\t;" , ";" );
		replace_all( val , "\n;" , "\n" );
	}
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
				loop_split(
					val ,
					[ ] ( auto&& idx , auto&& iter ) { return *iter == '\n'; } ,
					[ &o ] ( std::string&& val )
					{
						trim( val );
						if ( val.size( ) == 0 )return;
						o << indent << val << std::endl;
					}
				);
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
	bool indent = vm.count( "indent" );

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
			++indentLevel;
			if ( is_regular_file( arg ) )
				parse_file( arg , toLang , indent );
			else if ( is_directory( arg ) )
				traversePath( arg , toLang , indent );
			--indentLevel;

			cout << "Full parsing succeeded" << endl;

		}
		catch ( throwVal& val )
		{

			--indentLevel;
			cout << "Full parsing failed" << endl;

			return val.errCode;
		}
	}
	return 0;
}

void traversePath( boost::filesystem::path &arg , std::string &toLang , bool indent )
{
	using namespace std;
	using namespace std::string_literals;
	using boost::filesystem::current_path;
	using boost::filesystem::path;
	using boost::filesystem::directory_iterator;

	auto truncname = arg.filename( ).string( );

	cout << indent << truncname << " Dir. parsing started" << endl;

	++indentLevel;
	try
	{
		for ( directory_iterator first( arg ) , end; first != end; first++ )
		{
			path narg = first->path( );
			if ( is_regular_file( narg ) )parse_file( narg , toLang , indent );
			else if ( is_directory( narg ) )traversePath( narg , toLang , indent );
			else
				throw throwVal( "Path is neither a file not a directory" , notFileNorDir );
		}
		cout << ::indent << truncname << " Dir. parsing succeeded" << endl;
		--indentLevel;
	}
	catch ( throwVal& )
	{
		--indentLevel;
		cout << ::indent << truncname << " Dir. parsing failed" << endl;
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

void parse_file( boost::filesystem::path &filename , std::string &toLang , bool indent )
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

	cout << ::indent << truncname << " File parsing started" << endl;

	parse_file( filename , storage , iter , end , succeeded , qc , ast );

	if ( succeeded && iter == end )
	{
		auto csfile = filename.replace_extension( languageExtensionDict.at( toLang ) ).string( );
		struct __end__
		{
			size_t indent;
			__end__( ) :indent( indentLevel ) { indentLevel = 0; }
			void restore( ) { this->~__end__( ); }
			~__end__( ) { indentLevel = indent; }
		} __end__;
		ofstream o( csfile );
		qc_parser parser;
		try
		{
			parser( ast , o , toLang );
			__end__.restore( );
			cout << ::indent << truncname << " File parsing succeeded" << endl;
		}
		catch ( throwVal& )
		{
			cout << ::indent << truncname << " File parsing failed" << endl;
			throw;
		}
	}
	else
	{
		cout << ::indent << truncname << " File parsing failed" << endl;
		throw throwVal( "File parsing failed" , parsingFailed );
	}
}

inline throwVal::throwVal( const std::string& first , int errCode ) :errCode( errCode ) { std::cerr << indent( indentLevel + 1 ) << "Error: " << first << std::endl; }
