/**********************************************************************
 *
 * PROJECT:		Platform library
 * FILE:		Window.c
 * LICENCE:		See Licence.txt
 * PURPOSE:		Window system related functions.
 *
 *				(c) Tuomo Jauhiainen 2013
 *
 **********************************************************************/

#include "Platform/Window.h"
#include "Platform/Alloc.h"

#ifdef _WIN32

//////////////////////////////////////////////////////////////////////////
// Win32 implementation
//////////////////////////////////////////////////////////////////////////

#include "Stringy/Stringy.h"

syswindow_t* create_system_window( int32 x, int32 y, uint32 w, uint32 h, const char_t* title, bool border )
{
	WNDCLASS wc;
	HWND window;

	ZeroMemory( &wc, sizeof(wc) );

	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DROPSHADOW;
	wc.lpfnWndProc		= DefWindowProc;
	wc.hInstance		= GetModuleHandle(NULL);
	wc.lpszClassName	= _MTEXT("mylly_window");
	wc.hCursor			= NULL;

	RegisterClass( &wc );

	if ( border )
	{
		window = CreateWindowEx( (WS_EX_WINDOWEDGE|WS_EX_APPWINDOW), wc.lpszClassName, title,
			(WS_VISIBLE|WS_OVERLAPPEDWINDOW|WS_CLIPSIBLINGS|WS_CLIPCHILDREN) & ~(WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME),
			x, y, (int32)w, (int32)h, NULL, NULL, GetModuleHandle(NULL), NULL );
	}
	else
	{
		window = CreateWindowEx( WS_EX_APPWINDOW, wc.lpszClassName, title,
			WS_POPUP, x, y, (int32)w, (int32)h, NULL, NULL, GetModuleHandle(NULL), NULL );

		SetWindowLong( window, GWL_STYLE, 0 );
	}

	assert( window != NULL );

	ShowWindow( window, SW_SHOW );
	SetForegroundWindow( window );
	SetFocus( window );

	return (syswindow_t*)window;
}

void destroy_system_window( syswindow_t* window )
{
	DestroyWindow( (HWND)window );
}

