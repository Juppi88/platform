/**********************************************************************
 *
 * PROJECT:		Platform library
 * FILE:		Window.h
 * LICENCE:		See Licence.txt
 * PURPOSE:		Window system related functions.
 *
 *				(c) Tuomo Jauhiainen 2013
 *
 **********************************************************************/

#pragma once
#ifndef __LIB_PLATFORM_WINDOW_H
#define __LIB_PLATFORM_WINDOW_H

#include "stdtypes.h"

typedef enum
{
	CURSOR_ARROW,
	CURSOR_TEXT,
	CURSOR_CROSSHAIR,
	CURSOR_MOVE,
	CURSOR_FORBIDDEN,
	NUM_CURSORS
} MOUSECURSOR;

typedef void syswindow_t;

__BEGIN_DECLS

MYLLY_API syswindow_t*		create_system_window			( int x, int y, int w, int h, char_t* title, bool border );
MYLLY_API void				destroy_system_window			( syswindow_t* window );
MYLLY_API void				process_window_messages			( syswindow_t* window, bool (*callback)(void*) );
MYLLY_API bool				is_window_visible				( syswindow_t* window );

MYLLY_API void				window_pos_to_screen			( syswindow_t* window, uint16* x, uint16* y );
MYLLY_API void				get_window_pos					( syswindow_t* window, uint16* x, uint16* y );
MYLLY_API void				set_window_pos					( syswindow_t* window, uint16 x, uint16 y );
MYLLY_API void				get_window_size					( syswindow_t* window, uint16* w, uint16* h );
MYLLY_API void				set_window_size					( syswindow_t* window, uint16 x, uint16 y );

MYLLY_API void				copy_to_clipboard				( const char_t* text );
MYLLY_API const char_t*		paste_from_clipboard			( void );

MYLLY_API void				set_mouse_cursor				( MOUSECURSOR cursor );

__END_DECLS

#endif /* __LIB_PLATFORM_WINDOW_H */
