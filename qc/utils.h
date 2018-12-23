#pragma once

static constexpr int unknownError = -1;
static constexpr int invalidCommandLineParameters = 1;
static constexpr int noInpFileOrDir = 2;
static constexpr int notFileNorDir = 3;
static constexpr int braceMismatch = 4;
static constexpr int unableToOpenFileForInput = 5;
static constexpr int unableToOpenFileForOutput = 6;
static constexpr int parsingFailed = 7;
static constexpr int unsupportedLanguage = 8;
static constexpr int invalidLoopSyntax = 9;
static constexpr int propertyNotSupported = 10;
static constexpr int indexerNotSupported = 11;
static constexpr int getSetNotSupported = 12;
static constexpr int finallyNotSupported = 13;
static constexpr int eventNotSupported = 14;
static constexpr int addRemoveNotSupported = 15;
static constexpr int invalidPropertySyntax = 16;
static constexpr int invalidIndexerSyntax = 17;
static constexpr int templateOnNamespace = 18;
static constexpr int templateOnEnumeration = 19;
static constexpr int inheritanceOnNamespace = 20;
static constexpr int inheritanceOnEnumeration = 21;

struct _indent { size_t indentLevel; };
struct __indent
{
	_indent operator()( size_t indentLevel );
};

static __indent indent;
std::ostream& operator<<( std::ostream& o , _indent&& v );
std::ostream& operator<<( std::ostream& o , __indent );

void parse_lang( std::ostream& o , bool& isTagParsed , std::string & text , const tags & tagVal , languageToConvertTo toLang );
void commonReplacement( std::string & val );
void convStr( std::string & val , languageToConvertTo toLang , bool processNewLines = true , bool applyException = false );
void replaceAngleBrace( std::string & val );
void checkStr( const std::string_view & val , languageToConvertTo toLang );
std::string WriteDoLoop( std::string& val , std::ostream& o , languageToConvertTo toLang );
void convertTextToLang( std::ostream& o , std::string & text , languageToConvertTo toLang , bool doIndent = true , bool processNewLines = true, bool remCont = true );
size_t getFirstNewline( const std::string_view& val , size_t off = 0 );
int parseArgs( boost::program_options::variables_map& vm );

void parse_file( boost::filesystem::path &filename , languageToConvertTo toLang );
void parse_file( boost::filesystem::path & filename , std::string &storage , std::string::const_iterator &iter , std::string::const_iterator &end , bool &succeed , qc_grammar &qc , qc_data &ast );

template<typename Iter>forceinline bool isStrQuote( size_t i , Iter iter )
{ return ( *iter == '`' && ( i != 0 ? *( iter - 1 ) != '\\' : true ) ); }

class qcParsingException : public std::exception
{
public:
	int errCode;

	qcParsingException( const std::string& first , int errCode );
};

void parseLang_writeFunctionTemplate( std::string::iterator &templateIdx , std::string & subVal , languageToConvertTo toLang , std::ostream & o , std::string &retVal , std::string &funcSig );