void process_window_messages( syswindow_t* window, bool (*callback)(void*) )
{
	MSG msg;

	while ( PeekMessage( &msg, (HWND)window, 0, 0, PM_REMOVE ) )
	{
		if ( callback )
		{
			if ( !callback( (void*)&msg ) ) continue;
		}

		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
}

bool is_window_visible( syswindow_t* window )
{
	return IsWindowVisible( (HWND)window ) ? true : false;
}

void window_pos_to_screen( syswindow_t* window, int16* x, int16* y )
{
	POINT p;
	p.x = *x, p.y = *y;

	ClientToScreen( (HWND)window, &p );

	*x = (int16)p.x;
	*y = (int16)p.y;
}

void get_window_pos( syswindow_t* window, int16* x, int16* y )
{
	RECT rect;

	GetWindowRect( (HWND)window, &rect );

	*x = (int16)rect.left;
	*y = (int16)rect.top;
}

void set_window_pos( syswindow_t* window, int16 x, int16 y )
{
	SetWindowPos( (HWND)window, HWND_TOP, x, y, 0, 0, SWP_NOSIZE );
}

void get_window_size( syswindow_t* window, uint16* w, uint16* h )
{
	RECT rect;

	GetWindowRect( (HWND)window, &rect );

	*w = (uint16)( rect.right - rect.left );
	*h = (uint16)( rect.bottom - rect.top );
}

void set_window_size( syswindow_t* window, uint16 w, uint16 h )
{
	SetWindowPos( (HWND)window, HWND_TOP, 0, 0, w, h, SWP_NOMOVE );
}

void redraw_window( syswindow_t* window )
{
	RedrawWindow( (HWND)window, NULL, NULL, RDW_INTERNALPAINT );
}

#ifdef MYLLY_UNICODE
UINT data_mode = CF_UNICODETEXT;
#else
UINT data_mode = CF_TEXT;
#endif

void copy_to_clipboard( const char_t* text )
{
	size_t size;
	HGLOBAL mem;
	char_t* data;

	if ( !OpenClipboard( NULL ) ) return;

	size = mstrsize( text );

	mem = GlobalAlloc( GMEM_DDESHARE, size );
	if ( !mem ) return;

	data = (char_t*)GlobalLock( mem );

	mstrcpy( data, text, size );

	GlobalUnlock( mem );

	EmptyClipboard();
	SetClipboardData( data_mode, mem );
	CloseClipboard();
}

const char_t* paste_from_clipboard( void )
{
	HANDLE mem;
	size_t size;
	const char_t* data;
	static char_t* text = NULL;

	if ( !OpenClipboard( NULL ) ) return NULL;
	if ( !IsClipboardFormatAvailable( data_mode ) ) return NULL;

	mem = GetClipboardData( data_mode );

	if ( text ) mem_free( text ); // Free previously pasted text
	data = (const char_t*)GlobalLock( mem );

	size = mstrsize( data );
	text = (char_t*)mem_alloc( size );

	mstrcpy( text, data, size );

	GlobalUnlock( mem );

	CloseClipboard();

	return text;
}

void set_mouse_cursor( MOUSECURSOR cursor )
{
	static HCURSOR cursors[NUM_CURSORS] = { NULL };

	if ( cursor >= NUM_CURSORS ) return;

	if ( !cursors[cursor] )
	{
		// The cursor needs to be loaded first
		switch ( cursor )
		{
		case CURSOR_ARROW:
			cursors[cursor] = LoadCursor( NULL, IDC_ARROW );
			break;

		case CURSOR_TEXT:
			cursors[cursor] = LoadCursor( NULL, IDC_IBEAM );
			break;

		case CURSOR_CROSSHAIR:
			cursors[cursor] = LoadCursor( NULL, IDC_CROSS );
			break;

		case CURSOR_MOVE:
			cursors[cursor] = LoadCursor( NULL, IDC_SIZEALL );
			break;

		case CURSOR_FORBIDDEN:
			cursors[cursor] = LoadCursor( NULL, IDC_NO );
			break;
		}
	}

	SetCursor( cursors[cursor] );
}

#else

//////////////////////////////////////////////////////////////////////////
// X11 implementation
//////////////////////////////////////////////////////////////////////////

#include <X11/Xatom.h>

Display* display = NULL;
uint32 refcount = 0;

struct MWMHints
{
    uint32 flags;
    uint32 functions;
    uint32 decorations;
    int32 input_mode;
    uint32 status;
};

enum
{
    MWM_HINTS_FUNCTIONS		= 1 << 0,
    MWM_HINTS_DECORATIONS	= 1 << 1,
};

enum
{
    MWM_FUNC_ALL		= 1 << 0,
    MWM_FUNC_RESIZE		= 1 << 1,
    MWM_FUNC_MOVE		= 1 << 2,
    MWM_FUNC_MINIMIZE	= 1 << 3,
    MWM_FUNC_MAXIMIZE	= 1 << 4,
    MWM_FUNC_CLOSE		= 1 << 5,
};

syswindow_t* create_system_window( int32 x, int32 y, uint32 w, uint32 h, const char_t* title, bool border )
{
	Window wnd;
	int screen;
	syswindow_t* window;
	struct MWMHints hints;
	Atom prop;

	if ( display == NULL ) display = XOpenDisplay( NULL );
	if ( display == NULL ) return NULL;

	refcount++; // Display reference count

	screen = DefaultScreen( display );
	wnd = XCreateSimpleWindow( display, RootWindow( display, screen ), x, y, w, h, border ? 1 : 0,
							   BlackPixel( display, screen ), WhitePixel( display, screen ) );

	XSelectInput( display, wnd, ExposureMask|KeyPressMask|KeyReleaseMask|PointerMotionMask|ButtonPressMask|ButtonReleaseMask );
	XMapWindow( display, wnd );
	XStoreName( display, wnd, title );

	window = mem_alloc( sizeof(*window) );
	window->display = display;
	window->wnd = wnd;
	window->root = RootWindow( display, DefaultScreen( display ) );

	if ( !border )
	{
		memset( &hints, 0, sizeof(hints) );
		hints.flags = MWM_HINTS_DECORATIONS;
		hints.decorations = 0;

		prop = XInternAtom( display, "_MOTIF_WM_HINTS", False );

		XChangeProperty( display, wnd, prop, prop, 32, PropModeReplace, (uint8*)&hints, 5 );
	}

	return window;
}

void destroy_system_window( syswindow_t* window )
{
	if ( window == NULL ) return;

	XDestroyWindow( window->display, window->wnd );

	if ( --refcount == 0 )
		XCloseDisplay( window->display );

	mem_free( window );
}

void process_window_messages( syswindow_t* window, bool (*callback)(void*) )
{
	XEvent event;

	if ( window == NULL ) return;

	while ( XPending( window->display ) )
	{
		XNextEvent( window->display, &event );

		if ( callback )
			callback( &event );
	}
}

bool is_window_visible( syswindow_t* window )
{
	// Well, this seems fucked up.
	// TODO: Fix it.
	return true;

	/*Atom actual_type, state;
	int actual_format;
	unsigned long i, num_items, bytes_after;
	Atom* atoms;

	if ( window == NULL ) return false;

	atoms = NULL;
	state = XInternAtom( window->display, "_NET_WM_STATE", True );

	XGetWindowProperty( window->display, window->wnd, state, 0, 1024, False, XA_ATOM,
						&actual_type, &actual_format, &num_items, &bytes_after, (unsigned char**)&atoms );

	for ( i = 0; i < num_items; ++i )
	{
		if ( atoms[i] == state )
		{
			XFree( atoms );
			return true;
		}
	}

	XFree( atoms );
	return false;*/
}

void window_pos_to_screen( syswindow_t* window, int16* x, int16* y )
{
	int x_out, y_out;
	Window child;

	if ( window == NULL )
	{
		*x = 0;
		*y = 0;
		return;
	}

	XTranslateCoordinates( window->display, window->wnd, window->root,
						   (int32)*x, (int32)*y, &x_out, &y_out, &child );

	*x = (int16)x_out;
	*y = (int16)y_out;
}

void get_window_pos( syswindow_t* window, int16* x, int16* y )
{
	int x_out, y_out;
	Window child;


	if ( window == NULL )
	{
		*x = 0;
		*y = 0;
		return;
	}

	XTranslateCoordinates( window->display, window->wnd, window->root,
						   0, 0, &x_out, &y_out, &child );

	*x = (int16)x_out;
	*y = (int16)y_out;
}

void set_window_pos( syswindow_t* window, int16 x, int16 y )
{
	XWindowChanges xwc;

	if ( window == NULL ) return;

	xwc.x = x;
	xwc.y = y;

	XConfigureWindow( window->display, window->wnd, CWX|CWY, &xwc );
}

void get_window_size( syswindow_t* window, uint16* w, uint16* h )
{
	XWindowAttributes xwa;

	if ( window == NULL )
	{
		*w = 0;
		*h = 0;
		return;
	}

	XGetWindowAttributes( window->display, window->wnd, &xwa );

	*w = (uint16)xwa.width;
	*h = (uint16)xwa.height;
}

void set_window_size( syswindow_t* window, uint16 w, uint16 h )
{
	XWindowChanges xwc;

	if ( window == NULL ) return;

	xwc.width = (int)w;
	xwc.height = (int)h;

	XConfigureWindow( window->display, window->wnd, CWWidth|CWHeight, &xwc );
}

void redraw_window( syswindow_t* window )
{
	XWindowAttributes xwa;
	XExposeEvent event;

	if ( window == NULL ) return;

	XGetWindowAttributes( window->display, window->wnd, &xwa );

	event.type = Expose;
	event.serial = 0;
	event.send_event = True;
	event.display = window->display;
	event.window = window->wnd;
	event.x = 0;
	event.y = 0;
	event.width = xwa.width;
	event.height = xwa.height;
	event.count = 0;

	XSendEvent( window->display, window->wnd, False, ExposureMask, (XEvent*)&event );
}

void copy_to_clipboard( const char_t* text )
{
	// TODO: Add copy_to_clipboard
	UNREFERENCED_PARAM( text );
}

const char_t* paste_from_clipboard( void )
{
	// TODO: Add paste_from_clipboard
	return NULL;
}

void set_mouse_cursor( MOUSECURSOR cursor )
{
	// TODO: Add set_mouse_cursor
	UNREFERENCED_PARAM( cursor );
}

#endif
