
/* System */
#include <stdlib.h>
#include <stdio.h>

#include "trackerlogic.h"

#include "bgm_sds.h"

void sds_init( bgm_sds *dest )
{
  dest->data = malloc(sizeof(char) * 1);
  dest->space = 1;
  dest->data[0] = 0;
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

  /* new alloc */
  if ( dest->space == 0 ) {
    new_space = 1024;
    new_data = malloc( new_space * sizeof(char) );
    if( !new_data ) {
      exerr("bangumi: sds realloc data error");
      return 0;
    }
    dest->space = new_space;
    dest->data = new_data;
  }

  while ( new_size > dest->space ) {
    new_space = (dest->space >= 1024) ? 2 * dest->space : 1024;
    new_data = realloc( dest->data, new_space * sizeof(char) );
    if( !new_data ) {
      exerr("bangumi: sds realloc data error");
      return 0;
    }
    dest->space = new_space;
    dest->data = new_data;
  }

  if ( new_data ) {
    bangumi_debug_print("size %zu space %zu\n", strlen(dest->data), dest->space);
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
  if (sds_realloc( dest, strlen(dest->data) + strlen(src) + 1 )) {
    strcat(dest->data, src);
    return 1;
  }
  return 0;
}
