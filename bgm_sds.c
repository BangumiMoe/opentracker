
/* System */
#include <stdlib.h>
#include <stdio.h>

#include "trackerlogic.h"

#include "bgm_sds.h"

void sds_init( bgm_sds *dest )
{
  //memset( dest, 0, sizeof(bgm_sds) );
  dest->data = NULL;
  dest->space = 0;
  sds_realloc( dest, 1 );
}

void sds_free( bgm_sds *dest )
{
  free(dest->data);
  dest->data = NULL;
  dest->space = 0;
}

int sds_realloc( bgm_sds *dest, size_t new_size )
{
  char  *new_data = NULL;
  size_t new_space;

  if ( new_size > dest->space ) {
    new_space = (new_size / 1024 + 1) * 1024;
    if ( dest->space == 0 ) {
      new_data = malloc( new_space * sizeof(char) );
      if ( new_data ) {
        *new_data = 0;
      }
    } else {
      new_data = realloc( dest->data, new_space * sizeof(char) );
    }
    if( !new_data ) {
      exerr("bangumi: sds realloc data error");
      return 0;
    }
#ifdef _DEBUG_BANGUMI_SDS
    fprintf(stderr, "size %zu space %zu -> %zu\n", dest, dest->data ? sds_strlen(dest) : 0, dest->space, new_space);
#endif
    dest->space = new_space;
    dest->data = new_data;
  }

  return 1;
}

int sds_strcpy( bgm_sds *dest, char *src )
{
  if (sds_realloc( dest, strlen(src) + 1 )) {
    strcpy(dest->data, src);
    return 1;
  }
  return 0;
}

int sds_strcat( bgm_sds *dest, char *src )
{
  if (sds_realloc( dest, sds_strlen(dest) + strlen(src) + 1 )) {
    strcat(dest->data, src);
    return 1;
  }
  return 0;
}

size_t sds_strlen( bgm_sds *dest )
{
  return strlen( dest->data );
}
