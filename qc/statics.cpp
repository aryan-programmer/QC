#include "stdafx.h"
#include "statics.h"


size_t & statics::indentVal( )
{
	static size_t val;
	return val;
}
