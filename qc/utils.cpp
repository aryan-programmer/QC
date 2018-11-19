#include "stdafx.h"
#include "utils.h"
#include "cross_utils.h"
#include "tag_block_manager.h"
#include <sstream>
#include <boost/format.hpp>
#include <boost/stacktrace.hpp>
#define BOOST_SCOPE_EXIT_CONFIG_USE_LAMBDAS
#include <boost/scope_exit.hpp>

#define caref constexpr auto &
caref lTo = "To";
caref lStep = "Step";
caref lcolon_colon = "::";
caref ldot = ".";
caref l_Gives_ = " Gives ";
caref l_To_ = " To ";
caref l_Step_ = " Step ";
caref l_Of_ = " Of ";
caref lVariable_ = "Variable ";
caref l_Template_ = " Template ";
caref l_Inherits_ = " Inherits ";
caref l_And_ = "And";

const std::unordered_map<std::string_view , std::string_view> opReplacements
{
{"LessOrEq"		, "<=" },
{"MoreOrEq"		, ">=" },
{"NotEquals"	, "!=" },
{"Equals"		, "==" },
{"Less"			, "<" },
{"More"			, ">" },
{"LeftShiftSet"	, "<<=" },
{"RightShiftSet", ">>=" },
{"LeftShift"	, "<<" },
{"RightShift"	, ">>" },
};

const std::unordered_map<languageToConvertTo , std::map<std::string_view , std::string_view>> replacements
{
	{
		languageToConvertTo::CS,
		{
{"Abstract"		,"abstract"},
{"AlsoGive"		,"yield return"},
{"Await"		,"await"},
{"My Base"		,"//"},
{"Base"			,"base"},
{"Break"		,"break"},
{"CPPInclude"	,"//"},
{"CSLine"		,"" },
{"CPPLine"		,"//" },
{"Constant"		,"const"},
{"Templated"	,"//"},
{"Finalize"		,"yield break"},
{"Give"			,"return"},
{"Class"		,"class"},
{"CPPConst"		,""},
{"Continue"		,"continue"},
{"Default"		,"default"},
{"Delegate"		,"delegate"},
{"Event"		,"event"},
{"Explicit"		,"explicit"},
{"Extern"		,"extern"},
{"False"		,"false"},
{"Goto"			,"goto"},
{"Implicit"		,"implicit"},
{"In"			,"in"},
{"Is"			,"is"},
{"New"			,"new"},
{"DynNew"		,"new"},
//NullPtrType
//NullPtr
{"Null"			,"null"},
{"Operator"		,"operator"},
{"Override"		,"override"},
{"Parameters"	,"params"},
{"Private"		,"private"},
{"Protected"	,"protected"},
{"Public"		,"public"},
{"Internal"		,"internal"},
{"ReadOnly"		,"readonly"},
{"Sealed"		,"sealed"},
{"StackAlloc"	,"stackalloc"},
{"Static","static"},
{"Structure"	,"struct"},
{"Me"			,"this"},
{"Throw"		,"throw"},
{"True"			,"true"},
{"Virtual"		,"virtual"},
{"Nothing"		,"void"},
{"Volatile"		,"volatile"},
{"Auto"			,"var"},
{"CSVar"		,"var"},
{"using namespace QCCore"	,"using QCCore; using static QCCore.QCCoreCls"},
{"using namespace"	,"using"},
{"Namespace"	,"namespace"},
{"Using"		,"using"},
{"Global"		,"global" },
{"Dynamic"		,"dynamic" },
{"Wide"			,"" },
{"REM"			,"//" },
{"TypeOf"		,"typeof" },
{"Get"			,"get" },
{"Set"			,"set" },
{"CSPublic"		,"public" },

#pragma region Types
{"Int16","short"},
{"Int32","int"},
{"Int64","long"},
{"UInt16","ushort"},
{"UInt32","uint"},
{"UInt64","ulong"},
{"Short","short"},
{"Int","int"},
{"Long","long"},
{"UShort","ushort"},
{"UInt","uint"},
{"ULong","ulong"},
{"Char","char"},
{"WChar","char"},
{"Bool","bool"},
{"Byte","byte"},
{"SByte","sbyte"},
{"Object","object"},
{"HashCode","int"},
{"Float","float"},
{"Single","float"},
{"Double","double"},
{"String","string"},
{"WString","string"},
{"StringRef","string"},
{"WStringRef","string"},
{"GenOf","System.Collections.Generic.IEnumerable"},
{"Gen","System.Collections.IEnumerable"},
{"IterGenOf","System.Collections.Generic.IEnumerator"},
{"IterGen","System.Collections.IEnumerator"},
{"Ref","ref"},
{"CPPRef",""},
{"Out","out"},
{"CPPOut",""},
{"PriMutex","object"},
#pragma endregion
		}
	},
		{
		languageToConvertTo::CPP,
		{
{"Abstract"		,""},
{"AlsoGive","co_yield"},
{"Await","co_await"},
//Base
//Bool = bool
{"My Base"	,"using Base"},
//Byte = std::byte
{"CPPInclude","#include"},
{"CPPLine","" },
{"CSLine","//" },
{"Constant","static constexpr"},
{"Templated"	,"template<typename... Args>"},
{"CPPInclude QCCore","#include <QCCore.hpp>"},
{"Namespace","namespace"},
{"Using","using"},
{"Constant String","static constexpr ::std::string_view" },
{"Constant WString","static constexpr ::std::wstring_view" },
{"Me","(*this)"},
{"Finalize","return"},
{"Give","return"},
{"Break","break"},
{"Class","class"},
{"Checked",""},
{"CPPConst","const"},
{"Continue","continue"},
{"Default","default"},
{"Delegate","[&]"},
{"Event","event"},
{"Explicit","explicit"},
{"Extern","extern"},
{"False","false"},
{"Goto","goto"},
{"Is",":"},
{"Implicit",""},
{"DynNew","new"},
{"New",""},
{"NullPtr","nullptr"},
{"Null","nullptr"},
{"Operator","operator"},
{"Override",""},
{"Private","private:"},
{"Protected","protected:"},
{"Public","public:"},
{"Internal","public:"},
{"ReadOnly","const"},
{"Sealed","final"},
{"StackAlloc",""},
{"Static","static"},
{"Structure","struct"},
{"Throw","throw"},
{"True","true"},
{"Virtual","virtual"},
{"Nothing","void"},
{"Volatile","volatile"},
{"Auto","auto"},
{"CSVar",""},
{"Global",""},
{"Dynamic","auto"},
{ "Wide","L" },
{"REM","//" },
{"CSPublic"		,"" },

#pragma region Types
//Int16
//Int32
//Int64
//UInt16
//UInt32
//UInt64
//Short
//Int
//Long
//UShort
//UInt
//ULong
//Char
//WChar
//Bool
//Byte
//SByte
//Object
//HashCode
//Float
//Single
//Double
//String
//WString
//StringRef
//WStringRef
{"GenOf","std::experimental::generator<T>"},
{"Gen","std::experimental::generator<Object>"},
{"Ref",""},
{"CPPRef","&"},
{"Out",""},
{"CPPOut","&"},
{"NullPtrType","std::nullptr_t"},
{"PriMutex","::std::mutex"},
				#pragma endregion

		}
	}
};
const std::string_view nonAlnumChars { "`~!#$%^&*()-=+[]{}|\\;:'\",./?\n<>" };
#undef caref

