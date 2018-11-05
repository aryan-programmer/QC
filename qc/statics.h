#pragma once
struct statics
{
	static size_t& indentVal( );
	static const char* literal_cs( );
	static const char* literal_cpp( );
};
static size_t& indentLevel = statics::indentVal( );
static auto literal_cs = statics::literal_cs( );
static auto literal_cpp = statics::literal_cpp( );
