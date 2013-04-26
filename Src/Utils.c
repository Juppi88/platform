/**********************************************************************
 *
 * PROJECT:		Platform library
 * FILE:		Utils.c
 * LICENCE:		See Licence.txt
 * PURPOSE:		Various platform independent utility functions.
 *
 *				(c) Tuomo Jauhiainen 2013
 *
 **********************************************************************/

#include "Platform/Utils.h"

#ifdef _WIN32

//////////////////////////////////////////////////////////////////////////
// Win32 implementation
//////////////////////////////////////////////////////////////////////////

#include <direct.h>

char* get_working_directory( char* buffer, size_t len )
{
	return _getcwd( buffer, (int)len );
}

void exit_app_with_error( const char_t* errormsg )
{
	if ( errormsg && *errormsg )
	{
		MessageBox( 0, errormsg, "Error", MB_OK );
	}

	ExitProcess( 1 );
}

#else

//////////////////////////////////////////////////////////////////////////
// POSIX implementation
//////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <stdlib.h>

char* get_working_directory( char* buffer, size_t len )
{
	return getcwd( buffer, len );
}

void exit_app_with_error( const char_t* errormsg )
{
	exit( EXIT_FAILURE );
}

#endif
