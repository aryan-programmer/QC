#pragma once

static constexpr int unknownError = -2;
static constexpr int invalidCommandLineParameters = -1;
static constexpr int noInpFileOrDir = 1;
static constexpr int notFileNorDir = 2;
static constexpr int tagMismatch = 3;
static constexpr int braceMismatch = 4;
static constexpr int unableToOpenFileForInput = 5;
static constexpr int unableToOpenFileForOutput = 6;
static constexpr int parsingFailed = 7;
static constexpr int unsupportedLanguage = 8;
static constexpr int invalidExpr = 9;
static constexpr int invalidTypes = 10;
static constexpr int invalidLoopSyntax = 11;
static constexpr int propertyNotSupported = 12;
static constexpr int indexerNotSupported = 13;
static constexpr int getSetNotSupported = 14;
static constexpr int finallyNotSupported = 15;
static constexpr int invalidPropertySyntax = 16;
static constexpr int invalidIndexerSyntax = 17;

struct __indent
{
	struct _indent { size_t indentLevel; };
	_indent operator()( size_t indentLevel );
};
static __indent indent;
std::ostream& operator<<( std::ostream& o , __indent::_indent&& v );
std::ostream& operator<<( std::ostream& o , __indent );

void parseLang( std::ostream& o , bool& isTagParsed , std::string & text , const tags & tagVal , const std::string & toLang );
void commonReplacement( std::string & val );
void convStr( std::string & val , const std::string & toLang , bool processNewLines = true , bool applyException = false );
void replaceAngleBrace( std::string & val );
void checkStr( const std::string_view & val , const std::string & toLang );
std::string WriteDoLoop( std::string& val , std::ostream& o , const std::string & toLang );
void convertTextToLang( std::ostream& o , std::string & text , const std::string& toLang , bool doIndent = true , bool processNewLines = true );
size_t getFirstNewline( const std::string_view& val , size_t off = 0 );
int parseArgs( boost::program_options::variables_map vm );

void traversePath( boost::filesystem::path &arg , std::string &toLang , bool indent );

void parse_file( boost::filesystem::path &filename , std::string &toLang , bool indent );
void parse_file( boost::filesystem::path & filename , std::string &storage , std::string::const_iterator &iter , std::string::const_iterator &end , bool &succeed , qc_grammar &qc , qc_data &ast );

template<typename Iter>
forceinline bool isStrQuote( size_t i , Iter iter )
{ return ( *iter == '`' && ( i != 0 ? *( iter - 1 ) != '\\' : true ) ); }

class throwVal
{
public:
	int errCode;

	throwVal( const std::string& first , int errCode );
};

void __Print_Stack_Trace__PST__For__ERROR_Handling( );
