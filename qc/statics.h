#pragma once
struct statics
{
	static size_t& indentVal( );
};
static size_t& indentLevel = statics::indentVal( );
