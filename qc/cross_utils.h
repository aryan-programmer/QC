#pragma once
template<typename>
using has__getcwd = decltype( &::_getcwd );

char* crsgetcwd( char* buf , int size );

std::string workingDir( );

template<typename Str , typename Func>
size_t find_first_of( const Str& text , size_t after , Func&& f )
{
	if ( after >= text.size( ) )return Str::npos;
	auto iter = text.begin( ) + after;
	for ( size_t i = after; i < text.size( ); i++ , iter++ )
	{
		if ( f( i , iter ) )
			return i;
	}
	return Str::npos;
}

template<typename Str , typename Func>
size_t find_first_not_of( const Str& text , size_t after , Func&& f )
{
	if ( after >= text.size( ) )return Str::npos;
	auto iter = text.begin( ) + after;
	for ( size_t i = after; i < text.size( ); i++ , iter++ )
	{
		if ( !f( i , iter ) )
			return i;
	}
	return Str::npos;
}

template<typename Str , typename FuncS , typename FuncF>
void loop_split( const Str& text , FuncS&& sf , FuncF&& ff )
{
	std::size_t start = find_first_not_of( text , 0 , sf ) , end = 0;

	while ( ( end = find_first_of( text , start , sf ) ) != Str::npos )
	{
		ff( Str( text.data( ) + start , end - start ) );
		start = find_first_not_of( text , end , sf );
	}
	if ( start != Str::npos )
		ff( Str( text.data( ) + start , end - start ) );
}

template<typename Str , typename OutputIterator , typename Func>
void split( const Str& text , OutputIterator output , Func&& f )
{
	std::size_t start = find_first_not_of( text , 0 , f ) , end = 0;

	while ( ( end = find_first_of( text , start , f ) ) != Str::npos )
	{
		*output = text.substr( start , end - start );
		output++;
		start = find_first_not_of( text , end , f );
	}
	if ( start != Str::npos )
		*output = text.substr( start );
}

template<typename Str , typename OutputIterator , typename Func , typename FuncD , typename FuncE>
void split_with_enable_disable(
	const Str& text ,
	OutputIterator output ,
	Func&& f ,
	FuncD&& d ,
	FuncE&& e )
{
	std::size_t start = 0 , end = 0;
	bool hasToSwitchStart = true , hasToSwitchEnd = true;

	bool enabled = true;
	auto iter = text.begin( );
	for ( size_t i = 0; i < text.length( ); i++ , iter++ )
	{
		if ( !enabled )
		{
			if ( e( i , iter ) )
				//e(i,iter) is true so we start spliting
				enabled = true;
			continue;
		}
		// If we made it to here then spliting is enabled

		auto isFSatisfied = f( i , iter );
		if ( hasToSwitchStart && ( !isFSatisfied ) )
		{
			// We have to get a new end & the end is i.
			hasToSwitchStart = false;
			start = i;
		}
		else if ( hasToSwitchEnd&&isFSatisfied )
		{
			// We have to get a new end & the end is i.
			hasToSwitchEnd = false;
			end = i;
		}

		if ( !hasToSwitchEnd && !hasToSwitchStart )
		{
			hasToSwitchEnd = hasToSwitchStart = true;
			*output = text.substr( start , end - start );
			output++;
		}

		if ( enabled && d( i , iter ) )
			// d(i,iter) is true so we stop spliting
			enabled = false;
	}
	if ( start != Str::npos )
		*output = text.substr( start );
}
