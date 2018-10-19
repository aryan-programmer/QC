#include "stdafx.h"
#include "qc.h"

////////////////////
//  Main program  //
////////////////////
int main( int argc , char **argv )
{
	using namespace std;
	using namespace boost::program_options;
	try
	{
		options_description desc {
			R"(This is the QC converter & interpreter version 0.4.

It can be used to convert .qc files to an another language, at present only conversion to C# is supported.
Though you have to use "CS" as the language parameter not "C#".
It can also be used to run .qc files.
And note QC stands for Quick C.

Options)" };
		desc.add_options( )
			( "help,h" , "Print this message and exit." )
			( "file,f" , value<string>( ) , "The file to be parsed or converted." )
			( "language,l" , value<string>( )->implicit_value( "qc" ) , "The language to be converted to, at present only C# is supported. If language is set to 'qc' then the file will be parsed & interpreted, not converted to the language it already is in!\nNOTE: Use CS as this parameter when converting to C# as the character'#' is problematic when used in a makefile & many other utilities." );
		command_line_parser cl_parser( argc , argv );
		auto parsed_opts = cl_parser.options( desc ).allow_unregistered( ).style(
			command_line_style::default_style |
			command_line_style::allow_slash_for_short
		).run( );
		variables_map vm;
		store( parsed_opts , vm );
		notify( vm );

		if ( vm.count( "help" ) )
		{
			cout << desc << endl;
			return 0;
		}

		cout << setprecision( 100 ) << boolalpha;
		auto retVal = parseArgs( move( vm ) );
		return retVal;
	}
	catch ( const error &ex )
	{
		cerr << ex.what( ) << endl;
	}
}

