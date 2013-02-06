/**********************************************************************
 *
 * PROJECT:		Platform library
 * FILE:		Alloc.h
 * LICENCE:		See Licence.txt
 * PURPOSE:		Safer memory allocation functions.
 *
 *				(c) Tuomo Jauhiainen 2013
 *
 **********************************************************************/

#pragma once
#ifndef __ALLOC_H
#define __ALLOC_H

#include <malloc.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include "alloca.h"
#endif

static __inline void* mem_alloc( size_t size )
{
	void* ptr = malloc( size );

	assert( ptr != NULL );
	if ( !ptr ) { exit( EXIT_FAILURE ); }

	return ptr;
}

static __inline void* mem_alloc_clean( size_t size )
{
	void* ptr = malloc( size );

	assert( ptr != NULL );
	if ( !ptr ) { exit( EXIT_FAILURE ); }

	memset( ptr, 0, size );
	return ptr;
}

static __inline void mem_free( void* ptr )
{
	free( ptr );
}

static __inline void* mem_stack_alloc( size_t size )
{
#ifdef _WIN32
	__try
	{
		return _malloca( size );
	}
	__except ( GetExceptionCode() == STATUS_STACK_OVERFLOW )
	{
		exit( EXIT_FAILURE );
	}
#else
	return alloca( size );
#endif
}

static __inline void mem_stack_free( void* ptr )
{
#ifdef _WIN32
	_freea( ptr );
#else
	return;
#endif
}

#endif /* __ALLOC_H */
