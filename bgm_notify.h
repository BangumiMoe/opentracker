
#ifndef __BGM_NOTIFY_H__
#define __BGM_NOTIFY_H__

#ifdef WANT_NOTIFY

#include "trackerlogic.h"

void bgm_notify_init( );
void bgm_notify_deinit( );

int notify_torrent_update( ot_torrent *t, int iscompleted );

#endif

#endif
