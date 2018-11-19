#pragma once
#include "macros.h"
#include "languageToConvertTo.h"

enum class tags { DEFINE_TAGS };
std::string_view to_string( const tags& tag , languageToConvertTo lang , bool end = false );
std::string_view to_stringQCL( const tags& tag, bool end = false );