void ReplaceAllSurroundedByNonAlnumChars( std::string& input , std::string search , std::string replace );

void replaceInpAtStartSNRPaddedAtEnd( std::string &search , std::string &replace , std::string & input );

static const std::unordered_map<std::string , std::unordered_map<languageToConvertTo , std::string>> QCExtention_Language_Extention
{
	{
		// The usual QC file converted to ".cs" & ".cpp" in QC trans-compiled to C# and QC trans-compiled to C++, respectively.
		{".qc" },
		{
			{languageToConvertTo::CS,".cs" },
			{languageToConvertTo::CPP,".cpp" },
		}
	},
	{
		// A usual QC file tagged with an 's' conveying that it is a source code file. Files of this extension converted to ".cs" & ".cpp" in QC trans-compiled to C# and QC trans-compiled to C++, respectively.
		{".sqc" },
		{
			{languageToConvertTo::CS,".cs" },
			{languageToConvertTo::CPP,".cpp" },
		}
	},
	{
		// A usual QC file tagged with an 'h' conveying that it is a header file containing "CPPInclude"s and forward declarations. This file is usually to be used in QC trans-compiled to C++ only. Files of this extension converted to ".cs" & ".hpp" in QC trans-compiled to C# and QC trans-compiled to C++, respectively.
		{".hqc" },
		{
			{languageToConvertTo::CS,".cs" },
			{languageToConvertTo::CPP,".hpp" },
		}
	},
	{
		// A usual QC file tagged with an 'u' conveying this is used mainly in game-engines like Unity & Unreal. Files of this extension converted to ".cs" & ".cpp" in QC trans-compiled to C# and QC trans-compiled to C++, respectively.
		{".uqc" },
		{
			{ languageToConvertTo::CS,".cs" },
			{ languageToConvertTo::CPP,".hpp" },
		}
	},
};

static const std::set<std::string> validQCFileExtentions { ".qc",".sqc",".hqc",".uqc" };

static const std::unordered_map<char , char> openCloseBrace
{
	{ '<','>' },
	{ '(',')' },
	{ '[',']' },
	{ '{','}' }
};

static const std::set<char> allOpenBraces { '<','(','[','{' };
static const std::set<char> allCloseBraces { '>',')',']','}' };

auto isNewLine =
[ ] ( auto&& i , auto&& iter )
{
	// If the char is a new line
	return *iter == '\n' &&
		// and if this is not the 2nd char i.e. we cannot access the previous 2 chars then we if statement gives true
		( i <= 2 ? true :
		  // But if we can access the previous 2 chars 
		  // then we return false 
		  // if the previous char is an underscore 
			// and the char before is a space.
		  !( *( iter - 1 ) == '_' && isspace( *( iter - 2 ) ) ) );
};

void ReplaceNewLines( std::string &subVal );

void parse_lang_CSI_replace_And( languageToConvertTo toLang , std::string &inheritance , std::ostream & o );

auto __indent::operator()( size_t indentLevel )-> _indent { return _indent { indentLevel }; }

std::ostream& operator<<( std::ostream& o , _indent&& v )
{
	for ( size_t i = 0; i < v.indentLevel; i++ )o << '\t';
	return o;
}
std::ostream& operator<<( std::ostream& o , __indent )
{
	for ( size_t i = 0; i < indentLevel; i++ )o << '\t';
	return o;
}

void parse_lang_write_property( std::string &subVal , languageToConvertTo toLang , std::ostream & o )
{
	std::stringstream strm;
	convertTextToLang( strm , subVal , toLang , false , false , false );
	subVal = strm.str( );
	auto ofIdx = boost::find_first( subVal , l_Of_ ).begin( );
	if ( ofIdx == subVal.end( ) )throw qcParsingException( "The name of the property must come before \"Of\" and the type must come after \"Of\"." , invalidPropertySyntax );
	// Then the get the return type
	auto retVal = std::string( ofIdx + 4 , subVal.end( ) );
	// Convert and write it
	convertTextToLang( o , retVal , toLang , false , false );
	// a space
	o << ' ';
	// Get the function signature
	auto funcSig = std::string( subVal.substr( 0 , ofIdx - subVal.begin( ) ) );
	// Convert and write it
	convertTextToLang( o , funcSig , toLang , false , false );
}

void parse_lang_write_indexer( std::string &subVal , languageToConvertTo toLang , std::ostream & o )
{
	std::stringstream strm;
	convertTextToLang( strm , subVal , toLang , false , false , false );
	subVal = strm.str( );
	auto ofIdx = boost::find_first( subVal , l_Of_ ).begin( );
	if ( ofIdx == subVal.end( ) )throw qcParsingException( "The name of the property must come before \"Of\" and the type must come after \"Of\"." , invalidPropertySyntax );
	// Then the get the return type
	auto retVal = std::string( ofIdx + 4 , subVal.end( ) );
	// Convert and write it
	convertTextToLang( o , retVal , toLang , false , false );
	// a space
	o << " this";
	// Get the function signature
	auto funcSig = std::string( subVal.substr( 0 , ofIdx - subVal.begin( ) ) );
	// Convert and write it
	convertTextToLang( o , funcSig , toLang , false , false );
}

void parse_lang_write_for_loop( std::string &subVal , languageToConvertTo toLang , const char * errorDesc , std::ostream & o , const char * whenStepFormat , const char * whenNoStepFormat )
{
	using qi::lexeme;
	using qi::lit;
	using qi::phrase_parse;
	using ascii::alnum;
	using ascii::space;
	using ascii::char_;

	// The stream for retrieval
	std::stringstream strm;
	// convert the string and write to the stream
	convertTextToLang( strm , subVal , toLang , false , false , false );
	// retrieve the string
	subVal = strm.str( );

	// The start iter for parsing
	auto subValStart = subVal.begin( );
	// The end iter for comparison and stopping
	auto subValEnd = subVal.end( );
	ReplaceNewLines( subVal );

	// Whether to introduce a new variable
	bool newVar = false;
	// The vector of parsings. 
	std::vector<std::string> vec;
	if ( !phrase_parse(
		// Start
		subValStart ,
		// end
		subValEnd ,
		// If the subVal starts with "Var " then a new variable must be introduced
		lexeme[ -( lit( lVariable_ )[ phx::ref( newVar ) = true ] ) >>
		// The name of the variable
		+alnum ] >> '=' >>
		// The start of the for loop
		lexeme[ +( char_ - l_To_ ) ] >> lTo >>
		// The end of the for loop
		lexeme[ +( char_ - l_Step_ ) ] >>
		// The optional step
		-( lStep >> lexeme[ +char_ ] ) ,
		space , vec ) || subValStart != subValEnd )
		// Parsing failed due to invalid syntax
		throw qcParsingException( errorDesc , invalidLoopSyntax );
	// There is a step value
	if ( vec.size( ) == 4 )
	{
		o <<
			// The Format string when the step is given
			boost::format( whenStepFormat )
			// The variable name
			% vec[ 0 ]
			// The start
			% vec[ 1 ]
			// The stop
			% vec[ 2 ]
			// The step
			% vec[ 3 ]
			// The "Auto" string
			% ( newVar ? replacements.at( toLang ).at( "Auto" ) : "" );
	}
	// There is no step value
	else if ( vec.size( ) == 3 )
	{
		o <<
			boost::format( whenNoStepFormat )
			// The variable name
			% vec[ 0 ]
			// The start
			% vec[ 1 ]
			// The stop
			% vec[ 2 ]
			// The "Auto" string
			% ( newVar ? replacements.at( toLang ).at( "Auto" ) : "" );
	}
	// Invalid syntax
	else throw qcParsingException( errorDesc , invalidLoopSyntax );
}

