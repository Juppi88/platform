/**********************************************************************
 *
 * PROJECT:		Platform library
 * FILE:		PlatformUnix.c
 * LICENCE:		See Licence.txt
 * PURPOSE:		A library which provides a generic API for platform
 *				dependant functionality, such as shared library loading.
 *				POSIX implementation.
 *
 *				(c) Tuomo Jauhiainen 2012
 *
 **********************************************************************/

#ifndef _WIN32

#include "Platform.h"
#include <dlfcn.h>
#include <stddef.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

/**************************************************
	lib_open
**************************************************/
void* lib_open( const char* file )
{
	void* handle;

	handle = dlopen( file, RTLD_NOW );
	return handle;
}

/**************************************************
	lib_symbol
**************************************************/
void* lib_symbol( void* handle, const char* name )
{
	if ( !handle ) return NULL;
	return dlsym( handle, name );
}

/**************************************************
	lib_close
**************************************************/
int lib_close( void* handle )
{
	if ( !handle ) return 0;
	return dlclose( handle ) ? 0 : 1;
}

/**************************************************
	lib_error
**************************************************/
const char* lib_error( void )
{
	return dlerror();
}

/**************************************************
	thread_create
**************************************************/
int thread_create( thread_func_t func, void* arguments )
{
	pthread_t thread;
	pthread_attr_t attr;

	pthread_attr_init( &attr );
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );

	return pthread_create( &thread, &attr, func, arguments );
}

/**************************************************
	thread_sleep
**************************************************/
void thread_sleep( uint32 millisec )
{
	usleep( millisec * 1000 );
}

/**************************************************
	mem_alloc
**************************************************/
void* mem_alloc( uint32 size )
{
	void* ptr;
	ptr = malloc( size );

	if ( !ptr )
	{
		// TODO: Add a message or something nicer here. For now, just kill the app.
		exit( 1 );
	}

	return ptr;
}

/**************************************************
	mem_alloc_clean

	Initializes a memory block safely like mem_alloc
	but also initializes the memory to 0.
**************************************************/
void* mem_alloc_clean( uint32 size )
{
	void* ptr;
	ptr = malloc( size );

	if ( !ptr )
	{
		// TODO: Add a message or something nicer here. For now, just kill the app.
		exit( 1 );
	}

	memset( ptr, 0, size );
	return ptr;
}

/**************************************************
	mem_free
**************************************************/
void mem_free( void* ptr )
{
	if ( !ptr ) return;
	free( ptr );
}

/**************************************************
	get_tick_count
**************************************************/
uint32 get_tick_count( void )
{
	struct timespec now;
	clock_gettime( CLOCK_MONOTONIC, &now );

	return (uint32)( (now.tv_sec * 1000000000LL + now.tv_nsec ) / 1000000LL );
}

#endif /* _WIN32 */
