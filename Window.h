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

#ifndef MYLLY_PLATFORM_MINIMAL

#include "stdtypes.h"

typedef enum {
	CURSOR_ARROW,
	CURSOR_TEXT,
	CURSOR_CROSSHAIR,
	CURSOR_MOVE,
	CURSOR_FORBIDDEN,
	NUM_CURSORS
} MOUSECURSOR;

typedef void ( *clip_paste_cb )( const char* pasted, void* data );
typedef bool ( *wnd_message_cb )( void* packet );

#ifdef _WIN32

typedef void syswindow_t;

#else

#include <X11/Xlib.h>

typedef struct syswindow_t {
	Display* display;
	Window window;
	Window root;
	wnd_message_cb cb;
} syswindow_t;

#endif

__BEGIN_DECLS

MYLLY_API syswindow_t*		create_system_window			( int32 x, int32 y, uint32 w, uint32 h, const char_t* title, bool decoration, wnd_message_cb cb );
MYLLY_API void				destroy_system_window			( syswindow_t* window );

MYLLY_API void				process_window_messages			( syswindow_t* window, wnd_message_cb cb );
MYLLY_API bool				is_window_visible				( syswindow_t* window );

MYLLY_API void				window_pos_to_screen			( syswindow_t* window, int16* x, int16* y );
MYLLY_API void				get_window_pos					( syswindow_t* window, int16* x, int16* y );
MYLLY_API void				set_window_pos					( syswindow_t* window, int16 x, int16 y );
MYLLY_API void				get_window_size					( syswindow_t* window, uint16* w, uint16* h );
MYLLY_API void				set_window_size					( syswindow_t* window, uint16 x, uint16 y );

MYLLY_API void				redraw_window					( syswindow_t* window );

MYLLY_API void				clipboard_copy					( syswindow_t* window, const char_t* text );
MYLLY_API void				clipboard_paste					( syswindow_t* window, clip_paste_cb cb, void* data );

#ifndef _WIN32
MYLLY_API void				clipboard_handle_event			( syswindow_t* window, void* packet );
#endif

MYLLY_API void				set_mouse_cursor				( syswindow_t* window, MOUSECURSOR cursor );

__END_DECLS

#endif /* MYLLY_PLATFORM_MINIMAL */
#endif /* __LIB_PLATFORM_WINDOW_H */