void parse_lang_write_until( std::ostream & o , std::string &subVal , languageToConvertTo toLang )
{
	o << "while( !( ";
	convertTextToLang( o , subVal , toLang , false , false );
	o << " ) )";
}

void parse_lang_when_noPostModifierTag( std::ostream & o , const tags & tagVal , languageToConvertTo toLang , bool isCase )
{
	using namespace std;
	o << to_string( tagVal , toLang );
	o << endl;
	if ( !isCase ) o << indent << '{' << endl;
}

void parse_lang_write_foreach_when_cpp( std::ostream & o , const tags & tagVal , languageToConvertTo toLang , std::string &subVal )
{
	o << to_string( tagVal , toLang ) << '(';
	convertTextToLang( o , subVal , toLang , false , false );
	o << ")";
}

void parse_lang_write_using_when_cpp( bool &isSkip , std::ostream & o , std::string &subVal , languageToConvertTo toLang , const char * semicolon )
{
	using namespace std;

	isSkip = true;
	o << indent << '{' << endl;
	o << indent( indentLevel + 1 );
	convertTextToLang( o , subVal , toLang , false , false );
	o << semicolon << endl;
}

void parseLang_writeFunctionTemplate( std::string::iterator &templateIdx , std::string & subVal , languageToConvertTo toLang , std::ostream & o , std::string &retVal , std::string &funcSig )
{
	using namespace std;
	using namespace boost;

	// The template parameters
	auto templateParameter = string( templateIdx + 10 , subVal.end( ) );
	if ( toLang == languageToConvertTo::CPP )
		// If the language is C++ then we write the starting template<...> parameters.
		o << "template<typename " << replace_all_copy( templateParameter , "," , ", typename " ) << ">\n" << indent;

	// Then we write the return type
	convertTextToLang( o , retVal , toLang , false , false );
	// a space
	o << ' ';
	// If the language is C++ then we just write the function signature as is.
	if ( toLang == languageToConvertTo::CPP )
		convertTextToLang( o , funcSig , toLang , false , false );
	else
	{
		/// Otherwise we find,
		auto funcNameEnd = funcSig.find_first_of( '(' );
		// The function's name
		auto funcName = funcSig.substr( 0 , funcNameEnd - 1 );
		/// and
		// The function's argument list
		auto funcParams = funcSig.substr( funcNameEnd );
		// Convert and write the function's name,
		convertTextToLang( o , funcName , toLang , false , false );
		// The generic type parameter list
		o << "<";
		convertTextToLang( o , templateParameter , toLang , false , false );
		o << ">";
		// and the function's argument list.
		convertTextToLang( o , funcParams , toLang , false , false );
	}
}

void parseLang_functionRetTypNotGeneric( std::string::iterator & givesIdx , std::string & subVal , std::ostream & o , languageToConvertTo toLang )
{
	using namespace std;

	// Then the get the return type
	auto retVal = string( givesIdx + 7 , subVal.end( ) );
	// Convert and write it
	convertTextToLang( o , retVal , toLang , false , false );
	// a space
	o << ' ';
	// Get the function signature
	auto funcSig = subVal.substr( 0 , givesIdx - subVal.begin( ) );
	// Convert and write it
	convertTextToLang( o , funcSig , toLang , false , false );
}

void parseLang_writeFunction_whenGive( bool isTemplated , std::string::iterator &givesIdx , std::string & subVal , std::ostream & o , languageToConvertTo toLang , std::string::iterator &templateIdx )
{
	using namespace std;

	if ( !isTemplated ) parseLang_functionRetTypNotGeneric( givesIdx , subVal , o , toLang );
	else
	{
		// The return type
		auto retVal = string( givesIdx + 7 , templateIdx );
		// The function signature
		auto funcSig = subVal.substr( 0 , givesIdx - subVal.begin( ) );
		parseLang_writeFunctionTemplate( templateIdx , subVal , toLang , o , retVal , funcSig );
	}
}

void parse_lang_write_function( std::string &subVal , languageToConvertTo toLang , std::ostream & o )
{
	using namespace boost::algorithm;
	using namespace std;

	// Convert the string
	std::stringstream strm;
	convertTextToLang( strm , subVal , toLang , false , false , false );
	subVal = strm.str( );

	// Is the subVal a template
	bool isTemplated = boost::contains( subVal , l_Template_ );
	std::string::iterator templateIdx;
	// If yes then we set the template iterator of the first occurrence of " Template "
	if ( isTemplated ) templateIdx = boost::find_first( subVal , l_Template_ ).begin( );

	// Is a return type is specified
	if ( boost::contains( subVal , l_Gives_ ) )
	{
		// The iterator to the first occurrence of " Gives "
		auto givesIdx = boost::find_first( subVal , l_Gives_ ).begin( );
		parseLang_writeFunction_whenGive( isTemplated , givesIdx , subVal , o , toLang , templateIdx );
	}
	else
	{
		// If the function is not a template
		if ( !isTemplated )
		{
			// Then we write the return type
			o << "void ";
			convertTextToLang( o , subVal , toLang , false , false );
		}
		else
		{
			// The return type
			string retVal = "void";
			string funcSig = subVal.substr( 0 , templateIdx - subVal.begin( ) );
			parseLang_writeFunctionTemplate( templateIdx , subVal , toLang , o , retVal , funcSig );
		}
	}
}

void parse_lang_InterfaceText_AccessModifiers( std::string& subVal , std::ostream & o , languageToConvertTo toLang )
{
	if ( boost::starts_with( subVal , "Public " ) )
	{
		o << replacements.at( toLang ).at( "Public" ) << " ";
		subVal.erase( subVal.begin( ) , subVal.begin( ) + 7 );
	}
	else if ( boost::starts_with( subVal , "Internal " ) )
	{
		o << replacements.at( toLang ).at( "Internal" ) << " ";
		subVal.erase( subVal.begin( ) , subVal.begin( ) +  9 );
	}
	else if ( boost::starts_with( subVal , "Protected " ) )
	{
		o << replacements.at( toLang ).at( "Protected" ) << " ";
		subVal.erase( subVal.begin( ) , subVal.begin( ) + 10 );
	}
	else o << replacements.at( toLang ).at( "Protected" ) << " ";
}

