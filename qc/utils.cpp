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

caref l_Gives_ = " Gives ";
caref l_To_ = " To ";
caref lTo = "To";
caref l_Step_ = " Step ";
caref lStep = "Step";
caref l_Of_ = " Of ";
caref lcolon_colon = "::";
caref ldot = ".";
const std::unordered_map<std::string_view , std::unordered_map<std::string_view , std::string_view>> replacements
{
	{
		literal_cs,
		{
{"CPPInclude"	,"//"},
{"AlsoGive"		,"yield return"},
{"Finalize"		,"yield break"},
{"Give"			,"return"},
{"GiveIn"		,"return"},
{"GenOf"		,"System.Collections.Generic.IEnumerable"},
{"IterGenOf"	,"System.Collections.Generic.IEnumerator"},
{"Gen"			,"System.Collections.IEnumerable"},
{"IterGen"		,"System.Collections.IEnumerator"},
{"Abstract"		,"abstract"},
{"Ascending"	,"ascending"},
{"Asynchronous"	,"async"},
{"Await"		,"await"},
{"Base"			,"base"},
{"Bool"			,"bool"},
{"Break"		,"break"},
{"Byte"			,"byte"},
{"Case"			,"case"},
{"Class"		,"class"},
{"Char"			,"char"},
{"WChar"		,"char"},
{"Checked"		,"checked"},
{"Constant"		,"const"},
{"CPPConst"		,""},
{"Continue"		,"continue"},
{"Decimal"		,"decimal"},
{"Default"		,"default"},
{"Delegate"		,"delegate"},
{"Double"		,"double"},
{"Enum"			,"enum"},
{"Event"		,"event"},
{"Explicit"		,"explicit"},
{"Extern"		,"extern"},
{"False"		,"false"},
{"Float"		,"float"},
{"Goto"			,"goto"},
{"HashCode"		,"int"},
{"Implicit"		,"implicit"},
{"In"			,"in"},
{"Int"			,"int"},
{"Interface"	,"interface"},
{"Internal"		,"internal"},
{"Is"			,"is"},
{"Long"			,"long"},
{"New"			,"new"},
{"CPPNew"		,"new"},
{"Null"			,"null"},
{"Object"		,"object"},
{"Operator"		,"operator"},
{"Out"			,"out"},
{"CPPOut"		,""},
{"Override"		,"override"},
{"CPPOverride"	,""},
{"CSOverride"	,"override"},
{"Parameters"	,"params"},
{"Private"		,"private"},
{"Protected"	,"protected"},
{"Public"		,"public"},
{"ReadOnly"		,"readonly"},
{"Ref"			,"ref"},
{"CPPRef"		,""},
{"SByte"		,"sbyte"},
{"Sealed"		,"sealed"},
{"Short"		,"short"},
{"Single"		,"float"},
{"Float"		,"float"},
{"Double"		,"double"},
{"StackAlloc"	,"stackalloc"},
{"Static"		,"static"},
{"String"		,"string"},
{"StringRef"	,"string"},
{"WString"		,"string"},
{"WStringRef"	,"string"},
{"Structure"	,"struct"},
{"Switch"		,"switch"},
{"Me"			,"this"},
{"Throw"		,"throw"},
{"True"			,"true"},
{"Virtual"		,"virtual"},
{"Nothing"		,"void"},
{"Volatile"		,"volatile"},
{"Var"			,"var"},
{"CSVar"		,"var"},
{"Using Namespace QCCore"	,"using QCCore; using static QCCore.QCCoreCls"},
{"Using Namespace"	,"using"},
{"Using"		,"using"},
{"UShort"		,"ushort"},
{"UInt"			,"uint"},
{"ULong"		,"ulong"},
{"Global"		,"global::" },
{"PriMutex"		,"object" },
{"Get"			,"get" },
{"Set"			,"set" },
{"Dynamic"		,"dynamic" },
{"Wide"			,"" },
{"REM"			,"//" },
		}
	},
		{
		literal_cpp,
		{
{"CPPInclude QCCore","#include <QCCore.h>"},
{"CPPInclude","#include"},
{"Using Namespace","using namespace"},
{"Using","using"},
{"Constant String","constexpr ::std::string_view" },
{"Constant WString","constexpr ::std::wstring_view" },
{"Me","(*this)"},
{"AlsoGive","co_yield"},
{"Finalize","return"},
{"Give","return"},
{"GiveIn","return"},
{"Gen","std::experimental::generator<Object>"},
{"GenOf","std::experimental::generator"},
{"Await","co_await"},
{"Base","base"},
{"Break","break"},
{"Case","case"},
{"Class","class"},
{"Checked",""},
{"CPPConst","const"},
{"Constant","constexpr"},
{"Continue","continue"},
{"Decimal","long long"},
{"Default","default"},
{"Delegate","[&]"},
{"Double","double"},
{"Enum","enum"},
{"Event","event"},
{"Explicit","explicit"},
{"Extern","extern"},
{"False","false"},
{"Float","float"},
{"Goto","goto"},
{"Implicit",""},
{"Interface","interface"},
{"CPPNew","new"},
{"New",""},
{"Null","nullptr"},
{"Operator","operator"},
{"Out",""},
{"CPPOut","&"},
{"Override",""},
{"CPPOverride","override"},
{"CSOverride",""},
{"Private","private:"},
{"Protected","protected:"},
{"Public","public:"},
{"ReadOnly","const"},
{"Ref",""},
{"CPPRef","&"},
{"Sealed","final"},
{"StackAlloc",""},
{"Static","static"},
{"Structure","struct"},
{"Switch","switch"},
{"Throw","throw"},
{"True","true"},
{"Virtual","virtual"},
{"Nothing","void"},
{"Volatile","volatile"},
{"Var","auto"},
{"CSVar",""},
{"Global","::"},
{"PriMutex","::std::mutex"},
{"Get","get" },
{"Set","set" },
{"Dynamic","auto"},
{ "Wide","L" },
{"REM","//" },
		}
	}
};
const std::string_view nonAlnumChars { "`~!#$%^&*()-=+[]{}|\\;:'\",./?\n<>" };

