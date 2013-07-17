/**********************************************************************
 *
 * PROJECT:		Platform library
 * FILE:		Thread.c
 * LICENCE:		See Licence.txt
 * PURPOSE:		Platform independent threading functions.
 *
 *				(c) Tuomo Jauhiainen 2013
 *
 **********************************************************************/

#include "Platform/Thread.h"

#ifdef _WIN32

//////////////////////////////////////////////////////////////////////////
// Win32 implementation
//////////////////////////////////////////////////////////////////////////

#include <process.h>

int thread_create( thread_func_t func, void* args )
{
	HANDLE thread;
	uint32 thread_addr = 0;

	thread = (HANDLE)_beginthreadex( NULL, 0, func, args, 0, &thread_addr );

	if ( !thread ) return 1;

	CloseHandle( thread );
	return 0;
}

void thread_sleep( uint32 msec )
{
	Sleep( msec );
}

#else

//////////////////////////////////////////////////////////////////////////
// POSIX implementation
//////////////////////////////////////////////////////////////////////////

#include <pthread.h>
#include <unistd.h>

int thread_create( thread_func_t func, void* arguments )
{
	pthread_t thread;
	pthread_attr_t attr;

	pthread_attr_init( &attr );
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );

	return pthread_create( &thread, &attr, func, arguments );
}

void thread_sleep( uint32 millisec )
{
	usleep( millisec * 1000 );
}

#endif
