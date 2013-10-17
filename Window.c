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

#ifndef MYLLY_PLATFORM_MINIMAL

#include "Platform/Window.h"
#include "Platform/Alloc.h"
#include "Stringy/Stringy.h"

#ifdef _WIN32

//////////////////////////////////////////////////////////////////////////
// Win32 implementation
//////////////////////////////////////////////////////////////////////////

struct WndCallbacks
{
	void ( *window_message )( void* packet );
};

static LONG_PTR __stdcall wnd_proc( HWND hwnd, uint32 message, WPARAM wparam, LPARAM lparam )
{
	struct WndCallbacks* cbstruct;
	CREATESTRUCT* create;
	MSG msg;

	switch ( message )
	{
	case WM_CREATE:
		create = (CREATESTRUCT*)lparam;
		SetWindowLongPtr( hwnd, GWLP_USERDATA, (LONG_PTR)create->lpCreateParams );
		break;

	case WM_CLOSE:
		DestroyWindow( hwnd );
		break;

	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;
	}

	cbstruct = (struct WndCallbacks*)GetWindowLongPtr( hwnd, GWLP_USERDATA );
	if ( cbstruct )
	{
		msg.hwnd = hwnd;
		msg.message = message;
		msg.wParam = wparam;
		msg.lParam = lparam;

		cbstruct->window_message( (void*)&msg );
	}

	return DefWindowProc( hwnd, message, wparam, lparam );
}

