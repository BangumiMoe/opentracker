
#ifndef __BGM_NOTIFY_H__
#define __BGM_NOTIFY_H__

#ifdef WANT_NOTIFY

extern ot_ip6 g_notify_ip;
extern uint16_t g_notify_port;
extern char * g_notify_path;
extern unsigned int g_notify_interval;

void bgm_notify_init( );
void bgm_notify_deinit( );

int notify_torrent_update( ot_torrent *t, int iscompleted );

#endif

#endif
