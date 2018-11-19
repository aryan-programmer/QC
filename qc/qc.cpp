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
		// The main description
		options_description desc {
			R"(This is the QC trans-compiler version 0.9.1.

It is used to trans-compile .qc files to other languages.
NOTE: For trans-compilation to C# you have to use "CS" as the language parameter not "C#".
NOTE: For trans-compilation to C++ you have to use "CPP17WithBoost" to as the language parameter to acknowledge that C++17 & Boost will be required. (Boost.HOF(HigherOrderFunctions) is one of the libraries in Boost that is required.))" };
		// The options description
		options_description optDesc( "Options" );
		// We add the options
		optDesc.add_options( )
			// The help option -h||--help
			( "help,h" , "Print this message and exit." )
			// The file -f||--file filename
			( "file,f" , value<string>( ) , "The file to be trans-compiled." )
			// The language -l||--language lang
			( "language,l" , value<string>( ) , "The language to be trans-compiled to." );
		// Add the options the main description
		desc.add( optDesc );
		// Create a cmd parser
		command_line_parser cl_parser( argc , argv );
		auto parsed_opts = 
			// To the cmd parser
			cl_parser.
			// We add the options
			options( desc ).
			// We set the style
			style(
				// To the default style
			command_line_style::default_style |
				// And set -h to equal /h
			command_line_style::allow_slash_for_short).
			// And run the parser
			run( );
		// Make a variables map.
		variables_map vm;
		// Store the options in the variable map
		store( parsed_opts , vm );
		// And notify any event handlers (none at present)
		notify( vm );

		// If the user specified -h||--help
		if ( vm.count( "help" ) )
		{
			// We print the main description
			cout << desc << endl;
			// And exit
			return 0;
		}

		// and parse the arguments
		auto retVal = parseArgs( move( vm ) );
		// If the return value is not 0, i.e. there was an error 
		if ( retVal != 0 )
			// We print the error code
			cerr << "Error code: " << retVal << endl;
		// And return the error code in any case
		return retVal;
	}
	// ex is a boost::program_options::error
	catch ( const error &ex )
	{
		// We print what went wrong
		cerr << "Error: " << ex.what( ) << endl;
		// And the fixed error code saying that there were invalid parameters
		cerr << "Error code: " << invalidCommandLineParameters << endl;
		// And return that
		return invalidCommandLineParameters;
	}
	// In case of any other error
	catch ( const std::exception& ex )
	{
		// We print out that error
		cerr << "Error: " << ex.what( ) << endl;
		// And the fixed error code of unknown error
		cerr << "Error code: " << unknownError << endl;
		// And return that.
		return unknownError;
	}
}

