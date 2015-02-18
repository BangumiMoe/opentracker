
/* bgm simple dynamic string */

#ifndef __BGM_SDS_H__
#define __BGM_SDS_H__

#ifdef _DEBUG_BANGUMI
#define bangumi_debug_print(...) fprintf(stderr, __VA_ARGS__)
#else
#define bangumi_debug_print(...)
#endif

typedef struct {
  char *data;
  size_t space;
} bgm_sds;

void sds_init( bgm_sds *dest );
void sds_free( bgm_sds *dest );
int  sds_strcpy( bgm_sds *dest, char *src );
int  sds_strcat( bgm_sds *dest, char *src );

#endif
