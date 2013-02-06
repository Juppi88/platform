/**********************************************************************
 *
 * PROJECT:		Platform library
 * FILE:		Library.h
 * LICENCE:		See Licence.txt
 * PURPOSE:		Platform independent shared library loading.
 *
 *				(c) Tuomo Jauhiainen 2013
 *
 **********************************************************************/

#pragma once
#ifndef __LIB_PLATFORM_LIBRARY_H
#define __LIB_PLATFORM_LIBRARY_H

#include "stdtypes.h"

__BEGIN_DECLS

MYLLY_API void*			lib_open			( const char* file );
MYLLY_API void*			lib_symbol			( void* handle, const char* name );
MYLLY_API int			lib_close			( void* handle );
MYLLY_API const char*	lib_error			( void );

__END_DECLS

#endif /* __LIB_PLATFORM_LIBRARY_H */
