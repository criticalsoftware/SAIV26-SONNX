#ifndef CTENSORINT64_H_INCLUDED

#include <stdlib.h>
#include <stdint.h>
#include "cindex.h"

struct ctensorint64 {
  int32_t t_rank;
  int32_t * t_dims;
  int64_t * t_data;
};

struct ctensorint64 ctensorint64_create(int32_t * ds, int32_t n);

void ctensorint64_clear(struct ctensorint64 r);

void ctensorint64_reset(struct ctensorint64 r, int64_t v);

#define CTENSORINT64_H_INCLUDED
#endif // CTENSORINT64_H_INCLUDED