syswindow_t* create_system_window( int32 x, int32 y, uint32 w, uint32 h, const char_t* title, bool decoration, wnd_message_cb cb )
{
	WNDCLASS wc;
	HWND window;
	struct WndCallbacks* cbstruct = NULL;

	ZeroMemory( &wc, sizeof(wc) );

	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DROPSHADOW;
	wc.lpfnWndProc		= wnd_proc;
	wc.hInstance		= GetModuleHandle( NULL );
	wc.lpszClassName	= _MTEXT("mylly_window");
	wc.hCursor			= NULL;

	RegisterClass( &wc );

	if ( cb )
	{
		cbstruct = (struct WndCallbacks*)mem_alloc( sizeof(*cbstruct) );
		cbstruct->window_message = cb;
	}

	if ( decoration )
	{
		window = CreateWindowEx( (WS_EX_WINDOWEDGE|WS_EX_APPWINDOW), wc.lpszClassName, title,
			(WS_VISIBLE|WS_OVERLAPPEDWINDOW|WS_CLIPSIBLINGS|WS_CLIPCHILDREN) & ~(WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME),
			x, y, (int32)w, (int32)h, NULL, NULL, GetModuleHandle(NULL), cbstruct );
	}
	else
	{
		window = CreateWindowEx( WS_EX_APPWINDOW, wc.lpszClassName, title,
			WS_POPUP, x, y, (int32)w, (int32)h, NULL, NULL, GetModuleHandle(NULL), cbstruct );

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

void process_window_messages( syswindow_t* window, wnd_message_cb callback )
{
	MSG msg;
	struct WndCallbacks* cbstruct;

	cbstruct = (struct WndCallbacks*)GetWindowLongPtr( (HWND)window, GWLP_USERDATA );

	while ( PeekMessage( &msg, (HWND)window, 0, 0, PM_REMOVE ) )
	{
		if ( callback && cbstruct == NULL )
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

void get_window_drawable_size( syswindow_t* window, uint16* w, uint16* h )
{
	RECT rect;

	GetClientRect( (HWND)window, &rect );

	*w = (uint16)( rect.right - rect.left );
	*h = (uint16)( rect.bottom - rect.top );
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

void clipboard_copy( syswindow_t* window, const char_t* text )
{
	size_t size;
	HGLOBAL mem;
	char_t* data;

	UNREFERENCED_PARAM( window );

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

void clipboard_paste( syswindow_t* window, clip_paste_cb cb, void* cbdata )
{
	HANDLE mem;
	size_t size;
	const char_t* data;
	static char_t* text = NULL;

	UNREFERENCED_PARAM( window );

	if ( cb == NULL ) return;

	if ( !OpenClipboard( NULL ) ) return;
	if ( !IsClipboardFormatAvailable( data_mode ) ) return;

	mem = GetClipboardData( data_mode );

	if ( text ) mem_free( text ); // Free previously pasted text
	data = (const char_t*)GlobalLock( mem );

	if ( data == NULL ) return;

	size = mstrsize( data );
	text = (char_t*)mem_alloc( size );

	mstrcpy( text, data, size );

	GlobalUnlock( mem );

	CloseClipboard();

	cb( text, cbdata );
}

void set_mouse_cursor( syswindow_t* window, MOUSECURSOR cursor )
{
	static HCURSOR cursors[NUM_CURSORS] = { NULL };

	UNREFERENCED_PARAM( window );

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

static Display*			display				= NULL;
static uint32			display_refcount	= 0;
static clip_paste_cb	paste_cb			= NULL;
static void*			paste_data			= NULL;
static size_t			clipbrd_buf_len		= 0;
static char				clipbrd_buf[1024];

#define XC_X_cursor 0
#define XC_crosshair 34
#define XC_fleur 52
#define XC_left_ptr 68
#define XC_xterm 152

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

syswindow_t* create_system_window( int32 x, int32 y, uint32 w, uint32 h, const char_t* title, bool decoration, wnd_message_cb cb )
{
	Window wnd;
	int screen;
	syswindow_t* window;
	struct MWMHints hints;
	Atom prop;

	if ( display == NULL ) display = XOpenDisplay( NULL );
	if ( display == NULL ) return NULL;

	display_refcount++; // Display reference count

	screen = DefaultScreen( display );
	wnd = XCreateSimpleWindow( display, RootWindow( display, screen ), x, y, w, h, decoration ? 1 : 0,
							   BlackPixel( display, screen ), WhitePixel( display, screen ) );

	XSelectInput( display, wnd, ExposureMask|KeyPressMask|KeyReleaseMask|PointerMotionMask|ButtonPressMask|ButtonReleaseMask );
	XMapWindow( display, wnd );
	XStoreName( display, wnd, title );

	window = mem_alloc( sizeof(*window) );
	window->display = display;
	window->window = wnd;
	window->root = RootWindow( display, DefaultScreen( display ) );
	window->cb = cb;

	if ( !decoration )
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

	XDestroyWindow( window->display, window->window );

	if ( --display_refcount == 0 )
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

		if ( window->cb )
			window->cb( &event );

		else if ( callback )
			callback( &event );
	}
}

bool is_window_visible( syswindow_t* window )
{
	XWindowAttributes xwa;
	XGetWindowAttributes( window->display, window->window, &xwa );

	return ( xwa.map_state == IsViewable );
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

	XTranslateCoordinates( window->display, window->window, window->root,
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

	XTranslateCoordinates( window->display, window->window, window->root,
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

	XConfigureWindow( window->display, window->window, CWX|CWY, &xwc );
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

	XGetWindowAttributes( window->display, window->window, &xwa );

	*w = (uint16)xwa.width;
	*h = (uint16)xwa.height;
}

void set_window_size( syswindow_t* window, uint16 w, uint16 h )
{
	XWindowChanges xwc;

	if ( window == NULL ) return;

	xwc.width = (int)w;
	xwc.height = (int)h;

	XConfigureWindow( window->display, window->window, CWWidth|CWHeight, &xwc );
}

void redraw_window( syswindow_t* window )
{
	XWindowAttributes xwa;
	XExposeEvent event;

	if ( window == NULL ) return;

	XGetWindowAttributes( window->display, window->window, &xwa );

	event.type = Expose;
	event.serial = 0;
	event.send_event = True;
	event.display = window->display;
	event.window = window->window;
	event.x = 0;
	event.y = 0;
	event.width = xwa.width;
	event.height = xwa.height;
	event.count = 0;

	XSendEvent( window->display, window->window, False, ExposureMask, (XEvent*)&event );
}

void clipboard_copy( syswindow_t* window, const char_t* text )
{
	Atom atom;

	if ( window == NULL ) return;
	if ( text == NULL ) return;

	atom = XInternAtom( window->display, "CLIPBOARD", True );
	if ( atom == None ) return;

	mstrcpy( (char_t*)clipbrd_buf, text, sizeof(clipbrd_buf)/sizeof(char_t) );
	clipbrd_buf_len = mstrlen( text );

	XSetSelectionOwner( window->display, atom, window->window, CurrentTime );
}

void clipboard_paste( syswindow_t* window, clip_paste_cb cb, void* data )
{
	Atom atom;

	if ( window == NULL ) return;

	atom = XInternAtom( window->display, "CLIPBOARD", True );
	if ( atom == None ) return;

	paste_cb = cb;
	paste_data = data;

	XConvertSelection( window->display, atom, XA_STRING, XA_STRING, window->window, CurrentTime );
}

void clipboard_handle_event( syswindow_t* window, void* packet )
{
	XSelectionEvent* event;
	XSelectionRequestEvent* select;
	XEvent response;
	Atom type;
	int32 format;
	unsigned long items, bytes;
	uint8* buf;

	if ( window == NULL ) return;
	event = (XSelectionEvent*)packet;

	switch ( event->type )
	{
	case SelectionNotify:
		if ( event->property == None )
		{
			if ( paste_cb ) paste_cb( NULL, paste_data );

			paste_cb = NULL;
			paste_data = NULL;
			break;
		}

		XGetWindowProperty( window->display, window->window, event->property,
							0, (~0L), False, AnyPropertyType, &type, &format,
							&items, &bytes, &buf );

		paste_cb( (const char*)buf, paste_data );

		paste_cb = NULL;
		paste_data = NULL;
		break;

	case SelectionRequest:
		select = (XSelectionRequestEvent*)event;

		XChangeProperty( window->display, select->requestor, select->property,
						 select->target, 8, PropModeReplace, (uint8*)clipbrd_buf, (int32)clipbrd_buf_len );

		memset( &response, 0, sizeof(response) );

		response.xselection.type = SelectionNotify;
		response.xselection.send_event = True;
		response.xselection.display = select->display;
		response.xselection.requestor = select->requestor;
		response.xselection.selection = select->selection;
		response.xselection.target = select->target;
		response.xselection.time = select->time;
		response.xselection.property = select->property;

		XSendEvent( window->display, select->requestor, False, NoEventMask, &response );
		break;
	}
}

void set_mouse_cursor( syswindow_t* window, MOUSECURSOR cursor )
{
	static Cursor cursors[NUM_CURSORS] = { 0 };

	if ( cursor >= NUM_CURSORS ) return;

	if ( !cursors[cursor] )
	{
		// The cursor needs to be loaded first
		switch ( cursor )
		{
		case CURSOR_TEXT:
			cursors[cursor] = XCreateFontCursor( window->display, XC_xterm );
			break;

		case CURSOR_CROSSHAIR:
			cursors[cursor] = XCreateFontCursor( window->display, XC_crosshair );
			break;

		case CURSOR_MOVE:
			cursors[cursor] = XCreateFontCursor( window->display, XC_fleur );
			break;

		case CURSOR_FORBIDDEN:
			cursors[cursor] = XCreateFontCursor( window->display, XC_X_cursor );
			break;

		case CURSOR_ARROW:
		default:
			cursors[cursor] = XCreateFontCursor( window->display, XC_left_ptr );
			break;
		}
	}

	XDefineCursor( window->display, window->window, cursors[cursor] );
}

#endif /* _WIN32 */
#endif /* MYLLY_PLATFORM_MINIMAL */
