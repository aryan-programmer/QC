#pragma once
template<typename>
using has__getcwd = decltype( &::_getcwd );
char* crsgetcwd( char* buf , int size );
std::string workingDir( );

template<typename str_t , typename CkeckingFunc>
size_t find_first_of( const str_t& text , size_t after , CkeckingFunc&& checkingFunc )
{
	if ( after >= text.size( ) )return str_t::npos;
	auto iter = text.begin( ) + after;
	for ( size_t i = after; i < text.size( ); i++ , iter++ )
		if ( checkingFunc( i , iter ) ) return i;
	return str_t::npos;
}

template<typename str_t , typename CkeckingFunc>
size_t find_first_not_of( const str_t& text , size_t after , CkeckingFunc&& checkingFunc )
{
	if ( after >= text.size( ) )return str_t::npos;
	auto iter = text.begin( ) + after;
	for ( size_t i = after; i < text.size( ); i++ , iter++ )
		if ( !checkingFunc( i , iter ) ) return i;
	return str_t::npos;
}

template<typename str_t , typename CkeckingFunc>
size_t find_last_of( const str_t& text , size_t after , CkeckingFunc&& checkingFunc )
{
	if ( after >= text.size( ) )return str_t::npos;
	auto iter = text.begin( ) + after;
	for ( size_t i = text.size( ) - 1; i >= 0; i-- )
		if ( checkingFunc( i , iter ) ) return i;
	return str_t::npos;
}

template<typename str_t , typename CkeckingFunc>
size_t find_last_not_of( const str_t& text , size_t after , CkeckingFunc&& checkingFunc )
{
	if ( after >= text.size( ) )return str_t::npos;
	auto iter = text.begin( ) + after;
	for ( size_t i = text.size( ) - 1; i >= 0; i-- )
		if ( !checkingFunc( i , iter ) ) return i;
	return str_t::npos;
}

template<typename str_t , typename CheckingFunc , typename AtSplitFunc>
void loop_split( const str_t& text , CheckingFunc&& checkingFunc , AtSplitFunc&& atSplitFunc )
{
	std::size_t start = find_first_not_of( text , 0 , checkingFunc ) , end = 0;

	while ( ( end = find_first_of( text , start , checkingFunc ) ) != str_t::npos )
	{
		atSplitFunc( str_t( text.data( ) + start , end - start ) );
		start = find_first_not_of( text , end , checkingFunc );
	}
	if ( start != str_t::npos )
		atSplitFunc( str_t( text.data( ) + start , text.size( ) - start ) );
}

template<typename str_t , typename OutIter , typename CkeckingFunc>
inline void split( const str_t& text , OutIter output , CkeckingFunc&& checkingFunc )
{ loop_split( text , std::forward<CkeckingFunc>( checkingFunc ) , [ &output ] ( str_t&& val ) { *output = val; output++; } ); }

template<typename str_t , typename CkeckingFunc , typename DisableFunc , typename EnableFunc , typename AtSplitFunc>
void loop_split_with_enable_disable(
	const str_t& text , CkeckingFunc&& checkingFunc , DisableFunc&& disableFunc , EnableFunc&& enableFunc , AtSplitFunc&& asf )
{
	std::size_t start = 0 , end = 0;
	bool hasToSwitchStart = true , hasToSwitchEnd = true;

	bool enabled = true;
	auto iter = text.begin( );
	for ( size_t i = 0; i < text.length( ); i++ , iter++ )
	{
		if ( !enabled )
		{
			if ( enableFunc( i , iter ) )
				//enableFunc(i,iter) is true so we start spliting
				enabled = true;
			continue;
		}
		// If we made it to here then spliting is enabled

		auto isFSatisfied = checkingFunc( i , iter );
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
			asf( str_t( text.data( ) + start , end - start ) );
		}

		if ( enabled && disableFunc( i , iter ) )
			// disableFunc(i,iter) is true so we stop spliting
			enabled = false;
	}
	if ( start != str_t::npos )
		asf( str_t( text.data( ) + start , text.size( ) - start ) );
}

template<typename str_t , typename OutIter , typename CkeckingFunc , typename DisableFunc , typename EnableFunc>
inline void split_with_enable_disable(
	const str_t& text , OutIter output , CkeckingFunc&& checkingFunc , DisableFunc&& d , EnableFunc&& e )
{
	loop_split_with_enable_disable(
		text ,
		std::forward<CkeckingFunc>( checkingFunc ) , std::forward<DisableFunc>( d ) , std::forward<EnableFunc>( e ) ,
		[ &output ] ( str_t&& val ) { *output = val; output++; } );
}
