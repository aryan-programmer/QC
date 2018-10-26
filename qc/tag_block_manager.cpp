#include "stdafx.h"
#include "tag_block_manager.h"

const char* empty_str( )
{
	static const char* val = "";
	return val;
}
const char* closeBrace( )
{
	static const char* val = "}";
	return val;
}

std::unordered_map<std::string , std::unordered_map<tags , std::pair<std::string_view , std::string_view>>> constructsMap
{
	{
		"CS",
		{
{ tags::_QC_,{empty_str( ),empty_str( )}},
{ tags::_Class_,{"class",closeBrace( )} },
{ tags::_Structure_,{"struct",closeBrace( )} },
{ tags::_Namespace_,{"namespace",closeBrace( )} },
{ tags::_Interface_,{"interface",closeBrace( ) }},
{ tags::_Enum_,{"enum",closeBrace( )}},
{ tags::_Native_,{empty_str( ),empty_str( )} },
{ tags::_Property_,{empty_str( ),closeBrace( )} },
{ tags::_Indexer_,{empty_str( ),closeBrace( )} },
{ tags::_Unsafe_,{"unsafe",closeBrace( )} },
{ tags::_Unchecked_,{"unchecked",closeBrace( )} },
{ tags::_Checked_,{"checked",closeBrace( )} },
{ tags::_Get_,{"get",closeBrace( )} } ,
{ tags::_Set_,{"set",closeBrace( )} },
{ tags::_If_,{"if",closeBrace( )} } ,
{ tags::_ElseIf_,{"else if",closeBrace( )} },
{ tags::_Else_,{"else",closeBrace( )} },
{ tags::_Switch_,{"switch",closeBrace( )} },
{ tags::_Case_,{"case ","break;"} },
{ tags::_FTCase_,{"case ",empty_str( )} },
{ tags::_Default_,{"default: ","break;"} },
{ tags::_FTDefault_,{"default: ",empty_str( )} },
{ tags::_Try_,{"try",closeBrace( )} },
{ tags::_Catch_,{"catch",closeBrace( )} },
{ tags::_Finally_,{"finally",closeBrace( )} },
{ tags::_Do_,{"do",closeBrace( )} } ,
{ tags::_Until_,{"while",closeBrace( )} },
{ tags::_While_,{"while",closeBrace( )} },
{ tags::_For_,{"for",closeBrace( )} },
{ tags::_RevFor_,{"for",closeBrace( )} },
{ tags::_ForEach_,{"foreach",closeBrace( )} },
{ tags::_ForEver_,{ "for(;;)",closeBrace( )} },
{ tags::_Comment_,{"/*","*/"} },
{tags::_Function_,{empty_str( ),closeBrace( )}}
		}
	},
{
	"QC",
{DEFINE_QC_CONSTRUCTS }
 }
};

std::string_view to_string( const tags & tag , const std::string & lang , bool end )
{
	switch ( tag )
	{
		DEFINE_OSTR_SWITCH_CASES
	default:
		return "Unknown";
		break;
	}
}