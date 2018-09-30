#include "stdafx.h"
#include "utils.h"
#include "cross_utils.h"

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
	//{ '>','<' },
	//{ ')','(' },
	//{ ']','[' },
	//{ '}','{' }
};

static const std::set<char> allOpenBraces{ '<','(','[','{' };
static const std::set<char> allCloseBraces{ '>',')',']','}' };
static const std::set<std::string> supportedLanguages
{
	"",
	"C#"
};

template<typename Func>
size_t find_first_of( const std::string& text , size_t after , Func&& f )
{
	auto iter = text.begin()+after;
	for(size_t i = after; i<text.size(); i++ , iter++)
	{
		if(f( text , i , iter , std::string_view( text.c_str()+i , text.size()-i ) ))
		{
			return i;
		}
	}
	return std::string::npos;
}

template<typename Func>
size_t find_first_not_of( const std::string& text , size_t after , Func&& f )
{
	auto iter = text.begin()+after;
	for(size_t i = after; i<text.size(); i++ , iter++)
	{
		if(!f( text , i , iter , std::string_view( text.c_str()+i , text.size()-i ) ))
		{
			return i;
		}
	}
	return std::string::npos;
}

template<typename OutputIterator , typename Func>
void split( const std::string& text , OutputIterator output , Func&& f )
{
	std::size_t start = find_first_not_of( text , 0 , f ) , end = 0;

	while(( end = find_first_of( text , start , f ) )!=std::string::npos)
	{
		*output = text.substr( start , end-start );
		output++;
		start = find_first_not_of( text , end , f );
	}
	if(start!=std::string::npos)
		*output = text.substr( start );
}

void indentialTab( std::ostream& o ) { for(size_t i = 0; i<indent*4; i++)o<<" "; }

void parseLang(
	std::ostream& o , int indent , int tabsize ,
	bool& isTagParsed ,
	const std::string & text , const std::string & tagVal , const std::string & toLang )
{
	auto isComment = tagVal=="$";
	auto idx = (isTagParsed||isComment) ? 0 : getFirstNewline( text );
	if(idx==std::string::npos)idx = text.length();
	if(tagVal=="class"||tagVal=="struct")
	{
		if(!isTagParsed)
		{
			isTagParsed = true;
			o<<tagVal;
			o<<' '<<text.substr( 0 , idx )<<"{";
		}
		else
		{
			o<<text.substr( 0 , idx );
			tab( indent+tabsize , o );
		}
	}
	else if(isComment)
	{
		if(!isTagParsed)
			isTagParsed = true;
	}
	else if(tagVal=="func")
	{
		using namespace boost::algorithm;


		if(!isTagParsed)
		{
			isTagParsed = true;

			auto givesIdx = boost::find_first( text , " gives " ).begin();
			std::string retVal;
			std::copy( givesIdx+7 , text.begin()+idx , std::back_inserter( retVal ) );
			std::string val = retVal+' '+text.substr( 0 , givesIdx-text.begin() );
			convStr( val , toLang );
			o<<val<<"{";
		}
		else tab( indent+tabsize , o );
	}
	else
	{
		if(!isTagParsed&&( tagVal!="native" ))
		{
			o<<tagVal;
			isTagParsed = true;
			o<<'('<<text.substr( 0 , idx )<<"){";
		}
		else
		{
			o<<text.substr( 0 , idx );
			tab( indent+tabsize , o );
		}
	}
	if(!isComment)
	{
		auto restText = text.substr( idx );
		convertTextToLang( o , restText , toLang );
	}
	else
	{
		o<<"/*"<<text<<"*/"<<std::endl;
	}
}

void convStr( std::string & val , const std::string & toLang )
{
	using namespace boost::algorithm;

	replaceAngleBrace( val );
	replace_all( val , "also give" , "yield return" );
	replace_all( val , "finalize" , "yield break" );
	replace_all( val , "give" , "return" );
	replace_all( val , "gen of" , "System.Collections.Generic.IEnumerable" );
	replace_all( val , "iter gen of" , "System.Collections.Generic.IEnumerator" );
	replace_all( val , "gen" , "System.Collections.IEnumerable" );
	replace_all( val , "iter gen" , "System.Collections.IEnumerator" );
	replace_all( val , "\n" , ";\n" );
	replace_all( val , "_;" , "" );
	replace_all( val , "\n;" , "\n" );
	if(val[ 0 ]==';') val = val.substr( 1 );
}