void parse_lang_parse_interface_text( std::string & vText , languageToConvertTo toLang , std::ostream & o , const char * semicolon , std::string_view cppPrefix , std::string_view csPrefix )
{
	using namespace std;
	// We trim the string
	boost::trim( vText );
	// If the text is empty then we return
	if ( vText == "" )return;
	// We loop through the split string.
	loop_split(
		// The text
		vText ,
		// The splitting function
		isNewLine ,
		// The looping function
		[ &o , &semicolon , &toLang , &cppPrefix , &csPrefix ]( string&& subVal )
		{
			// We trim the string
			boost::trim( subVal );
			if ( subVal == "" )return;
			// We add an indent.
			o << indent;
			parse_lang_InterfaceText_AccessModifiers( subVal , o , toLang );
			// If the language is C++ then we insert the C++ prefix
			if ( toLang == languageToConvertTo::CPP ) o << cppPrefix;
			// Else If the language is C# then we insert the C# prefix
			else if ( toLang == languageToConvertTo::CS ) o << csPrefix;
			// If the return type is given
			if ( boost::contains( subVal , l_Gives_ ) )
			{
				// Get the return type idx
				auto givesIdx = boost::find_first( subVal , l_Gives_ ).begin( );
				parseLang_functionRetTypNotGeneric( givesIdx , subVal , o , toLang );
			}
			// Otherwise the return type is "void" and the function signature is the subVal
			else
			{
				o << "void ";
				convertTextToLang( o , subVal , toLang , false , false );
			}
			// If the language is C++ then we make the function pure virtual.
			if ( toLang == languageToConvertTo::CPP ) o << " = 0";
			// We need the semicolon
			o << semicolon << endl;
		}
	);
}

// Post Modifiable Tag Enclosed With Brace
void parse_lang_PMTEWB( std::ostream & o , const tags & tagVal , languageToConvertTo toLang , std::string &subVal )
{
	o << to_string( tagVal , toLang ) << '(';
	convertTextToLang( o , subVal , toLang , false , false );
	o << ")";
	if ( tagVal == tags::_Lock_ && toLang == languageToConvertTo::CPP )o << ";";
}

void parse_lang_write_event( std::string &subVal , languageToConvertTo toLang , std::ostream & o )
{
	using namespace std;

	std::stringstream strm;
	convertTextToLang( strm , subVal , toLang , false , false , false );
	subVal = strm.str( );
	auto ofIdx = boost::find_first( subVal , l_Of_ ).begin( );
	if ( ofIdx == subVal.end( ) )throw qcParsingException( "The name of the event must come before \"Of\" and the type must come after \"Of\"." , invalidPropertySyntax );
	o << " event ";
	// Then the get the return type
	auto retVal = std::string( ofIdx + 4 , subVal.end( ) );
	// Convert and write it
	convertTextToLang( o , retVal , toLang , false , false );
	// a space
	o << ' ';
	// Get the function signature
	auto funcSig = std::string( subVal.substr( 0 , ofIdx - subVal.begin( ) ) );
	// Convert and write it
	convertTextToLang( o , funcSig , toLang , false , false );
}


void parse_lang_CSI_replace_And( languageToConvertTo toLang , std::string &inheritance , std::ostream & o )
{
	if ( toLang == languageToConvertTo::CPP )
	{
		// The default and mandatory inheritance access specifier is Public.
		ReplaceAllSurroundedByNonAlnumChars( inheritance , l_And_ , ", Public " );
		// The inheritance specifier access specifier is Public for the first type
		o << "public ";
	}
	else
	{
		// " And " means that the CSI inherits the previous _And_ the next types.
		ReplaceAllSurroundedByNonAlnumChars( inheritance , l_And_ , ", " );
	}
}

void parseLang_writeCSI_Template( std::string & subVal , languageToConvertTo toLang , std::ostream & o , const tags & tagVal )
{
	using namespace std;

	// The iterator to the first occurrence of " Template "
	auto templateIdx = boost::find_first( subVal , l_Template_ ).begin( );
	// The template parameters
	auto templateParameters = string( templateIdx + 10 , subVal.end( ) );
	// If the language is C++
	if ( toLang == languageToConvertTo::CPP )
		// We insert the appropriate starting template<...> parameter list in C++
		o << "template<typename " << boost::replace_all_copy( templateParameters , "," , ", typename" ) << ">\n" << indent;
	// Write the tag's string repr.
	o << to_string( tagVal , toLang ) << ' ';
	// The new subVal containing all the information about the Class/Structure/Interface's DECLARATION.
	auto newSubVal = subVal.substr( 0 , templateIdx - subVal.begin( ) );
	// If the Class/Structure/Interface inherits some types.
	if ( boost::contains( newSubVal , l_Inherits_ ) )
	{
		// The iterator to the first occurance of " Inherits "
		auto inheritsIdx = boost::find_first( newSubVal , l_Inherits_ ).begin( );
		// The name of the Class/Structure/Interface which we convert and write.
		auto name = newSubVal.substr( 0 , inheritsIdx - newSubVal.begin( ) );
		convertTextToLang( o , name , toLang , false , false );
		// The template specialization in C++ and the generic type parameter list declaration in C#.
		o << "<";
		convertTextToLang( o , templateParameters , toLang , false , false );
		o << ">";
		// The string containing all the types that the Class/Structure/Interface inherit
		auto inheritance = newSubVal.substr( ( inheritsIdx - newSubVal.begin( ) ) + 10 );
		// The inherits specifier
		o << ": ";
		parse_lang_CSI_replace_And( toLang , inheritance , o );
		// We print the inheritance list.
		convertTextToLang( o , inheritance , toLang , false , false );
	}
	// If the class inherits no types
	else
	{
		// Write the class' name
		convertTextToLang( o , newSubVal , toLang , false , false );
		// The template specialization in C++ and the generic type parameter list declaration in C#.
		o << "<";
		convertTextToLang( o , templateParameters , toLang , false , false );
		o << ">";
	}
}

// class structure namespace enum interface
void parse_lang_write_CSNEI( std::string &subVal , languageToConvertTo toLang , std::ostream & o , const tags & tagVal )
{
	using namespace std;

	// If the CSNEI is templated
	if ( boost::contains( subVal , l_Template_ ) )
	{
		// If the tag is a namespace or an enum we throw the corresponding errors.
		if ( tagVal == tags::_Namespace_ )
			throw qcParsingException( "Namespaces can't have templates." , templateOnNamespace );
		if ( tagVal == tags::_Enum_ )
			throw qcParsingException( "Enumerations can't have templates." , templateOnEnumeration );
		parseLang_writeCSI_Template( subVal , toLang , o , tagVal );
	}
	else
	{
		// Write the tag's string repr.
		o << to_string( tagVal , toLang ) << ' ';
		// If the Class/Structure/Interface inherits some types.
		if ( boost::contains( subVal , l_Inherits_ ) )
		{
			// If the tag is a namespace or an enum we throw the corresponding errors.
			if ( tagVal == tags::_Namespace_ )
				throw qcParsingException( "Namespaces can't have inheritance" , inheritanceOnNamespace );
			if ( tagVal == tags::_Enum_ )
				throw qcParsingException( "Enumerations can't have inheritance" , inheritanceOnEnumeration );
			// The iterator to the first occurance of " Inherits "
			auto inheritsIdx = boost::find_first( subVal , l_Inherits_ ).begin( );
			// The name of the Class/Structure/Interface which we convert and write.
			auto name = subVal.substr( 0 , inheritsIdx - subVal.begin( ) );
			convertTextToLang( o , name , toLang , false , false );
			// The string containing all the types that the Class/Structure/Interface inherit
			auto inheritance = subVal.substr( ( inheritsIdx - subVal.begin( ) ) + 10 );
			// The inherits specifier
			o << ": ";
			parse_lang_CSI_replace_And( toLang , inheritance , o );
			// We print the inheritance list.
			convertTextToLang( o , inheritance , toLang , false , false );
		}
		// If the class inherits no types then we just have the name of the type
		else convertTextToLang( o , subVal , toLang , false , false );
	}
}

