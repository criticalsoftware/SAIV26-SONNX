#include "ctensorint64.h"
struct ctensorint64;


struct ctensorint64 ctensorint64_create(int32_t * ds, int32_t n) {
  int32_t m;
  int64_t * vs;
  struct ctensorint64 ctensorint64;
  m = cdim_size(ds, n);
  vs = malloc(((uint32_t) m) * sizeof(int64_t));
  ctensorint64.t_rank = !vs ? 0 : n;
  ctensorint64.t_dims = ds;
  ctensorint64.t_data = vs;
  return ctensorint64;
}

void ctensorint64_clear(struct ctensorint64 r) {
  int32_t m, i, o;
  m = cdim_size(r.t_dims, r.t_rank);
  o = m - 1;
  if (0 <= o) {
    for (i = 0; ; ++i) {
      r.t_data[i] = INT64_C(0);
      if (i == o) {
        break;
      }
    }
  }
}

void ctensorint64_reset(struct ctensorint64 r, int64_t v) {
  int32_t m, i, o;
  m = cdim_size(r.t_dims, r.t_rank);
  o = m - 1;
  if (0 <= o) {
    for (i = 0; ; ++i) {
      r.t_data[i] = v;
      if (i == o) {
        break;
      }
    }
  }
}
