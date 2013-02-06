/**********************************************************************
 *
 * PROJECT:		Platform library
 * FILE:		Timer.h
 * LICENCE:		See Licence.txt
 * PURPOSE:		Platform independent timer and time related functions.
 *
 *				(c) Tuomo Jauhiainen 2013
 *
 **********************************************************************/

#pragma once
#ifndef __LIB_PLATFORM_TIMER_H
#define __LIB_PLATFORM_TIMER_H

#include "stdtypes.h"

typedef void systimer_t;

__BEGIN_DECLS

MYLLY_API uint32		get_tick_count			( void );

MYLLY_API systimer_t*	systimer_create			( float interval );
MYLLY_API void			systimer_destroy		( systimer_t* timer );
MYLLY_API float			systimer_wait			( systimer_t* timer, bool wait );

__END_DECLS

#endif /* __LIB_PLATFORM_TIMER_H */
