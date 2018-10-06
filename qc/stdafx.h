// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

struct qc_grammar;

#define _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <direct.h>
#include <stdlib.h>

#include <boost/filesystem.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/type_traits/is_detected.hpp>
#include <boost/phoenix/statement/throw.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <set>
#include <list>
#include <chrono>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <strstream>
#include <unordered_set>
#include <unordered_map>

#include "qc_parser_runner.h"
#include "qc_grammar.h"
#include "utils.h"

// TODO: reference additional headers your program requires here