void ReplaceNewLines( std::string &subVal )
{
	boost::replace_all( subVal , " _\n" , " \n" );
}

void parse_lang( std::ostream& o , bool& isTagParsed ,
				 std::string & text , const tags & tagVal ,
				 languageToConvertTo toLang )
{
	using namespace std;
	auto
		// Is the tag a comment
		isComment = tagVal == tags::_Comment_ ,
		// Is the tag a root (QC) tag
		isRoot = tagVal == tags::_QC_ ,
		// Should the tag's data be inserted as is
		isNative = tagVal == tags::_Native_ || tagVal == tags::_CPP_ || tagVal == tags::_CS_ ,
		// Should skip inserting open brace
		isSkip = isNative || isRoot || isComment /* ||extra stuff */ ,
		// Is the tag a statement in a switch (Case||FTCase||Default||FTDefault)
		isCase = false ,
		// Is the tag one that has no post modifiers.
		isANoPostModiferTag = false ,
		// Does the tag encapsulate abstract functions.
		isAbstract = tagVal == tags::_Abstract_;
	// We only trim from the left because we do not want the new line characters to be erased.
	boost::trim_left( text );
	// If the tag has been parsed or the tag is a comment tag then subVal
	// i.e. the modifier on the tag has no meaning.
	auto idx = ( isTagParsed || isComment ) ? 0 : getFirstNewline( text );
	// The modifier on the tag.
	string subVal;
	// If there are no new lines then the newline is considered to be the end of the string.
	if ( idx == string::npos ) idx = text.length( );
	subVal = text.substr( 0 , idx );
	// This is required later so as to ascertain that
	const bool wasTagParsed = isTagParsed;
	// We trim both sides here because we will manually insert the semicolons when needed.
	boost::trim( subVal );
	// Makes sure that empty lines don't have semicolons inserted.
	const auto semicolon = ( subVal.length( ) != 0 ? ";" : "" );
	ReplaceNewLines( subVal );
	idx = subVal.length( );

	// Only if the tag has not been parsed
	if ( !isTagParsed )
	{
		// We indent for cleanliness.
		o << indent;
		// The large switch
		switch ( tagVal )
		{
			// Block
		case tags::_Blk_: goto NoPostModifierTags;
			// Root
		case tags::_QC_: break;

			// Class, Structure, Namespace, Interface & Enum
		case tags::_Class_: case tags::_Structure_: case tags::_Namespace_:
		case tags::_Interface_: case tags::_Enum_:
			parse_lang_write_CSNEI( subVal , toLang , o , tagVal );
			break;

		case tags::_Abstract_:
			isSkip = true;
			break;

			// NativeCPP, NativeCS & Native
		case tags::_CPP_: case tags::_CS_: case tags::_Native_: break;

			// Property
		case tags::_Property_:
			if ( toLang == languageToConvertTo::CPP )throw qcParsingException( "Properties aren't supported in QC trans-compiled to C++ (17 With Boost)." , propertyNotSupported );
			parse_lang_write_property( subVal , toLang , o );
			break;

			// Indexer
		case tags::_Indexer_:
			if ( toLang == languageToConvertTo::CPP )throw qcParsingException( "Indexers aren't supported in QC trans-compiled to C++ (17 With Boost)." , indexerNotSupported );
			parse_lang_write_indexer( subVal , toLang , o );
			break;

			// Unsafe, Unchecked, Checked (n)
		case tags::_Unsafe_: case tags::_Unchecked_:case tags::_Checked_:
			goto NoPostModifierTags;
			// (n) Get, Set (n)
		case tags::_Get_: case tags::_Set_:
			if ( toLang == languageToConvertTo::CPP )throw qcParsingException( "Get & Set aren't supported in QC trans-compiled to C++ (17 With Boost)." , getSetNotSupported );
			goto NoPostModifierTags;

			// If, ElseIf (y)
		case tags::_If_: case tags::_ElseIf_: goto PostModifiableTagEncolsedWithBrace;

			// (n) Else (n)
		case tags::_Else_: goto NoPostModifierTags;

			// (y) Switch (y)
		case tags::_Switch_: goto PostModifiableTagEncolsedWithBrace;

			// Case & FallThroughCase
		case tags::_Case_: case tags::_FTCase_:
			o << "case ";
			convertTextToLang( o , subVal , toLang , false , false );
			o << ":";
			isCase = true;
			break;

			// (n) Default, FallThroughDefault (n)
		case tags::_Default_: case tags::_FTDefault_:
			isCase = true;
			goto NoPostModifierTags;

			// (n) Try (n)
		case tags::_Try_: goto NoPostModifierTags;
			// (y) Catch (y)
		case tags::_Catch_: goto PostModifiableTagEncolsedWithBrace;

			// (n) Finally (n)
		case tags::_Finally_:
			if ( toLang == languageToConvertTo::CPP ) throw qcParsingException( "Finally blocks aren't supported in QC trans-compiled to C++ (17 With Boost)." , finallyNotSupported );
			// (n) Do (n)
			// Unsafe, Unchecked, Checked, Get, Set, Else, Default, FallThroughDefault, Try, Finally, Do, ForEver, Add and Remove
		case tags::_Do_:
		NoPostModifierTags:
			parse_lang_when_noPostModifierTag( o , tagVal , toLang , isCase );
			isSkip = isANoPostModiferTag = true;
			break;

			// Until
		case tags::_Until_:
			parse_lang_write_until( o , subVal , toLang );
			break;

			// (y) While (y)
		case tags::_While_: goto PostModifiableTagEncolsedWithBrace;

			// For
		case tags::_For_:
		{
			const char* errorDesc = "Invalid For loop syntax";
			const char* whenStepFormat = "for(%5% %1% = %2%; %1% <= %3%; %1% += %4% )";
			const char* whenNoStepFormat = "for(%4% %1% = %2%; %1% <= %3%; ++%1% )";

			parse_lang_write_for_loop( subVal , toLang , errorDesc , o , whenStepFormat , whenNoStepFormat );
		}
		break;

		// Reverse For
		case tags::_RevFor_:
		{
			const char* errorDesc = "Invalid RevFor loop syntax";
			const char* whenStepFormat = "for( %5% %1% = %3%; %1% >= %2%; %1% -= %4% )";
			const char* whenNoStepFormat = "for( %4% %1% = %3%; %1% >= %2%; --%1% )";

			parse_lang_write_for_loop( subVal , toLang , errorDesc , o , whenStepFormat , whenNoStepFormat );
		}
		break;

		// (y) ForEach (y)
		case tags::_ForEach_:
			if ( toLang == languageToConvertTo::CPP ) parse_lang_write_foreach_when_cpp( o , tagVal , toLang , subVal );
			else goto PostModifiableTagEncolsedWithBrace;
			break;

			// (n) ForEver (n)
		case tags::_ForEver_: goto NoPostModifierTags;

			// Comment
		case tags::_Comment_:break;

			// Function
		case tags::_Function_: parse_lang_write_function( subVal , toLang , o );
			break;

			// (y) Lock (Y)
		case tags::_Lock_:goto PostModifiableTagEncolsedWithBrace;

			// (y) Using (y) when CS else Using
		case tags::_Using_:
			if ( toLang == languageToConvertTo::CPP ) parse_lang_write_using_when_cpp( isSkip , o , subVal , toLang , semicolon );
			else goto PostModifiableTagEncolsedWithBrace;
			break;

			// Event
		case tags::_Event_:
			if ( toLang == languageToConvertTo::CPP )throw qcParsingException( "Events aren't supported in QC trans-compiled to C++ (17 With Boost)." , eventNotSupported );
			parse_lang_write_event( subVal , toLang , o );
			break;

			// (n) Add and Remove (n)
		case tags::_Add_:case tags::_Remove_:
			if ( toLang == languageToConvertTo::CPP )throw qcParsingException( "Add & Remove aren't supported in QC trans-compiled to C++ (17 With Boost)." , addRemoveNotSupported );
			goto NoPostModifierTags;

			// If, ElseIf, Switch, Catch, While, ForEach, Lock and (Using when CS)
		default: PostModifiableTagEncolsedWithBrace:
			parse_lang_PMTEWB( o , tagVal , toLang , subVal );
			break;
		}

		// Inserts the subVal when the subVal has no difference.
		if ( subVal != "" && isANoPostModiferTag )
		{
			// Indent
			o << indent( indentLevel + 1 );
			// Write the subVal
			convertTextToLang( o , subVal , toLang , false , false );
			// And insert a semicolon
			o << semicolon << endl;

			isSkip = true;
		}

		// If we don't skip the insertion of a new line and open brace.
		if ( !isSkip )
		{
			// We must insert a new line.
			o << endl;
			// Cases don't have braces
			if ( !isCase ) o << indent << '{' << endl;
		}
		// Comments, Roots and Natives don't need an increment in the indent level
		if ( !( isComment || isRoot || isNative || isAbstract ) ) ++indentLevel;

		// The tag parsing has been finished
		isTagParsed = true;
	}

	// If the text is null then we do nothing.
	if ( text == "" );
	// Comments start with /*
	else if ( isComment )o << "/*" << endl << indent << text << endl;
	// If the tag's direct text are native only.
	else if ( isNative )
	{
		// If the tag's direct text are CPP only
		if ( tagVal == tags::_CPP_ )
		{
			// If the language is C++ then we write the text
			if ( toLang == languageToConvertTo::CPP )goto final_;
			// Else we do not write the elements
		}
		// If the tag's direct text are CS only
		if ( tagVal == tags::_CS_ )
		{
			// If the language is C# then we write the text
			if ( toLang == languageToConvertTo::CS )goto final_;
			// Else we do not write the text
		}
		// If the tag is just native
		else
		{
		final_:
			// Then we write the text as is.
			commonReplacement( text );
			boost::replace_all( text , "[[" , "<" );
			boost::replace_all( text , "]]" , ">" );
			boost::replace_all( text , "[|" , "<" );
			boost::replace_all( text , "|]" , ">" );
			o << text << endl;
		}
	}
	// If the tag is a root
	else if ( isRoot )
	{
		// We convert the text to the language and print it.
		convertTextToLang( o , text , toLang );
		// We insert a new line, for spacing
		o << endl;
	}
	else if ( tagVal == tags::_Interface_ )
	{
		auto _text = wasTagParsed ? "" : text.substr( idx );
		auto& vText = wasTagParsed ? text : _text;
		// We parse the interface text
		parse_lang_parse_interface_text( vText , toLang , o , ";" , "virtual " , "" );
	}
	else if ( isAbstract )
		// The conversions for abstract functions and interface functions are the same.
	{
		parse_lang_parse_interface_text( text , toLang , o , ";" , "virtual " , "abstract " );
	}
	else
	{
		auto _text = wasTagParsed ? "" : text.substr( idx );
		auto& vText = wasTagParsed ? text : _text;
		if ( vText != "" )
		{
			// We convert the text to the language and print it.
			convertTextToLang( o , vText , toLang );
			// We insert a new line only if the last character is not a new line
			if ( vText[ vText.length( ) - 1 ] != '\n' ) o << endl;
		}
	}
}

