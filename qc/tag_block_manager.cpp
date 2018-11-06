#include "stdafx.h"
#include "tag_block_manager.h"
#include <mutex>

const char* empty_str( ) { static const char* val = ""; return val; }
const char* closeBrace( ) { static const char* val = "}"; return val; }

std::unordered_map<std::string , std::unordered_map<tags , std::pair<std::string_view , std::string_view>>> constructsMap
{
	{
		literal_cs,
		{
{ tags::_Blk_,{empty_str( ),closeBrace( )}},
{ tags::_QC_,{empty_str( ),empty_str( )}},
{ tags::_Class_,{"class",closeBrace( )} },
{ tags::_Structure_,{"struct",closeBrace( )} },
{ tags::_Namespace_,{"namespace",closeBrace( )} },
{ tags::_Interface_,{"interface",closeBrace( ) }},
{ tags::_Enum_,{"enum",closeBrace( )}},
{ tags::_Native_,{empty_str( ),empty_str( )} },
{ tags::_NativeCS_,{empty_str( ),empty_str( )} },
{ tags::_NativeCPP_,{"/*","*/"} },
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
{ tags::_ForEver_,{ "while( true )",closeBrace( )} },
{ tags::_Comment_,{"/*","*/"} },
{tags::_Function_,{empty_str( ),closeBrace( )}},
{tags::_Lock_,{"lock",closeBrace( ) } },
{tags::_Using_,{"using",closeBrace( ) } },
{tags::_Event_,{empty_str( ),closeBrace( ) } },
{tags::_Add_,{"add",closeBrace( ) } },
{tags::_Remove_,{"remove",closeBrace( ) } },
		}
	},
	{
		literal_cpp,
		{
{ tags::_Blk_,{empty_str( ),closeBrace( )}},
{ tags::_QC_,{empty_str( ),empty_str( )}},
{ tags::_Class_,{"class",closeBrace( )} },
{ tags::_Structure_,{"struct",closeBrace( )} },
{ tags::_Namespace_,{"namespace",closeBrace( )} },
{ tags::_Interface_,{"interface",closeBrace( ) }},
{ tags::_Enum_,{"enum class",closeBrace( )}},
{ tags::_Native_,{empty_str( ),empty_str( )} },
{ tags::_NativeCPP_,{empty_str( ),empty_str( )} },
{ tags::_NativeCS_,{"/*","*/"} },
{ tags::_Unsafe_,{empty_str( ),closeBrace( )} },
{ tags::_Unchecked_,{empty_str( ),closeBrace( )} },
{ tags::_Checked_,{empty_str( ),closeBrace( )} },
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
{ tags::_Do_,{"do",closeBrace( )} } ,
{ tags::_Until_,{"while",closeBrace( )} },
{ tags::_While_,{"while",closeBrace( )} },
{ tags::_For_,{"for",closeBrace( )} },
{ tags::_RevFor_,{"for",closeBrace( )} },
{ tags::_ForEach_,{"for",closeBrace( )} },
{ tags::_ForEver_,{ "while( true )",closeBrace( )} },
{ tags::_Comment_,{"/*","*/"} },
{ tags::_Function_,{empty_str( ),closeBrace( )}},
{ tags::_Lock_,{"{::std::lock_guard<::std::mutex> __mutex_locker__","} }"}},
{ tags::_Using_,{empty_str( ),closeBrace( ) } }
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
