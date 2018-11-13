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
static constexpr int inheritanceOnNamespace = 18;
static constexpr int inheritanceOnEnumeration = 19;

struct _indent { size_t indentLevel; };
struct __indent
{
	_indent operator()( size_t indentLevel );
};
static __indent indent;
std::ostream& operator<<( std::ostream& o , _indent&& v );
std::ostream& operator<<( std::ostream& o , __indent );

void parseLang( std::ostream& o , bool& isTagParsed , std::string & text , const tags & tagVal , const std::string & toLang );
void commonReplacement( std::string & val );
void convStr( std::string & val , const std::string & toLang , bool processNewLines = true , bool applyException = false );
void replaceAngleBrace( std::string & val );
void checkStr( const std::string_view & val , const std::string & toLang );
std::string WriteDoLoop( std::string& val , std::ostream& o , const std::string & toLang );
void convertTextToLang( std::ostream& o , std::string & text , const std::string& toLang , bool doIndent = true , bool processNewLines = true, bool remCont = true );
size_t getFirstNewline( const std::string_view& val , size_t off = 0 );
int parseArgs( boost::program_options::variables_map vm );

void traversePath( boost::filesystem::path &arg , std::string &toLang , bool indent );

void parse_file( boost::filesystem::path &filename , std::string &toLang );
void parse_file( boost::filesystem::path & filename , std::string &storage , std::string::const_iterator &iter , std::string::const_iterator &end , bool &succeed , qc_grammar &qc , qc_data &ast );

template<typename Iter>forceinline bool isStrQuote( size_t i , Iter iter )
{ return ( *iter == '`' && ( i != 0 ? *( iter - 1 ) != '\\' : true ) ); }

class qcParsingException : public std::exception
{
public:
	int errCode;

	qcParsingException( const std::string& first , int errCode );
};

void parseLang_writeFunctionTemplate( std::string::iterator &templateIdx , std::string & subVal , const std::string & toLang , std::ostream & o , std::string &retVal , std::string &funcSig );

void parseLang_writeFunction_whenGive_t( bool isTemplated , std::_String_iterator<std::_String_alloc<std::_String_base_types<char , std::allocator<char>>>::_Mydata_t> &givesIdx , std::string & subVal , std::ostream & o , const std::string & toLang , std::string::iterator &templateIdx );
