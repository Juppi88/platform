/**********************************************************************
 *
 * PROJECT:		Platform library
 * FILE:		Timer.c
 * LICENCE:		See Licence.txt
 * PURPOSE:		Platform independent timer and time related functions.
 *
 *				(c) Tuomo Jauhiainen 2013
 *
 **********************************************************************/

#include "Platform/Timer.h"

#ifdef _WIN32

//////////////////////////////////////////////////////////////////////////
// Win32 implementation
//////////////////////////////////////////////////////////////////////////

#include "Platform/Alloc.h"

static float _timerfreq = 0.0f;

struct systimer_s
{
	HANDLE			handle;
	LARGE_INTEGER	interval;
	LARGE_INTEGER	prev_tick;
};

uint32 get_tick_count( void )
{
	return (uint32)GetTickCount64();
}

systimer_t* systimer_create( float interval )
{
	struct systimer_s* timer;
	LARGE_INTEGER frequency;

	if ( _timerfreq == 0 )
	{
		// Cache the frequency first
		QueryPerformanceFrequency( &frequency );
		_timerfreq = (float)frequency.QuadPart;
	}

	timer = (systimer_t*)mem_alloc( sizeof(*timer) );

	timer->handle				= CreateWaitableTimer( NULL, TRUE, NULL );
	timer->interval.QuadPart	= (LONGLONG)( (double)interval * -10000000.0 );

	QueryPerformanceCounter( &timer->prev_tick );
	SetWaitableTimer( timer->handle, &timer->interval, 0, NULL, NULL, 0 );

	return (systimer_t*)timer;
}

void systimer_destroy( systimer_t* timer )
{
	struct systimer_s* p;
	
	if ( !timer ) return;

	p = (struct systimer_s*)timer;

	CloseHandle( p->handle );
	mem_free( timer );
}

float systimer_wait( systimer_t* timer, bool wait )
{
	LARGE_INTEGER tick;
	float delta;
	struct systimer_s* p;

	if ( !timer ) return 0.0f;

	p = (struct systimer_s*)timer;

	if ( wait )
	{
		WaitForSingleObject( p->handle, INFINITE );
		SetWaitableTimer( p->handle, &p->interval, 0, NULL, NULL, 0 );
	}

	QueryPerformanceCounter( &tick );

	delta = (float)( tick.QuadPart - p->prev_tick.QuadPart ) / _timerfreq;

	p->prev_tick = tick;

	return delta;
}

#else

//////////////////////////////////////////////////////////////////////////
// POSIX implementation
//////////////////////////////////////////////////////////////////////////

#include <time.h>

uint32 get_tick_count( void )
{
	struct timespec now;
	clock_gettime( CLOCK_MONOTONIC, &now );

	return (uint32)( (now.tv_sec * 1000000000LL + now.tv_nsec ) / 1000000LL );
}

#endif
