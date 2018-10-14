#pragma once

template<typename T> class singleton;

template<typename T>
struct singleton_ref
{
	using ref = T & ;
	using ptr = T * ;
	ptr operator->( ) { return &singleton<T>::__INST__( ); }
	ref operator*( ) { return singleton<T>::__INST__( ); }
	ref get( ) { return singleton<T>::__INST__( ); }
	ptr get_ptr( ) { return &singleton<T>::__INST__( ); }
	ref operator()( ) { return singleton<T>::__INST__( ); }
	operator ref( ) { return singleton<T>::__INST__( ); }
	operator ptr( ) { return &singleton<T>::__INST__( ); }
};

template<typename T>
class singleton
{
	static T& __INST__( );
protected:
	singleton( ) = default;
	virtual ~singleton( ) = default;
	singleton( const singleton& ) = delete;
	singleton& operator=( const singleton& ) = delete;
	singleton( singleton&& ) = delete;
	singleton& operator=( singleton&& ) = delete;
public:
	static singleton_ref<T> I , &inst , &i;

	template<typename T> friend struct singleton_ref;
};

template<typename T>
inline T & singleton<T>::__INST__( ) { static T val; return val; }

template<typename T>singleton_ref<T> singleton<T>::I;
template<typename T>singleton_ref<T>& singleton<T>::inst = singleton<T>::I;
template<typename T>singleton_ref<T>& singleton<T>::i = singleton<T>::I;
