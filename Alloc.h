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
#include "stdtypes.h"

static void*	mem_alloc		( size_t size );
static void*	mem_alloc_clean	( size_t size );
static void		mem_free		( void* ptr );

static MYLLY_INLINE void* mem_alloc( size_t size )
{
	void* ptr = malloc( size );

	assert( ptr != NULL );
	if ( !ptr ) { exit( EXIT_FAILURE ); }

	return ptr;
}

static MYLLY_INLINE void* mem_alloc_clean( size_t size )
{
	void* ptr = malloc( size );

	assert( ptr != NULL );
	if ( !ptr ) { exit( EXIT_FAILURE ); }

	memset( ptr, 0, size );
	return ptr;
}

static MYLLY_INLINE void mem_free( void* ptr )
{
	free( ptr );
}

#ifdef _WIN32

#define mem_stack_alloc( ptr, size ) \
	__try \
		{ ptr = _malloca( size ); } \
	__except ( GetExceptionCode() == STATUS_STACK_OVERFLOW ) \
		{ exit( EXIT_FAILURE ); }

#define mem_stack_free( ptr ) \
	_freea( ptr )

#else

#include <alloca.h>

#define mem_stack_alloc( ptr, size ) \
	ptr = alloca( size )

#define mem_stack_free( ptr )

#endif

#define SAFE_DELETE(x) \
	if ( x ) {         \
		mem_free( x ); \
		x = NULL;      \
	}

#define SAFE_DELETE_CPP(x) \
		if ( x ) {         \
		delete x;          \
		x = NULL;          \
	}

#endif /* __ALLOC_H */