void commonReplacement( std::string & val )
{
	for ( const auto&[ search , replace ] : opReplacements )
		ReplaceAllSurroundedByNonAlnumChars( val , std::string( search ) , std::string( replace ) );
}

// Equality comparer for chars that considers ' ' == '\t'
struct isEqualConsiderSpace
{
	bool operator()( const char& arg1 , const char& arg2 ) const
	{
		static const auto isSpace = [ ] ( char ch ) { return ch == ' ' || ch == '\t'; };
		if ( isSpace( arg1 ) && isSpace( arg2 ) ) return true;
		return arg1 == arg2;
	}
};

void replaceAllConSpace( std::string& Input , std::string& Search , std::string& Format )
{
	using namespace boost::algorithm;
	// We pad the search string in the front
	Search = " " + Search;
	// and the back.
	Search.push_back( ' ' );
	// We pad the replace string in the front
	Format = " " + Format;
	// and the back.
	Format.push_back( ' ' );
	// We loop through all the supported non alpha-numeric characters for the ends of the search & replace strings
	for ( auto& i : nonAlnumChars )
	{
		// And set the end of the string to the alpha-numeric character.
		Search[ Search.length( ) - 1 ] = Format[ Format.length( ) - 1 ] = i;
		// Search & replace.
		boost::algorithm::find_format_all(
			Input ,
			::boost::algorithm::first_finder( Search , isEqualConsiderSpace( ) ) ,
			::boost::algorithm::const_formatter( Format ) );
	}
	// We reset the end of the string
	Search[ Search.length( ) - 1 ] = Format[ Format.length( ) - 1 ] = ' ';
	// We loop through all the supported non alpha-numeric characters for the starts of the search & replace strings
	for ( auto& i : nonAlnumChars )
	{
		// And set the start of the string to the alpha-numeric character.
		Search[ 0 ] = Format[ 0 ] = i;
		// Search & replace.
		boost::algorithm::find_format_all(
			Input ,
			::boost::algorithm::first_finder( Search , isEqualConsiderSpace( ) ) ,
			::boost::algorithm::const_formatter( Format ) );
	}
	// We reset the start of the string
	Search[ 0 ] = Format[ 0 ] = ' ';
	// We loop through all the supported non alpha-numeric characters for the starts of the search & replace strings,
	for ( auto& i : nonAlnumChars )
	{
		// And set the start of the string to the alpha-numeric character.
		Search[ 0 ] = Format[ 0 ] = i;
		// We loop through all the supported non alpha-numeric characters for the ends of the search & replace strings again for correct-ness,
		for ( auto& j : nonAlnumChars )
		{
			// And set the end of the string to the alpha-numeric character.
			Search[ Search.length( ) - 1 ] = Format[ Format.length( ) - 1 ] = j;
			// Search & replace.
			boost::algorithm::find_format_all(
				Input ,
				::boost::algorithm::first_finder( Search , isEqualConsiderSpace( ) ) ,
				::boost::algorithm::const_formatter( Format ) );
		}
	}

	// We reset the end of the string
	Search[ Search.length( ) - 1 ] = Format[ Format.length( ) - 1 ] = ' ';
	// and we reset the start of the string.
	Search[ 0 ] = Format[ 0 ] = ' ';
	// Search & replace.
	boost::algorithm::find_format_all(
		Input ,
		::boost::algorithm::first_finder( Search , isEqualConsiderSpace( ) ) ,
		::boost::algorithm::const_formatter( Format ) );
}

