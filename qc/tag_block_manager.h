#pragma once
#include "macros.h"

enum class tags { DEFINE_TAGS };
std::string_view to_string( const tags& tag, const std::string& lang, bool end = false );

class tag_block_manager
{
public:
};

