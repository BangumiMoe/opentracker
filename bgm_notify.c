
/* System */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <pthread.h>
#include <ndelay.h>
/* Libowfat */
#include "byte.h"
#include "io.h"
#include "iob.h"
#include "array.h"
#include "ip6.h"
#include "socket.h"
/* bgm */
#include "bgm_notify.h"
#include "bgm_sds.h"

#ifdef WANT_NOTIFY

#ifdef _DEBUG_BANGUMI_NOTIFY
#define bangumi_debug_print(...) fprintf(stderr, __VA_ARGS__)
#else
#define bangumi_debug_print(...)
#endif

extern ot_ip6   g_notify_ip;
extern uint16_t g_notify_port;
extern char *   g_notify_path;
extern uint32_t g_notify_interval;

static pthread_mutex_t bangumi_poster_mutex;
static ot_vector bangumi_poster_vector;

static bgm_sds sz_post_body;
static bgm_sds sz_post;

static char*to_hex(char*d, uint8_t*s){ char*m = "0123456789ABCDEF"; char *t = d; char*e = d + 40; while (d<e){ *d++ = m[*s >> 4]; *d++ = m[*s++ & 15]; }*d = 0; return t; }

int notify_torrent_update( ot_torrent *t, int iscompleted ) {
  int         exactmatch;
  char        hex_out[42];
  ot_torrent *torrent;

  bangumi_debug_print("new torrent notifity: %s\n", to_hex(hex_out, t->hash));

  pthread_mutex_lock ( &bangumi_poster_mutex );

  //insert torrent to vector
  torrent = vector_find_or_insert(&bangumi_poster_vector, t->hash, sizeof( ot_torrent ), OT_HASH_COMPARE_SIZE, &exactmatch);
  if ( !torrent ) {
    fprintf(stderr, "bangumi: resize the vector failed\n");
    pthread_mutex_unlock ( &bangumi_poster_mutex );
    return -1; //resize the vector failed
  }

  if( !exactmatch ) {
    memcpy( torrent, t, sizeof(ot_torrent) );
  } else {
    torrent->peer_list = t->peer_list;
  }

  if (iscompleted) torrent->bgm_completed++;
  //the t->bgm_completed should always be zero,
  //only torrent->bgm_completed++ when notify_torrent_update is called with iscompleted=1
  //after post the data to bangumi server, torrent->bgm_completed will be set to zero again due to memcpy

  pthread_mutex_unlock ( &bangumi_poster_mutex );

  bangumi_debug_print(exactmatch ? "exactmatch torrent: %s\n" : "new torrent: %s\n", to_hex(hex_out, torrent->hash));
  bangumi_debug_print("vector size: %zu, space: %zu \n", bangumi_poster_vector.size, bangumi_poster_vector.space);

  return 0;
}

static void * bangumi_poster(void * args) {
  const char sz_post_data_element_f[] = "{\"action\":\"%s\",\"infoHash\":\"%s\",\"data\":{\"completed\":%u,\"downs\":%zu,\"peers\":%zu,\"seeds\":%zu}}";
  const char sz_post_header_f[] =
    "POST /%s HTTP/1.1\r\n"
    "Host: %s:%u\r\n"
    "User-Agent: opentracker/mod\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: %lu\r\n\r\n";

  char hex_out[42];
  char sz_post_data_element[512], sz_post_header[1024];
  char szip[80];
  int64 sock;
  uint32_t datalen;
  size_t i;

  while (1) {
    sleep(g_notify_interval);

    pthread_mutex_lock ( &bangumi_poster_mutex );
    bangumi_debug_print("Bangumi Poster Work Thread! \n");

    size_t member_count = bangumi_poster_vector.size;
    if (member_count == 0) goto fail_lock;
    sds_strcpy(&sz_post_body, "[");

    for ( i = 0; i < member_count; i++ ) {

      ot_torrent *torrent = (ot_torrent *) (bangumi_poster_vector.data + sizeof(ot_torrent) * i);

      bangumi_debug_print("POST TORRENT %s!\n", to_hex(hex_out, torrent->hash));

      sprintf(sz_post_data_element, sz_post_data_element_f, "update",
        to_hex(hex_out, torrent->hash), torrent->bgm_completed,
        torrent->peer_list->down_count, torrent->peer_list->peer_count, torrent->peer_list->seed_count);

      sds_strcat(&sz_post_body, sz_post_data_element);
      if (i < member_count - 1) sds_strcat(&sz_post_body, ",");
    }

    sds_strcat(&sz_post_body, "]");

    sock = socket_tcp6();

    if (sock < 0 || !io_fd(sock)) goto fail_lock;
    if (ndelay_off(sock) == -1) goto fail_socket;
    if (socket_connect6(sock, g_notify_ip, g_notify_port, 0) == -1 /*&&
      errno != EINPROGRESS && errno != EWOULDBLOCK*/) goto fail_socket;

    datalen = fmt_ip6c(szip, g_notify_ip);
    szip[datalen] = 0;

    sprintf(sz_post_header, sz_post_header_f,
      g_notify_path, szip, g_notify_port, sds_strlen(&sz_post_body));

    sds_strcpy(&sz_post, sz_post_header);
    sds_strcat(&sz_post, sz_post_body.data);

    datalen = sds_strlen(&sz_post);

    bangumi_debug_print("all request data(%d):\n%s\n", datalen, sz_post.data);

    if (io_waitwrite(sock, sz_post.data, datalen) > 0) {
      //post successfully, clear the vector
      bangumi_poster_vector.size = 0;
    }

  fail_socket:
    io_close(sock);

  fail_lock:
    pthread_mutex_unlock ( &bangumi_poster_mutex );
    continue;
  }

  return NULL;
}

static pthread_t bangumi_thread_id;
void bgm_notify_init( ) {
  byte_zero( &bangumi_poster_vector, sizeof( ot_vector ) );

  sds_init( &sz_post );
  sds_init( &sz_post_body );

  pthread_mutex_init( &bangumi_poster_mutex, NULL );
  pthread_create( &bangumi_thread_id, NULL, bangumi_poster, NULL );
}

void bgm_notify_deinit( ) {
  byte_zero( &bangumi_poster_vector, sizeof( ot_vector ) );
  pthread_cancel( bangumi_thread_id );
  pthread_mutex_destroy( &bangumi_poster_mutex );

  sds_free( &sz_post );
  sds_free( &sz_post_body );
}

#endif
