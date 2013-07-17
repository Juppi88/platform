/**********************************************************************
 *
 * PROJECT:		Platform library
 * FILE:		Library.c
 * LICENCE:		See Licence.txt
 * PURPOSE:		Platform independent shared library loading.
 *
 *				(c) Tuomo Jauhiainen 2013
 *
 **********************************************************************/

#include "Platform/Library.h"

#ifdef _WIN32

//////////////////////////////////////////////////////////////////////////
// Win32 implementation
//////////////////////////////////////////////////////////////////////////

void* lib_open( const char* file )
{
	void* handle;

	handle = (void*)GetModuleHandleA( file );

	if ( !handle )
	{
		handle = (void*)LoadLibraryA( file );
	}

	return handle;
}

void* lib_symbol( void* handle, const char* name )
{
	if ( !handle ) return NULL;

	return (void*)GetProcAddress( (HMODULE)handle, name );
}

int lib_close( void* handle )
{
	if ( !handle ) return 0;

	return FreeLibrary( (HMODULE)handle );
}

const char* lib_error( void )
{
	return "";
}

#else

//////////////////////////////////////////////////////////////////////////
// POSIX implementation
//////////////////////////////////////////////////////////////////////////

#include <dlfcn.h>

void* lib_open( const char* file )
{
	void* handle;

	handle = dlopen( file, RTLD_NOW );
	return handle;
}

void* lib_symbol( void* handle, const char* name )
{
	if ( !handle ) return NULL;
	return dlsym( handle, name );
}

int lib_close( void* handle )
{
	if ( !handle ) return 0;
	return dlclose( handle ) ? 0 : 1;
}

const char* lib_error( void )
{
	return dlerror();
}

#endif