void replaceInpAtEndSNRPaddedAtStart( std::string &search , std::string &replace , std::string & input )
{
	// Loop through all the supported non alpha-numeric characters for the starts of the search & replace strings of the string to replace the end of the input string,
	for ( auto& i : nonAlnumChars )
	{
		// Set start to i
		search[ 0 ] = replace[ 0 ] = i;
		// Search and replace at end
		if ( boost::algorithm::ends_with( input , search ) )
		{
			input.erase( input.end( ) - search.size( ) , input.end( ) );
			input.append( replace );
		}
	}
	// Reset start to space
	search[ 0 ] = replace[ 0 ] = ' ';
	// Search and replace at end
	if ( boost::algorithm::ends_with( input , search ) )
	{
		input.erase( input.end( ) - search.size( ) , input.end( ) );
		input.append( replace );
	}
}

void replaceInpAtStartSNRPaddedAtEnd( std::string &search , std::string &replace , std::string & input )
{
	for ( auto& i : nonAlnumChars )
	{
		// Set end to i
		search[ search.size( ) - 1 ] = replace[ replace.size( ) - 1 ] = i;
		// Search and replace at start
		if ( boost::algorithm::starts_with( input , search ) )
		{
			input.erase( input.begin( ) , input.begin( ) + search.size( ) );
			input.insert( 0 , replace );
		}
	}
	// Reset end to space
	search[ search.size( ) - 1 ] = replace[ replace.size( ) - 1 ] = ' ';
	// Search and replace at start
	if ( boost::algorithm::starts_with( input , search ) )
	{
		input.erase( input.begin( ) , input.begin( ) + search.size( ) );
		input.insert( 0 , replace );
	}
}

void ReplaceAllSurroundedByNonAlnumChars( std::string& input , std::string search , std::string replace )
{
	// if the input is the same as the search
	if ( input == search )
	{
		// Set the input to the search
		input = replace;
		// No need to look any further
		return;
	}
	// Do the regular replacements
	replaceAllConSpace( input , search , replace );
	// We remove the back of the extended search & replace.
	search.pop_back( );
	replace.pop_back( );
	replaceInpAtEndSNRPaddedAtStart( search , replace , input );
	// Push back the removed char and erase the start space.
	search.push_back( ' ' );
	search.erase( search.begin( ) );
	replace.push_back( ' ' );
	replace.erase( replace.begin( ) );
	// Loop through all the supported non alpha-numeric characters for the ends of the search & replace strings of the string to replace the start of the input string,
	replaceInpAtStartSNRPaddedAtEnd( search , replace , input );
	// Pop the added char.
	search.pop_back( );
	replace.pop_back( );
}

void convStr( std::string & val , languageToConvertTo toLang , bool processNewLines , bool applyException )
{
	using namespace boost::algorithm;
	replaceAngleBrace( val );
	// If the language is not C++
	if ( toLang != languageToConvertTo::CPP )
	{
		// Replace all "::" with "."
		replace_all( val , lcolon_colon , ldot );
		// But we have to revert "Global." to "Global::"
		std::string search = " Global." , replace = " Global::";
		// So we replace all " Global." with " Global::"
		boost::algorithm::find_format_all(
			val ,
			::boost::algorithm::first_finder( search , isEqualConsiderSpace( ) ) ,
			::boost::algorithm::const_formatter( replace ) );
		// Then we loop through all the non-alpha-numeric characters
		for ( auto& i : nonAlnumChars )
		{
			// Set the start of the search and replace to the corresponding non-alpha-numeric character.
			search[ 0 ] = replace[ 0 ] = i;
			// Search and replace
			boost::algorithm::find_format_all(
				val ,
				::boost::algorithm::first_finder( search , isEqualConsiderSpace( ) ) ,
				::boost::algorithm::const_formatter( replace ) );
		}
		// Erase the start spaces
		search.erase( search.begin( ) );
		replace.erase( replace.begin( ) );
		// If the value starts with "Global."
		if ( boost::algorithm::starts_with( val , search ) )
		{
			// Erase the starting "Global."
			val.erase( val.begin( ) , val.begin( ) + search.size( ) );
			// And append at the start "Global::"
			val.insert( 0 , replace );
		}
	}
	for ( const auto&[ search , replace ] : replacements.at( toLang ) )
		ReplaceAllSurroundedByNonAlnumChars( val , std::string( search ) , std::string( replace ) );
	commonReplacement( val );
	if ( processNewLines )
	{
		replace_all( val , "\n" , ";\n" );
		replace_all( val , " _;" , "" );
		replace_all( val , "\t_;" , "\t" );
		replace_all( val , " ;" , ";" );
		replace_all( val , "\t;" , ";" );
		replace_all( val , "\n;" , "\n" );
	}
	if ( val[ 0 ] == ';' && ( applyException ? ( val[ 1 ] != '\n' ) : true ) )
		val = val.substr( 1 );
}

void replaceAngleBrace( std::string & val )
{
	boost::algorithm::replace_all( val , "[[" , "<" );
	boost::algorithm::replace_all( val , "]]" , ">" );
}

