/**********************************************************************
 *
 * PROJECT:		Platform library
 * FILE:		Platform.h
 * LICENCE:		See Licence.txt
 * PURPOSE:		A library which provides a generic API for platform
 *				dependant functionality, such as shared library loading.
 *
 *				(c) Tuomo Jauhiainen 2012
 *
 **********************************************************************/

#pragma once
#ifndef __LIB_PLATFORM_H
#define __LIB_PLATFORM_H

#include "stdtypes.h"

#if defined( _WIN32 )
	typedef uint32 ( __stdcall  *thread_func_t )( void* args );
	#define _THREAD_FUNC( func ) unsigned int __stdcall func( void* args )
#else
	typedef void* ( *thread_func_t )( void* args );
	#define _THREAD_FUNC( func ) void* func( void* args )
#endif

typedef struct systimer_s systimer_t;

typedef enum
{
	CURSOR_ARROW,
	CURSOR_TEXT,
	CURSOR_CROSSHAIR,
	CURSOR_MOVE,
	CURSOR_FORBIDDEN,
	NUM_CURSORS
} MOUSECURSOR;

__BEGIN_DECLS

//
// Shared library loading
//
MYLLY_API void*					lib_open						( const char* file );
MYLLY_API void*					lib_symbol						( void* handle, const char* name );
MYLLY_API int					lib_close						( void* handle );
MYLLY_API const char*			lib_error						( void );

//
// Thread related functions
//
MYLLY_API int					thread_create					( thread_func_t func, void* arguments );
MYLLY_API void					thread_sleep					( uint32 millisec );

//
// Safe memory allocation functions
//
MYLLY_API void*					mem_alloc						( uint32 size );
MYLLY_API void*					mem_alloc_clean					( uint32 size );
MYLLY_API void					mem_free						( void* ptr );

//
// Time/timer functions
//
MYLLY_API uint32				get_tick_count					( void );
MYLLY_API systimer_t*			systimer_create					( float interval );
MYLLY_API void					systimer_destroy				( systimer_t* timer );
MYLLY_API float					systimer_wait					( systimer_t* timer, bool wait );

//
// Misc functions
//
MYLLY_API char*					get_working_directory			( char* buffer, size_t len );

//
// Window system
//
MYLLY_API void*					create_system_window			( int x, int y, int w, int h, char_t* title, bool border );
MYLLY_API void					destroy_system_window			( void* window );
MYLLY_API void					process_window_messages			( void* window, bool (*callback)(void*) );
MYLLY_API bool					is_window_visible				( void* window );

MYLLY_API void					window_pos_to_screen			( void* window, uint16* x, uint16* y );
MYLLY_API void					get_window_pos					( void* window, uint16* x, uint16* y );
MYLLY_API void					set_window_pos					( void* window, uint16 x, uint16 y );
MYLLY_API void					get_window_size					( void* window, uint16* w, uint16* h );
MYLLY_API void					set_window_size					( void* window, uint16 x, uint16 y );

MYLLY_API void					copy_to_clipboard				( const char_t* text );
MYLLY_API const char_t*			paste_from_clipboard			( void );

MYLLY_API void					set_mouse_cursor				( MOUSECURSOR cursor );

MYLLY_API void					exit_app_with_error				( const char_t* errormsg );

__END_DECLS


#endif /* __LIB_PLATFORM_H */