void replaceAngleBrace( std::string & val )
{
	using namespace boost::algorithm;

	replace_all( val , "{{" , "<" );
	replace_all( val , "}}" , ">" );
}

void checkStr( std::string & val , const std::string & toLang )
{
	using namespace std::string_literals;
	std::list<char> closeBraces;
	size_t idx = -1;
	for(auto first = val.begin() , end = val.end();
		 first!=end;
		 first++ , idx++)
	{
		auto& chr = *first;
		if(allOpenBraces.find( chr )!=allOpenBraces.end())
		{
			// chr is an open brace, lets add it to the checking list, closeBraces.
			closeBraces.push_back( openCloseBrace.at( chr ) );
		}
		else
		{
			// chr is not an open brace, but maybe a close brace.
			if(allCloseBraces.find( chr )!=allCloseBraces.end()&&closeBraces.size()>=1)
			{
				auto expectedCloseBrace = closeBraces.back();
				if(expectedCloseBrace!=chr)
				{
					// chr is a close brace, but not the correct one,
					// so the code is incorrect.
					auto loc = val.substr(
						val.find_last_of( '\n' , idx+1 )+1 ,
						getFirstNewline( val , idx ) );
					throw throwVal( (
						"Expected \""s+
						expectedCloseBrace+
						"\"at \"\n"+
						loc+
						"\n\",\n got "+
						chr ) , braceMismatch );
				}
				else
				{
					// chr is the correct close brace.
					closeBraces.pop_back();
				}
			}
			else
			{
				// chr is neither an open brace, nor a close brace.
				continue;
			}
		}
	}
}

void convertTextToLang( std::ostream& o , std::string & text , const std::string & toLang )
{
	using namespace phoenix::arg_names;
	using namespace boost::algorithm;
	using namespace std;

	vector<std::string> allSplit;
	replace_all( text , "\\|]~" , ">" );
	replace_all( text , "\\~[|" , "<" );

	split( text , std::back_inserter( allSplit ) ,
		( [] ( auto&& , auto&& idx , auto&& iter , auto&& )
	{
		return iter[ 0 ]=='`'&&( idx==0 ? true : iter[ -1 ]!='\\' );
	} ) );

	for(int i = 0; i<allSplit.size(); i++)
	{
		bool isEven = ( i%2 )==0;
		auto& val = allSplit[ i ];
		replace_all( val , "\\`" , "`" );
		if(isEven)
		{
			replaceAngleBrace( val );
			checkStr( val , toLang );
			convStr( val , toLang );
			o<<val;
		}
		else
		{
			replace_all( val , R"(")" , R"("")" );
			o<<R"(@")"<<val<<R"(")";
		}
	}
}

size_t getFirstNewline( const std::string & val , size_t off )
{
	using namespace boost::algorithm;
	auto pos = val.find_first_of( '\n' , off );
	while(true)
	{
		if(pos==0) return pos;
		auto underscore = val.find_last_of( "_" , pos );
		if(pos==( underscore+1 ))pos = val.find_first_of( '\n' , pos+1 );
		else return pos;
	}
}

int parseArgs( std::vector<std::string>args )
{
	using namespace std;
	using namespace std::string_literals;
	using boost::filesystem::current_path;
	using boost::filesystem::path;
	using boost::filesystem::directory_iterator;

	path curr = workingDir();

	path arg;
	if(args.size()>=1)
	{
		arg = args[ 0 ];
		if(!arg.is_absolute())
		{
			arg = curr;
			arg /= args[ 0 ];
		}
	}
	else
	{
		cerr<<"Error: No input file or directory provided.";
		return noInpFileOrDir;
	}

	string toLang( "" );
	if(args.size()>=2) toLang = args[ 1 ];

	if(supportedLanguages.find( toLang )==supportedLanguages.end())
	{
		auto v = throwVal( "Language \""+toLang+"\" is not presently supported" , unsupportedLanguage ).errCode;
		getchar();
		return v;
	}

	//cout<<"------------------------------"<<endl;
	cout<<"Full parsing started"<<endl;
	//cout<<"------------------------------"<<endl;

	try
	{
		indent++;
		if(is_regular_file( arg ))
			parse_file( arg , toLang );
		else if(is_directory( arg ))
			traversePath( arg , toLang );
		indent--;
		//cout<<"------------------------------"<<endl;
		cout<<"Full parsing succeeded"<<endl;
		//cout<<"------------------------------"<<endl;
	}
	catch(throwVal& val)
	{
		//cout<<"------------------------------"<<endl;
		indent--;
		cout<<"Full parsing failed"<<endl;
		//cout<<"------------------------------"<<endl;
		getchar();
		return val.errCode;
	}
	getchar();
	return 0;
}

