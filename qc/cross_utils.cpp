#include "stdafx.h"
#include "cross_utils.h"

char * crsgetcwd( char * buf , int size )
{
	if constexpr( boost::is_detected_v<has__getcwd , void> )
		return _getcwd( buf , size );
	else return getcwd( buf , size );
}
std::string workingDir( )
{
	char buffer[ 256 ];
	const char* const retBuf = crsgetcwd( buffer , sizeof( buffer ) );
	return retBuf;
}
