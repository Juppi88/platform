/**********************************************************************
 *
 * PROJECT:		Platform library
 * FILE:		PlatformWin32.c
 * LICENCE:		See Licence.txt
 * PURPOSE:		A library which provides a generic API for platform
 *				dependant functionality, such as shared library loading.
 *				Windows implementation.
 *
 *				(c) Tuomo Jauhiainen 2012
 *
 **********************************************************************/

#ifdef _WIN32

#include "Platform.h"
#include "Stringy/Stringy.h"
#include <stdio.h>
#include <process.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>

static float			_timerfreq = 0.0f;

struct timer_s
{
	HANDLE			handle;
	LARGE_INTEGER	interval;
	LARGE_INTEGER	prev_tick;
};


/**************************************************
	lib_open
**************************************************/
void* lib_open( const char* file )
{
	void* handle;

	handle = (void*)GetModuleHandleA( file );

	if ( !handle )
	{
		handle = (void*)LoadLibraryA( file );
	}

	return handle;
}

/**************************************************
	lib_symbol
**************************************************/
void* lib_symbol( void* handle, const char* name )
{
	if ( !handle ) return NULL;

	return (void*)GetProcAddress( (HMODULE)handle, name );
}

/**************************************************
	lib_close
**************************************************/
int lib_close( void* handle )
{
	if ( !handle ) return 0;

	return FreeLibrary( (HMODULE)handle );
}

/**************************************************
	thread_create
**************************************************/
int thread_create( thread_func_t func, void* arguments )
{
	HANDLE thread;
	uint32 thread_addr = 0;

	thread = (HANDLE)_beginthreadex( NULL, 0, func, arguments, 0, &thread_addr );

	if ( !thread ) return 1;

	CloseHandle( thread );
	return 0;
}

/**************************************************
	thread_sleep
**************************************************/
void thread_sleep( uint32 millisec )
{
	Sleep( millisec );
}

/**************************************************
	mem_alloc
**************************************************/
void* mem_alloc( uint32 size )
{
	void* ptr;
	ptr = malloc( size );

	assert( ptr != NULL );

	return ptr;
}

/**************************************************
	mem_alloc_clean

	Initializes a memory block safely like mem_alloc
	but also initializes the memory to 0.
**************************************************/
void* mem_alloc_clean( uint32 size )
{
	void* ptr;
	ptr = malloc( size );

	assert( ptr != NULL );

	memset( ptr, 0, size );
	return ptr;
}

/**************************************************
	mem_free
**************************************************/
void mem_free( void* ptr )
{
	if ( !ptr ) return;
	free( ptr );
}


/**************************************************
	timer_get_ticks
**************************************************/
uint32 timer_get_ticks( void )
{
	return (uint32)GetTickCount();
}

/**************************************************
	timer_create
**************************************************/
timer_t timer_create( float interval )
{
	timer_t			timer;
	LARGE_INTEGER	frequency;

	if ( !_timerfreq )
	{
		// Cache the frequency first
		QueryPerformanceFrequency( &frequency );
		_timerfreq = (float)frequency.QuadPart;
	}

	timer = (timer_t)mem_alloc( sizeof(*timer) );

	timer->handle				= CreateWaitableTimer( NULL, TRUE, NULL );
	timer->interval.QuadPart	= (LONGLONG)( (double)interval * -10000000.0 );

	QueryPerformanceCounter( &timer->prev_tick );
	SetWaitableTimer( timer->handle, &timer->interval, 0, NULL, NULL, 0 );

	return timer;
}

/**************************************************
	timer_destroy
**************************************************/
void timer_destroy( timer_t timer )
{
	if ( !timer ) return;

	CloseHandle( timer->handle );
	mem_free( timer );
}

/**************************************************
	timer_wait
**************************************************/
float timer_wait( timer_t timer, bool wait )
{
	LARGE_INTEGER tick;
	float delta;

	if ( !timer ) return 0.0f;

	if ( wait )
	{
		WaitForSingleObject( timer->handle, INFINITE );
		SetWaitableTimer( timer->handle, &timer->interval, 0, NULL, NULL, 0 );
	}

	QueryPerformanceCounter( &tick );

	delta = (float)( tick.QuadPart - timer->prev_tick.QuadPart ) / _timerfreq;

	timer->prev_tick = tick;

	return delta;
}

/**************************************************
	create_system_window
**************************************************/
void* create_system_window( int x, int y, int w, int h, char_t* title, bool border )
{
	WNDCLASS wc;
	HWND window;

	ZeroMemory( &wc, sizeof(wc) );

	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DROPSHADOW;
	wc.lpfnWndProc		= DefWindowProc;
	wc.hInstance		= GetModuleHandle(NULL);
	wc.lpszClassName	= _TEXT("mylly_window");
	wc.hCursor			= LoadCursor( NULL, IDC_ARROW );

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

	return (void*)window;
}

/**************************************************
	create_system_window
**************************************************/
void destroy_system_window( void* window )
{
	DestroyWindow( (HWND)window );
}

/**************************************************
	process_messages
**************************************************/
void process_window_messages( void* window, bool (*callback)(void*) )
{
	MSG msg;
	
	while ( PeekMessage( &msg, window, 0, 0, PM_REMOVE ) )
	{
		if ( callback )
		{
			if ( !callback( (void*)&msg ) ) continue;
		}

		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
}

/**************************************************
	is_window_visible
**************************************************/
bool is_window_visible( void* window )
{
	return IsWindowVisible( (HWND)window ) ? true : false;
}

/**************************************************
	window_pos_to_screen
**************************************************/
void window_pos_to_screen( void* window, uint16* x, uint16* y )
{
	POINT p;
	p.x = (int)*x, p.y = (int)*y;

	ClientToScreen( (HWND)window, &p );

	*x = (uint16)p.x;
	*y = (uint16)p.y;
}

/**************************************************
	get_window_pos
**************************************************/
void get_window_pos( void* window, uint16* x, uint16* y )
{
	RECT rect;

	GetWindowRect( (HWND)window, &rect );

	*x = (uint16)rect.left;
	*y = (uint16)rect.top;
}

/**************************************************
	set_window_pos
**************************************************/
void set_window_pos( void* window, uint16 x, uint16 y )
{
	SetWindowPos( (HWND)window, HWND_TOP, x, y, 0, 0, SWP_NOSIZE );
}

/**************************************************
	get_window_size
**************************************************/
void get_window_size( void* window, uint16* w, uint16* h )
{
	RECT rect;

	GetWindowRect( (HWND)window, &rect );

	*w = (uint16)( rect.right - rect.left );
	*h = (uint16)( rect.bottom - rect.top );
}

/**************************************************
	set_window_size
**************************************************/
void set_window_size( void* window, uint16 w, uint16 h )
{
	SetWindowPos( (HWND)window, HWND_TOP, 0, 0, w, h, SWP_NOMOVE );
}

/**************************************************
	copy_to_clipboard
**************************************************/
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

/**************************************************
	paste_from_clipboard
**************************************************/
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
	text = mem_alloc( size );

	mstrcpy( text, data, size );

	GlobalUnlock( mem );

	return text;
}

#endif /* _WIN32 */