void traversePath( boost::filesystem::path &arg , std::string &toLang )
{
	using namespace std;
	using namespace std::string_literals;
	using boost::filesystem::current_path;
	using boost::filesystem::path;
	using boost::filesystem::directory_iterator;

	auto truncname = arg.filename().string();

	//cout<<"------------------------------"<<endl;
	indentialTab( cout );
	cout<<truncname<<" Dir. parsing started"<<endl;
	//cout<<"------------------------------"<<endl;

	try
	{
		for(directory_iterator first( arg ) , end; first!=end; first++)
		{
			path narg = first->path();
			if(is_regular_file( narg ))
			{
				indent++;
				parse_file( narg , toLang );
				indent--;
			}
			else if(is_directory( narg ))
			{
				indent++;
				traversePath( narg , toLang );
				indent--;
			}
			else throw throwVal( "Path is neither a file not a directory" , notFileNorDir );
		}
		//cout<<"------------------------------"<<endl;
		indentialTab( cout );
		cout<<truncname<<" Dir. parsing succeeded"<<endl;
		//cout<<"------------------------------"<<endl;
	}
	catch(throwVal&)
	{
		//cout<<"------------------------------"<<endl;
		indentialTab( cout );
		cout<<truncname<<" Dir. parsing failed"<<endl;
		//cout<<"------------------------------"<<endl;
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

	std::ifstream in( filename.string() , ios_base::in );

	if(!in)
	{
		throw throwVal( "Could not open input file: "+filename.string() , unableToOpenFile );
	}

	in.unsetf( ios::skipws ); // No white space skipping!
	in.seekg( 0 , ios::end );
	storage.reserve( in.tellg() );
	in.seekg( 0 , ios::beg );
	copy(
		istream_iterator<char>( in ) ,
		istream_iterator<char>() ,
		back_inserter( storage ) );

	using boost::spirit::ascii::space;
	iter = storage.begin();
	end = storage.end();
	succeed = phrase_parse( iter , end , qc , space , ast );
}

void parse_file( boost::filesystem::path &filename , std::string &toLang )
{
	if(filename.extension()!=".qc") return;

	using namespace std;
	using namespace std::string_literals;

	bool succeeded;
	string::const_iterator iter , end;
	qc_grammar qc;
	qc_data ast;
	string storage; // We will read the contents here.

	auto truncname = filename.filename().string();

	//cout<<"------------------------------"<<endl;
	indentialTab( cout );
	cout<<truncname<<" File parsing started"<<endl;
	//cout<<"------------------------------"<<endl;

	parse_file( filename , storage , iter , end , succeeded , qc , ast );

	if(succeeded && iter==end)
	{
		qc_parser parser;
		std::ofstream o( filename.replace_extension( languageExtensionDict.at( toLang ) ).string() );
		try
		{
			parser( ast , o , toLang );
			//cout<<"------------------------------"<<endl;
			indentialTab( cout );
			cout<<truncname<<" File parsing succeeded"<<endl;
			//cout<<"------------------------------"<<endl;
		}
		catch(throwVal&)
		{
			//cout<<"------------------------------"<<endl;
			indentialTab( cout );
			cout<<truncname<<" File parsing failed"<<endl;
			//cout<<"------------------------------"<<endl;
			throw;
		}
		return;
	}
	else
	{
		//cout<<"------------------------------"<<endl;
		cout<<truncname<<endl;
		cout<<" File parsing failed"<<endl;
		//cout<<"------------------------------"<<endl;
		throw throwVal( "File parsing failed" , parsingFailed );
	}
}

inline throwVal::throwVal( const std::string first , int errCode ) :errCode( errCode )
{
	indent++;
	indentialTab( std::cout );
	std::cerr<<"Error: "<<first<<std::endl;
	indent--;
}
