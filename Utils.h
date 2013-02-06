/**********************************************************************
 *
 * PROJECT:		Platform library
 * FILE:		Utils.h
 * LICENCE:		See Licence.txt
 * PURPOSE:		Various platform independent utility functions.
 *
 *				(c) Tuomo Jauhiainen 2013
 *
 **********************************************************************/

#pragma once
#ifndef __LIB_PLATFORM_UTILS_H
#define __LIB_PLATFORM_UTILS_H

#include "stdtypes.h"

__BEGIN_DECLS

MYLLY_API char*		get_working_directory		( char* buffer, size_t len );
MYLLY_API void		exit_app_with_error			( const char_t* errormsg );

__END_DECLS

#endif /* __LIB_PLATFORM_UTILS_H */
