#include "stdafx.h"

languageToConvertTo fromString( std::string_view s )
{
	if ( s == literal_cpp )return languageToConvertTo::CPP;
	else if ( s == literal_cs ) return languageToConvertTo::CS;
	else throw qcParsingException( "Language \"" + std::string( s ) + "\" is not presently supported" , unsupportedLanguage );
}