void checkStr( const std::string_view & val , languageToConvertTo toLang )
{
	using namespace std::string_literals;
	std::list<char> closeBraces;
	size_t i = 0;
	bool isEnabled = true;
	for (
		auto iter = val.begin( ) , end = val.end( );
		iter != end;
		iter++ , i++ )
	{
		if ( !isEnabled )
		{
			if ( isStrQuote( i , iter ) ) isEnabled = true;
			continue;
		}
		BOOST_SCOPE_EXIT_ALL( &isEnabled , &i , &iter )
		{
			if ( isEnabled && isStrQuote( i , iter ) )
				isEnabled = false;
		};
		auto& chr = *iter;
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
						val.find_last_of( '\n' , i + 1 ) + 1 ,
						getFirstNewline( val , i ) );
					throw qcParsingException( (
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
	if ( errStr != "" ) throw qcParsingException( errStr , braceMismatch );
}

std::string WriteDoLoop( std::string & val , std::ostream & o , languageToConvertTo toLang )
{
	using namespace boost::algorithm;

	auto loopCondIter = boost::find( val , first_finder( "While " , isEqualConsiderSpace( ) ) ).begin( );
	bool isWhile = true;
	if ( loopCondIter == val.end( ) )
	{
		loopCondIter = boost::find( val , first_finder( "Until " , isEqualConsiderSpace( ) ) ).begin( );
		if ( loopCondIter == val.end( ) )
			throw qcParsingException( "Invalid Do loop syntax" , invalidLoopSyntax );
		isWhile = false;
	}

	auto loopCondIdx = loopCondIter - val.begin( );
	auto loopCondEndIdx = getFirstNewline( val , loopCondIdx );
	auto loopCond = val.substr( loopCondIdx + 6 , loopCondEndIdx - loopCondIdx - 6 );
	o << " while( ";
	if ( !isWhile )o << " ( !";
	o << loopCond;
	if ( !isWhile )o << " )";
	o << " );" << std::endl;
	return val.substr( loopCondEndIdx );
}

void convertTextToLang( std::ostream& o , std::string & text , languageToConvertTo toLang , bool doStartIndent , bool processNewLines , bool remCont )
{
	using namespace boost::algorithm;
	replace_all( text , "|]" , ">" );
	replace_all( text , "[|" , "<" );
	checkStr( text , toLang );

	size_t idx = 0;
	bool start = true;
	bool isStr = false;
	std::list<std::string> lst;
	bool applyNot = false;
	if ( isStrQuote( 0 , text.begin( ) ) )applyNot = true;
	loop_split(
		text ,
		[ ] ( auto&& idx , auto&& iter ) { return isStrQuote( idx , iter ); } ,
		[
			&applyNot , &idx , &toLang , &processNewLines , &isStr , &o , &doStartIndent , &start , &remCont
		] ( std::string&& val )
		{
			bool isEven = applyNot ? ( idx % 2 ) != 0 : ( idx % 2 ) == 0;
			if ( isEven )
			{
				replaceAngleBrace( val );
				convStr( val , toLang , processNewLines , isStr );
				loop_split(
					val , isNewLine ,
					[ &o , &doStartIndent , &start , &idx , &processNewLines , &remCont ] ( std::string&& val )
					{
						if ( remCont ) ReplaceNewLines( val );
						trim( val );
						if ( val.size( ) == 0 )return;
						if ( !start )o << std::endl;
						if ( !doStartIndent && start );
						else o << indent;
						o << val;
						start = false;
					}
				);
				isStr = false;
			}
			else
			{
				if ( toLang == languageToConvertTo::CS ) replace_all( val , R"(")" , R"("")" );
				replace_all( val , "\\`" , "`" );
				if ( toLang == languageToConvertTo::CS ) o << "@\"" << val << "\"";
				else if ( toLang == languageToConvertTo::CPP ) o << "R\"(" << val << ")\"";
				isStr = true;
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
		if ( pos == ( underscore + 1 ) && ( underscore >= 1 || isspace( val[ underscore - 1 ] ) ) )
			pos = val.find_first_of( '\n' , pos + 1 );
		else return pos;
	}
}

void traversePath( boost::filesystem::path &arg , languageToConvertTo toLang , bool indent );

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

	try
	{
		languageToConvertTo toLang( fromString( vm[ "language" ].as<std::string>( ) ) );
		bool indent = vm.count( "indent" );

		cout << "Full parsing started" << endl;

		++indentLevel;
		if ( is_regular_file( arg ) ) parse_file( arg , toLang );
		else if ( is_directory( arg ) ) traversePath( arg , toLang , indent );
		else throw qcParsingException( "Path is neither a (regular) file not a directory" , notFileNorDir );
		--indentLevel;
	}
	catch ( const qcParsingException& val )
	{
		--indentLevel;
		cout << "Full parsing failed" << endl;
		return val.errCode;
	}
	cout << "Full parsing succeeded" << endl;
	return 0;
}

void traversePath( boost::filesystem::path &arg , languageToConvertTo toLang , bool indent )
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
			if ( is_regular_file( narg ) )parse_file( narg , toLang );
			else if ( is_directory( narg ) )traversePath( narg , toLang , indent );
			else
				throw qcParsingException( "Path is neither a (regular) file not a directory" , notFileNorDir );
		}
		cout << ::indent << truncname << " Dir. parsing succeeded" << endl;
		--indentLevel;
	}
	catch ( qcParsingException& v )
	{
		--indentLevel;
		cout << ::indent << truncname << " Dir. parsing failed" << endl;
		throw v;
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
		throw qcParsingException( "Could not open input file: " + filename.string( ) , unableToOpenFileForInput );
	}

	in.unsetf( ios::skipws ); // No white space skipping!
	in.seekg( 0 , ios::end );
	storage.reserve( in.tellg( ) );
	in.seekg( 0 , ios::beg );

	auto inStart = istream_iterator<char>( in ) , inEnd = istream_iterator<char>( );
	for ( ; inStart != inEnd; ++inStart )
	{
		auto val = *inStart;
		if ( val != '\r' ) storage.push_back( val );
	}

	using boost::spirit::ascii::space;
	iter = storage.begin( );
	end = storage.end( );
	succeed = phrase_parse( iter , end , qc , space , ast );
}

void parse_file( boost::filesystem::path &filename , languageToConvertTo toLang )
{
	auto ext = filename.extension( ).string( );
	if ( validQCFileExtentions.find( ext ) == validQCFileExtentions.end( ) ) return;

	using namespace std;
	using namespace std::string_literals;

	bool succeeded;
	string::const_iterator iter , end;
	qc_grammar qc;
	qc_data ast;
	string storage; // We will read the contents here.

	auto truncname = filename.filename( ).string( );

	cout << indent << truncname << " File parsing started" << endl;

	parse_file( filename , storage , iter , end , succeeded , qc , ast );
	if ( succeeded && iter == end )
	{
		auto csfile = filename.replace_extension( QCExtention_Language_Extention.at( ext ).at( toLang ) ).string( );
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
			if ( !o )
			{
				throw qcParsingException( "Could not open output file: " + filename.string( ) , unableToOpenFileForOutput );
			}
			parser( ast , o , toLang );
			__end__.restore( );
			cout << indent << truncname << " File parsing succeeded" << endl;
		}
		catch ( qcParsingException& )
		{
			cout << indent << truncname << " File parsing failed" << endl;
			throw;
		}
	}
	else
	{
		cout << indent << truncname << " File parsing failed" << endl;
		throw qcParsingException( "File parsing failed" , parsingFailed );
	}
}

inline qcParsingException::qcParsingException( const std::string& first , int errCode ) :errCode( errCode )
{ std::cerr << "Error: " << first << std::endl << "STACKTRACE:" << std::endl << boost::stacktrace::stacktrace( ) << std::endl; }
