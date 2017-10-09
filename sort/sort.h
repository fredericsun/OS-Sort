#ifndef __sort_h__
#define __sort_h__

// DO NOT EDIT THIS FILE
#include <limits.h>
#define MAX_DATA_INTS USHRT_MAX

typedef struct __rec_dataptr_t {
  unsigned int index;
  unsigned int data_ints;
  unsigned int *data_ptr;
} rec_dataptr_t;

typedef struct __rec_data_t {
  unsigned int index;
  unsigned int data_ints;
  unsigned int data[MAX_DATA_INTS];
} rec_data_t;

typedef struct __rec_nodata_t {
  unsigned int index;
  unsigned int data_ints;
} rec_nodata_t;



#endif // __sort_h__

