#ifndef __PIMP_H__
#define __PIMP_H__

#include <assert.h>
#include <complex.h>
#include <stdlib.h>

#define CHECK_ALLOC(errcode) assert(errcode)

#ifdef USE_DOUBLE
#define DTYPE double
#else
#define DTYPE float
#endif

typedef DTYPE         pfloat;
typedef complex DTYPE pcomplex;

void left_extend(size_t len, pfloat buf[len], pfloat x);
void block_right_extend(size_t len, size_t blocklen, pfloat buf[len], pfloat x[blocklen]);

typedef struct {
    size_t  len;
    pfloat  stepsize;
    pfloat  leakage;
    pfloat  eps;
    pfloat* w;
} LMSFilter;

LMSFilter* lms_init(size_t len, pfloat stepsize, pfloat leakage);
void       lms_destroy(LMSFilter* self);
void       lms_set_w(LMSFilter* self, pfloat w[self->len]);
void       lms_update(LMSFilter* self, pfloat x[self->len], pfloat e);
pfloat     lms_predict(LMSFilter* self, pfloat x[self->len]);
void       lms_train(LMSFilter* self, size_t n, pfloat xs[n], pfloat ys[n]);

typedef struct {
    size_t   len;   // filter length
    pfloat   alpha; // memory-fading coefficient
    pfloat   r;     // measurement noise variance
    pfloat   q;     // process noise variance
    pfloat*  k;     // Kalman gain
    pfloat*  Px;    // temp
    pfloat*  w;     // Filter estimate
    pfloat** P;     // Filter covariance estimate
} RLSFilter;

RLSFilter* rls_init(size_t len, pfloat alpha, pfloat Pinit);
void       rls_destroy(RLSFilter* self);
void       rls_set_w(RLSFilter* self, pfloat w[self->len]);
void       rls_update(RLSFilter* self, pfloat x[self->len], pfloat e);
pfloat     rls_predict(RLSFilter* self, pfloat x[self->len]);
void       rls_train(RLSFilter* self, size_t n, pfloat xs[n], pfloat ys[n]);

#if defined(PIMP_WITH_POCKETFFT) || defined(PIMP_WITH_NE10)

struct rfft_plan_i;
typedef struct rfft_plan_i* rfft_plan;

rfft_plan make_rfft_plan(size_t n);
void      destroy_rfft_plan(rfft_plan plan);
void      rfft(rfft_plan plan, size_t n, pfloat src[n], pcomplex dest[n / 2 + 1]);
void      irfft(rfft_plan plan, size_t n, pcomplex src[n / 2 + 1], pfloat dest[n]);
size_t    rfft_length(rfft_plan plan);

typedef struct {
    size_t    len;
    size_t    blocklen;
    pfloat    stepsize;
    pfloat    leakage;
    pfloat    eps;
    pcomplex* W;
    rfft_plan plan;
    pcomplex* _Y; // temp array in prediction
    pcomplex* _U; // temp array in update
} BlockLMSFilter;

BlockLMSFilter* blms_init(size_t len, size_t blocklen, pfloat stepsize, pfloat leakage);
void            blms_destroy(BlockLMSFilter* self);
void            blms_set_w(BlockLMSFilter* self, const pfloat w[self->len]);
void            blms_get_w(BlockLMSFilter* self, pfloat w[self->len]);
void            blms_update(BlockLMSFilter* self, const pcomplex X[self->len + 1], pfloat e[self->blocklen]);
void            blms_predict(BlockLMSFilter* self, const pcomplex X[self->len + 1], pfloat y[self->blocklen]);
void            blms_train(BlockLMSFilter* self, size_t n, pfloat x[n], pfloat y[n]);

#endif

#endif /* __PIMP_H__ */