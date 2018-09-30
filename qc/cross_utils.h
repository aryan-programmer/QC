#pragma once
template<typename>
using has__getcwd = decltype( &::_getcwd );

char* crsgetcwd( char* buf , int size );

std::string workingDir();
