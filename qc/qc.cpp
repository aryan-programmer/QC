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
			R"(This is the QC converter & interpreter version 0.9.
	
It can be used to convert .qc files to an another language.
NOTE: For conversion to C# you have to use "CS" as the language parameter not "C#".
NOTE: For conversion to C++ you have to use "CPP17WithBoost" to as the language parameter to acknowledge that C++17 & Boost will be required. (Boost.HOF(HigherOrderFunctions) is required.)
It can also be used to run .qc files.
And note QC stands for Quick C.)" };
		options_description optDesc( "Options" );
		optDesc.add_options( )
			( "help,h" , "Print this message and exit." )
			( "file,f" , value<string>( ) , "The file to be parsed or converted." )
			( "language,l" , value<string>( )->implicit_value( "qc" ) , "The language to be converted to. If language is set to 'qc' then the file will be parsed & interpreted, not converted to the language it already is in!" );
		desc.add( optDesc );
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
		try
		{
			auto retVal = parseArgs( move( vm ) );
			if ( retVal ) cerr << "Error code: " << retVal << endl;
			return retVal;
		}
		catch ( const std::exception& ex )
		{
			cerr << "Error: " << ex.what( ) << endl;
			cerr << "Error code: " << unknownError << endl;
			return unknownError;
		}
	}
	catch ( const error &ex )
	{
		cerr << ex.what( ) << endl;
		cerr << "Error code: " << invalidCommandLineParameters << endl;
		return invalidCommandLineParameters;
	}
}

