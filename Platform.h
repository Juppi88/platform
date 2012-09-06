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

#include "Defines.h"

#if defined( _WIN32 )
	typedef uint32 ( __stdcall  *thread_func_t )( void* args );
	#define _THREAD_FUNC( func ) unsigned int __stdcall func( void* args )
#else
	typedef void* ( *thread_func_t )( void* args );
	#define _THREAD_FUNC( func ) void* func( void* pArgs )
#endif

typedef struct timer_s* timer_t;

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
// Timer functions
//
MYLLY_API uint32				timer_get_ticks					( void );
MYLLY_API timer_t				timer_create					( float interval );
MYLLY_API void					timer_destroy					( timer_t timer );
MYLLY_API float					timer_wait						( timer_t timer, bool wait );

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

__END_DECLS


#endif /* __LIB_PLATFORM_H */