#undef caref

void ReplaceAllSurroundedByNonAlnumChars( std::string& input , std::string_view search , std::string replace );

static const std::unordered_map<std::string , std::string> languageExtensionDict
{
	{literal_cs,".cs" },
	{literal_cpp,"" }
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
	literal_cs,
	literal_cpp
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

void parseLang( std::ostream& o , bool& isTagParsed ,
				std::string & text , const tags & tagVal ,
				const std::string & toLang )
{
	using namespace std;
	auto
		isComment = tagVal == tags::_Catch_ ,
		isRoot = false ,
		isInterface = false ,
		isNative = tagVal == tags::_Native_ ,
		isSkip = false ,
		isCase = false ,
		cppOnly = false ,
		csOnly = false ,
		isCpp = toLang == literal_cpp ,
		isCs = toLang == literal_cs;
	auto idx = ( isTagParsed || isComment ) ? 0 : getFirstNewline( text );
	boost::trim_left( text );
	string subVal;
	if ( idx == string::npos )
		idx = text.length( );
	subVal = text.substr( 0 , idx );
	bool wasTagParsed = isTagParsed;
	auto semicolon = ( subVal.length( ) != 0 ? ";" : "" );
	boost::trim( subVal );
	if ( !isTagParsed )
		o << indent;

	switch ( tagVal )
	{
	case tags::_Blk_: goto UUCGSETFDCFTCDFTD;
	case tags::_QC_:
	{
		isRoot = true;
		isSkip = true;
	}
	break;

	case tags::_Class_: case tags::_Structure_: case tags::_Namespace_:
		goto _CSNE_;

	case tags::_Interface_:
	{
		isInterface = true;
		if ( !isTagParsed )
		{
			convStr( subVal , toLang );
			o << ( isCpp ? "struct " : "interface " ) << subVal;
		}
	}
	break;

	case tags::_Enum_:
		// Class Structure Namespace Enum
	_CSNE_:
	{
		if ( !isTagParsed )
		{
			convStr( subVal , toLang , false );
			o << to_string( tagVal , toLang ) << ' ' << subVal;
		}
	}
	break;

	case tags::_NativeCPP_:cppOnly = true; goto _Native_;
	case tags::_NativeCS_:csOnly = true; goto _Native_;
	case tags::_Native_:
	_Native_:
	{
		isNative = true;
		isSkip = true;
	}
	break;

	case tags::_Property_:
	{
		if ( isCpp )throw throwVal( "Properties aren't supported in QC trans-compiled to C++ (17 With Boost)." , propertyNotSupported );
		if ( !isTagParsed )
		{
			convStr( subVal , toLang );
			auto ofIdx = boost::find_first( subVal , l_Of_ ).begin( );
			if ( ofIdx == subVal.end( ) )throw throwVal( "The name of the property must come before \"Of\" and the type must come after \"Of\"." , invalidPropertySyntax );
			o <<
				string( ofIdx + 4 , subVal.end( ) ) <<
				' ' <<
				subVal.substr( 0 , ofIdx - subVal.begin( ) );
		}
	}
	break;

	case tags::_Indexer_:
	{
		if ( isCpp )throw throwVal( "Indexers aren't supported in QC trans-compiled to C++ (17 With Boost)." , indexerNotSupported );
		if ( !isTagParsed )
		{
			std::stringstream strm;
			convertTextToLang( strm , subVal , toLang , false , false );
			subVal = strm.str( );
			auto ofIdx = boost::find_first( subVal , l_Of_ ).begin( );
			if ( ofIdx == subVal.end( ) )throw throwVal( "The argument list of the indexer must come before \"Of\" and the type must come after \"Of\"." , invalidIndexerSyntax );
			o <<
				string( ofIdx + 4 , subVal.end( ) ) <<
				" this" <<
				subVal.substr( 0 , ofIdx - subVal.begin( ) );
		}
	}
	break;

	case tags::_Unsafe_:
	case tags::_Unchecked_:
	case tags::_Checked_:goto UUCGSETFDCFTCDFTD;
	case tags::_Get_:
	case tags::_Set_:
		if ( isCpp )throw throwVal( "Get & Set aren't supported in QC trans-compiled to C++ (17 With Boost)." , getSetNotSupported );
		goto UUCGSETFDCFTCDFTD;

	case tags::_If_:
	case tags::_ElseIf_: goto _default_;

	case tags::_Else_: goto UUCGSETFDCFTCDFTD;

	case tags::_Switch_: goto _default_;

	case tags::_Case_:
	case tags::_FTCase_:
	{
		if ( !isTagParsed )
		{
			o << "case ";
			convertTextToLang( o , subVal , toLang , false , false );
			o << ":";
		}
		isCase = true;
	}
	break;

	case tags::_Default_:
	case tags::_FTDefault_:
		isCase = true;
		goto UUCGSETFDCFTCDFTD;

	case tags::_Try_: goto UUCGSETFDCFTCDFTD;
	case tags::_Catch_: goto _default_;

	case tags::_Finally_:
		if ( isCpp )
			throw throwVal( "Finally blocks aren't supported in QC trans-compiled to C++ (17 With Boost)." , finallyNotSupported );
	case tags::_Do_:
		// Unsafe Unchecked Checked Get Set Else Try Finally Do Case FallThroughCase Default FallThroughDefault
	UUCGSETFDCFTCDFTD:
	{
		if ( !isTagParsed )
		{
			o << to_string( tagVal , toLang );
			o << endl;
			if ( !isCase ) o << indent << '{' << endl;
		}
		isSkip = true;
		if ( subVal != "" )
		{
			o << indent( indentLevel + 1 );
			convertTextToLang( o , subVal , toLang , false , false );
			o << semicolon << endl;
		}
	}
	break;

	case tags::_Until_:
	{
		if ( !isTagParsed )
		{
			o << "while( !( ";
			convertTextToLang( o , subVal , toLang , false , false );
			o << " ) )";
		}
	}
	break;

	case tags::_While_: goto _default_;

	case tags::_For_:
	{
		if ( !isTagParsed )
		{
			using qi::lexeme;
			using qi::lit;
			using qi::phrase_parse;
			using ascii::alnum;
			using ascii::space;
			using ascii::char_;

			std::stringstream strm;
			convertTextToLang( strm , subVal , toLang , false , false );
			subVal = strm.str( );
			auto subValStart = subVal.begin( );
			auto subValEnd = subVal.end( );
			std::vector<std::string> vec;
			bool newVar = false;
			auto res = phrase_parse(
				subValStart ,
				subValEnd ,
				lexeme[ -( lit( "var " )[ phx::ref( newVar ) = true ] ) >> +alnum ] >> '=' >>
				lexeme[ +( char_ - l_To_ ) ] >> lTo >>
				lexeme[ +( char_ - l_Step_ ) ] >>
				-( lStep >> lexeme[ +char_ ] ) ,
				space , vec );
			if ( vec.size( ) == 4 )
			{
				o <<
					boost::format(
						"for(%5% %1% = %2%; %1% <= %3%; %1% += %4% )" )
					% vec[ 0 ]
					% vec[ 1 ]
					% vec[ 2 ]
					% vec[ 3 ]
					% ( newVar ? replacements.at( toLang ).at( "Var" ) : "" );
			}
			else if ( vec.size( ) == 3 )
			{
				o <<
					boost::format(
						"for(%4% %1% = %2%; %1% <= %3%; ++%1% )" )
					% vec[ 0 ]
					% vec[ 1 ]
					% vec[ 2 ]
					% ( newVar ? replacements.at( toLang ).at( "Var" ) : "" );
			}
			else
			{
				throw throwVal( "Invalid For loop syntax" , invalidLoopSyntax );
			}
		}
	}
	break;

	case tags::_RevFor_:
	{
		if ( !isTagParsed )
		{
			using qi::lexeme;
			using qi::lit;
			using qi::phrase_parse;
			using ascii::alnum;
			using ascii::space;
			using ascii::char_;

			std::stringstream strm;
			convertTextToLang( strm , subVal , toLang , false , false );
			subVal = strm.str( );
			auto subValStart = subVal.begin( );
			auto subValEnd = subVal.end( );
			std::vector<std::string> vec;
			bool newVar = false;
			auto res = phrase_parse(
				subValStart ,
				subValEnd ,
				lexeme[ -( lit( "Var " )[ phx::ref( newVar ) = true ] ) >>
				+alnum ] >> '=' >>
				lexeme[ +( char_ - l_To_ ) ] >> lTo >>
				lexeme[ +( char_ - l_Step_ ) ] >>
				-( lStep >> lexeme[ +char_ ] ) ,
				space , vec );
			if ( vec.size( ) == 4 )
			{
				o <<
					boost::format(
						"for( %5% %1% = %3%; %1% >= %2%; %1% -= %4% )" )
					% vec[ 0 ]
					% vec[ 1 ]
					% vec[ 2 ]
					% vec[ 3 ]
					% ( newVar ? replacements.at( toLang ).at( "Var" ) : "" );
			}
			else if ( vec.size( ) == 3 )
			{
				o <<
					boost::format(
						"for( %4% %1% = %3%; %1% >= %2%; --%1% )" )
					% vec[ 0 ]
					% vec[ 1 ]
					% vec[ 2 ]
					% ( newVar ? replacements.at( toLang ).at( "Var" ) : "" );
			}
			else
			{
				throw throwVal( "Invalid RevFor loop syntax" , invalidLoopSyntax );
			}
		}
	}
	break;

	case tags::_ForEach_:
		if ( isCpp )
		{
			if ( !isTagParsed )
			{
				o << to_string( tagVal , toLang ) << '(';
				ReplaceAllSurroundedByNonAlnumChars( subVal , "In" , ":" );
				convertTextToLang( o , subVal , toLang , false , false );
				o << ")";
			}
		}
		else goto _default_;
		break;

	case tags::_ForEver_: goto UUCGSETFDCFTCDFTD;

	case tags::_Comment_:
	{
		isComment = true;
		isSkip = true;
	}
	break;

	case tags::_Function_:
	{
		using namespace boost::algorithm;

		if ( !isTagParsed )
		{
			std::stringstream strm;
			convertTextToLang( strm , subVal , toLang , false , false );
			subVal = strm.str( );
			if ( boost::contains( subVal , l_Gives_ ) )
			{
				auto givesIdx = boost::find_first( subVal , l_Gives_ ).begin( );
				o <<
					string( givesIdx + 7 , subVal.end( ) ) <<
					' ' <<
					subVal.substr( 0 , givesIdx - subVal.begin( ) );
			}
			else o << "void " << subVal;
		}
	}
	break;

	case tags::_Lock_:goto _default_;

	case tags::_Using_:
		if ( isCpp && !isTagParsed )
		{
			isSkip = true;
			o << indent << '{' << endl;
			o << indent( indentLevel + 1 );
			convertTextToLang( o , subVal , toLang , false , false );
			o << semicolon << endl;
		}
		else goto _default_;
		break;

	case tags::_Event_:
	{
		if ( isCpp )throw throwVal( "Events aren't supported in QC trans-compiled to C++ (17 With Boost)." , eventNotSupported );
		if ( !isTagParsed )
		{
			convStr( subVal , toLang );
			auto ofIdx = boost::find_first( subVal , l_Of_ ).begin( );
			if ( ofIdx == subVal.end( ) )throw throwVal( "The name of the event must come before \"Of\" and the type must come after \"Of\"." , invalidPropertySyntax );
			o <<
				" event " <<
				string( ofIdx + 4 , subVal.end( ) ) <<
				' ' <<
				subVal.substr( 0 , ofIdx - subVal.begin( ) );
		}
	}
	break;

	case tags::_Add_:
	case tags::_Remove_:
		if ( isCpp )throw throwVal( "Add & Remove aren't supported in QC trans-compiled to C++ (17 With Boost)." , addRemoveNotSupported );
		goto UUCGSETFDCFTCDFTD;

	default: _default_:
		{
			if ( !isTagParsed )
			{
				o << to_string( tagVal , toLang ) << '(';
				convertTextToLang( o , subVal , toLang , false , false );
				o << ")";
				if ( tagVal == tags::_Lock_ && isCpp )o << ";";
			}
		}
		break;
	}

	if ( !isTagParsed )
	{
		if ( !isSkip )
		{
			o << endl;
			if ( !isCase ) o << indent << '{' << endl;
		}
		if ( !( isComment || isRoot || isNative ) )
			++indentLevel;
		isTagParsed = true;
	}

	if ( text == "" || isspace( text[ 0 ] ) );
	else if ( isComment )o << "/*" << endl << indent << text << endl;
	else if ( isNative )
	{
		if ( cppOnly ) { if ( isCpp )goto final_; }
		if ( csOnly ) { if ( isCs )goto final_; }
		else
		{
		final_:
			o << text << endl;
		}
	}
	else if ( isRoot )
	{
		convertTextToLang( o , text , toLang );
		o << endl;
	}
	else if ( isInterface )
	{
		auto _text = wasTagParsed ? "" : text.substr( idx );
		auto& vText = wasTagParsed ? text : _text;
		replaceAngleBrace( vText );
		checkStr( vText , toLang );
		convStr( vText , toLang , false );
		boost::trim( vText );
		if ( vText == "" )return;
		loop_split(
			vText ,
			[ ] ( auto&& i , auto&& iter )
			{
				return *iter == '\n' &&
					!( i <= 2 ? false :
					( *( iter - 1 ) == '_' && isspace( *( iter - 2 ) ) ) );
			} ,
			[ &o , &semicolon , &isCpp , &toLang ] ( string&& subVal )
			{
				boost::trim( subVal );
				o << indent;
				if ( isCpp ) o << "virtual ";
				if ( boost::contains( subVal , l_Gives_ ) )
				{
					auto givesIdx = boost::find_first( subVal , l_Gives_ ).begin( );
					auto retVal = string( givesIdx + 7 , subVal.end( ) );
					convertTextToLang( o , retVal , toLang , false , false );
					o << ' ';
					auto funcSig = subVal.substr( 0 , givesIdx - subVal.begin( ) );
					convertTextToLang( o , funcSig , toLang , false , false );
				}
				else o << "void " << subVal;
				if ( isCpp ) o << " = 0";
				o << semicolon << endl;
			}
			);
	}
	else
	{
		auto _text = wasTagParsed ? "" : text.substr( idx );
		auto& vText = wasTagParsed ? text : _text;
		if ( vText != "" )
		{
			convertTextToLang( o , vText , toLang );
			if ( vText[ vText.length( ) - 1 ] != '\n' )
				o << endl;
		}
	}
}

void commonReplacement( std::string & val )
{
	using namespace boost::algorithm;

	ReplaceAllSurroundedByNonAlnumChars( val , "LessOrEq" , "<=" );
	ReplaceAllSurroundedByNonAlnumChars( val , "MoreOrEq" , ">=" );
	ReplaceAllSurroundedByNonAlnumChars( val , "NotEquals" , "!=" );
	ReplaceAllSurroundedByNonAlnumChars( val , "Equals" , "==" );
	ReplaceAllSurroundedByNonAlnumChars( val , "Less" , "<" );
	ReplaceAllSurroundedByNonAlnumChars( val , "More" , ">" );
	ReplaceAllSurroundedByNonAlnumChars( val , "LeftShiftSet" , "<<=" );
	ReplaceAllSurroundedByNonAlnumChars( val , "RightShiftSet" , ">>=" );
	ReplaceAllSurroundedByNonAlnumChars( val , "LeftShift" , "<<" );
	ReplaceAllSurroundedByNonAlnumChars( val , "RightShift" , ">>" );
}

struct isEqualConsiderSpace
{
	bool operator()( const char& arg1 , const char& arg2 ) const
	{
		static const auto isSpace = [ ] ( char ch ) { return ch == ' ' || ch == '\t'; };
		if ( isSpace( arg1 ) && isSpace( arg2 ) )
		{
			return true;
		}
		return arg1 == arg2;
	}
};

inline void replaceAllConSpace(
	std::string& Input ,
	std::string Search ,
	std::string Format )
{
	using namespace boost::algorithm;
	Search = " " + Search;
	Search.push_back( ' ' );
	Format = " " + Format;
	Format.push_back( ' ' );
	for ( auto& i : nonAlnumChars )
	{
		Search[ Search.length( ) - 1 ] =
			Format[ Format.length( ) - 1 ] = i;
		boost::algorithm::find_format_all(
			Input ,
			::boost::algorithm::first_finder( Search , isEqualConsiderSpace( ) ) ,
			::boost::algorithm::const_formatter( Format ) );
	}
	Search[ Search.length( ) - 1 ] = Format[ Format.length( ) - 1 ] = ' ';
	for ( auto& i : nonAlnumChars )
	{
		Search[ 0 ] = Format[ 0 ] = i;
		boost::algorithm::find_format_all(
			Input ,
			::boost::algorithm::first_finder( Search , isEqualConsiderSpace( ) ) ,
			::boost::algorithm::const_formatter( Format ) );
	}
	Search[ 0 ] = Format[ 0 ] = ' ';
	for ( auto& i : nonAlnumChars )
	{
		Search[ 0 ] = Format[ 0 ] = i;
		for ( auto& j : nonAlnumChars )
		{
			Search[ Search.length( ) - 1 ] =
				Format[ Format.length( ) - 1 ] = j;
			boost::algorithm::find_format_all(
				Input ,
				::boost::algorithm::first_finder( Search , isEqualConsiderSpace( ) ) ,
				::boost::algorithm::const_formatter( Format ) );
		}
	}

	Search[ Search.length( ) - 1 ] = Format[ Format.length( ) - 1 ] = ' ';
	Search[ 0 ] = Format[ 0 ] = ' ';
	boost::algorithm::find_format_all(
		Input ,
		::boost::algorithm::first_finder( Search , isEqualConsiderSpace( ) ) ,
		::boost::algorithm::const_formatter( Format ) );
}

//Replace All Surrounded By Non Alphanumeric Characters
void ReplaceAllSurroundedByNonAlnumChars(
	std::string& input ,
	std::string_view search ,
	std::string replace )
{
	replaceAllConSpace(
		input ,
		std::string( search ) ,
		replace );
	if ( boost::algorithm::ends_with( input , search ) )
	{
		input =
			input.substr( 0 , input.size( ) - search.size( ) ) +
			replace;
	}
	if ( boost::algorithm::starts_with( input , search ) )
		input = replace + input.substr( search.length( ) );
}

void convStr( std::string & val , const std::string & toLang , bool processNewLines , bool applyException )
{
	using namespace boost::algorithm;
	replaceAngleBrace( val );
	if ( toLang != literal_cpp )replace_all( val , lcolon_colon , ldot );
	for ( auto& i : replacements.at( toLang ) ) ReplaceAllSurroundedByNonAlnumChars( val , i.first , std::string( i.second ) );
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
	if ( val[ 0 ] == ';' && ( applyException ? ( val[ 1 ] != '\n' ) : true ) ) val = val.substr( 1 );
}

void replaceAngleBrace( std::string & val )
{
	boost::algorithm::replace_all( val , "[[" , "<" );
	boost::algorithm::replace_all( val , "]]" , ">" );
}

void checkStr( const std::string_view & val , const std::string & toLang )
{
	using namespace std::string_literals;
	std::list<char> closeBraces;
	size_t i = 0;
	bool isEnabled = true;
	for ( auto iter = val.begin( ) , end = val.end( );
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

std::string WriteDoLoop( std::string & val , std::ostream & o , const std::string & toLang )
{
	using namespace boost::algorithm;

	auto loopCondIter = boost::find( val , first_finder( "While " , isEqualConsiderSpace( ) ) ).begin( );
	bool isWhile = true;
	if ( loopCondIter == val.end( ) )
	{
		loopCondIter = boost::find( val , first_finder( "Until " , isEqualConsiderSpace( ) ) ).begin( );
		if ( loopCondIter == val.end( ) )
			throw throwVal( "Invalid Do loop syntax" , invalidLoopSyntax );
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

void convertTextToLang( std::ostream& o , std::string & text , const std::string & toLang , bool doStartIndent , bool processNewLines )
{
	using namespace boost::algorithm;

	replace_all( text , "|]" , ">" );
	replace_all( text , "[|" , "<" );
	checkStr( text , toLang );

	size_t idx = 0;
	bool start = true;
	bool isStr = false;
	loop_split(
		text ,
		[ ] ( auto&& idx , auto&& iter ) { return isStrQuote( idx , iter ); } ,
		[
			&idx ,
			&toLang ,
			&o ,
			&doStartIndent ,
			&start ,
			&processNewLines ,
			&isStr
		] ( std::string&& val )
		{
			bool isEven = ( idx % 2 ) == 0;
			if ( isEven )
			{
				replaceAngleBrace( val );
				convStr( val , toLang , processNewLines , isStr );
				loop_split(
					val ,
					[ ] ( auto&& idx , auto&& iter ) { return *iter == '\n'; } ,
					[ &o , &doStartIndent , &start , &idx ]
				( std::string&& val )
					{
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
				if ( toLang == literal_cs ) replace_all( val , R"(")" , R"("")" );
				replace_all( val , "\\`" , "`" );
				if ( toLang == literal_cs ) o << "@\"" << val << "\"";
				else if ( toLang == literal_cpp ) o << "R\"(" << val << ")\"";
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
		if ( pos == ( underscore + 1 ) && !isspace( val[ underscore - 1 ] ) )pos = val.find_first_of( '\n' , pos + 1 );
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

	cout << "Full parsing started" << endl;

	try
	{
		++indentLevel;
		if ( is_regular_file( arg ) )
			parse_file( arg , toLang , indent );
		else if ( is_directory( arg ) )
			traversePath( arg , toLang , indent );
		else throw throwVal( "Path is neither a (regular) file not a directory" , notFileNorDir );
		--indentLevel;

		cout << "Full parsing succeeded" << endl;

	}
	catch ( const throwVal& val )
	{

		--indentLevel;
		cout << "Full parsing failed" << endl;

		return val.errCode;
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
				throw throwVal( "Path is neither a (regular) file not a directory" , notFileNorDir );
		}
		cout << ::indent << truncname << " Dir. parsing succeeded" << endl;
		--indentLevel;
	}
	catch ( throwVal& v )
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
		throw throwVal( "Could not open input file: " + filename.string( ) , unableToOpenFileForInput );
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

void __Print_Stack_Trace__PST__For__ERROR_Handling( )
{ std::cerr << boost::stacktrace::stacktrace( ) << std::endl; }

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
			if ( !o )
			{
				throw throwVal( "Could not open output file: " + filename.string( ) , unableToOpenFileForOutput );
			}
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

inline throwVal::throwVal( const std::string& first , int errCode ) :errCode( errCode )
{ std::cerr << "Error: " << first << std::endl; }
