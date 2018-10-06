#pragma once

static constexpr int noInpFileOrDir = 1;
static constexpr int notFileNorDir = 2;
static constexpr int tagMismatch = 3;
static constexpr int braceMismatch = 4;
static constexpr int unableToOpenFile = 5;
static constexpr int parsingFailed = 6;
static constexpr int invalidFile = 7;
static constexpr int unsupportedLanguage = 8;
static constexpr int invalidExpr = 9;
static size_t indent = 0;

void indentialTab( std::ostream& o );

inline void tab( int indent , std::ostream& o )
{ for ( int i = 0; i < indent; ++i ) o << ' '; }

void parseLang( std::ostream& o , int indent , int tabsize , bool& isTagParsed , std::string & text , const std::string & tagVal , const std::string & toLang );
void convStr( std::string & val , const std::string & toLang );
void replaceAngleBrace( std::string & val );
void checkStr( std::string & val , const std::string & toLang );
void convertTextToLang( std::ostream& o , std::string & text , const std::string& toLang );
size_t getFirstNewline( const std::string& val , size_t off = 0 );
int parseArgs( std::vector<std::string> args );

void traversePath( boost::filesystem::path &arg , std::string &toLang );

void parse_file( boost::filesystem::path &filename , std::string &toLang );
void parse_file( boost::filesystem::path& filename , std::string &storage , std::string::const_iterator &iter , std::string::const_iterator &end , bool &succeed , qc_grammar &qc , qc_data &ast );

class throwVal
{
public:
	int errCode;

	throwVal( const std::string first , int errCode );
};
