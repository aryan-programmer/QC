#include "stdafx.h"
#include "qc.h"

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int main( int argc , char **argv )
{
	using namespace std;

	auto start = std::chrono::steady_clock::now( );
	parseArgs( { "code.qc" } );
	auto end = std::chrono::steady_clock::now( );
	auto diff = end - start;
	cout << endl << "Time: " << diff.count( ) << endl;
	return 0;
	//return parseArgs( std::vector<std::string>( argv+1 , argv+argc ) );
}

