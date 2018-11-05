#include "stdafx.h"
#include "statics.h"

size_t & statics::indentVal( ) { static size_t val; return val; }
const char * statics::literal_cs( ) { static const auto& lit = "CS"; return lit; }
const char * statics::literal_cpp( )
{ static const auto& lit = "CPP17WithBoost"; return lit; }
