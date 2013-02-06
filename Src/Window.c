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

syswindow_t* create_system_window( int x, int y, int w, int h, char_t* title, bool border )
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
			x, y, w, h, NULL, NULL, GetModuleHandle(NULL), NULL );
	}
	else
	{
		window = CreateWindowEx( WS_EX_APPWINDOW, wc.lpszClassName, title,
			WS_POPUP, x, y, w, h, NULL, NULL, GetModuleHandle(NULL), NULL );

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

void window_pos_to_screen( syswindow_t* window, uint16* x, uint16* y )
{
	POINT p;
	p.x = (int)*x, p.y = (int)*y;

	ClientToScreen( (HWND)window, &p );

	*x = (uint16)p.x;
	*y = (uint16)p.y;
}

void get_window_pos( syswindow_t* window, uint16* x, uint16* y )
{
	RECT rect;

	GetWindowRect( (HWND)window, &rect );

	*x = (uint16)rect.left;
	*y = (uint16)rect.top;
}

void set_window_pos( syswindow_t* window, uint16 x, uint16 y )
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

	data = (char*)GlobalLock( mem );

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

// TODO!

#endif
