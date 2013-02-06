/**********************************************************************
 *
 * PROJECT:		Platform library
 * FILE:		Thread.h
 * LICENCE:		See Licence.txt
 * PURPOSE:		Platform independent threading functions.
 *
 *				(c) Tuomo Jauhiainen 2013
 *
 **********************************************************************/

#pragma once
#ifndef __LIB_PLATFORM_THREAD_H
#define __LIB_PLATFORM_THREAD_H

#include "stdtypes.h"

#ifdef _WIN32
	typedef uint32 ( __stdcall  *thread_func_t )( void* args );
	#define _THREAD_FUNC( func ) unsigned int __stdcall func( void* args )
#else
	typedef void* ( *thread_func_t )( void* args );
	#define _THREAD_FUNC( func ) void* func( void* args )
#endif

__BEGIN_DECLS

MYLLY_API int		thread_create			( thread_func_t func, void* args );
MYLLY_API void		thread_sleep			( uint32 msec );

__END_DECLS

#endif /* __LIB_PLATFORM_THREAD_H */
