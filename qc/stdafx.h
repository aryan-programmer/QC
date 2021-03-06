// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define _SILENCE_FPOS_SEEKPOS_DEPRECATION_WARNING
#define _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING
#define forceinline BOOST_FORCEINLINE
// static CaST
#define cst static_cast
#define _USE_MATH_DEFINES
struct qc_grammar;

#include "targetver.h"

#include <tchar.h>
#include <direct.h>
#include <stdlib.h>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix/statement/throw.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <set>
#include <chrono>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <unordered_map>

#include "statics.h"
#include "languageToConvertTo.h"
#include "qc_grammar.h"
#include "utils.h"
