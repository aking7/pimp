#include "pimp.h"
#include "tests.h"
#include "unity.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void setUp(void) {}
void tearDown(void) {}

void test_create_sweep(void) {
    AudioBuf* expected = audiobuf_from_wav("../tests/data/sweep.wav");
    AudioBuf* actual   = create_sweep(1, 1000, 0.5, 0.5);

    assert(expected->length == actual->length);

    TEST_ASSERT_DOUBLE_ARRAY_WITHIN(1e-7, expected->data, actual->data, expected->length);

    audiobuf_destroy(expected);
    audiobuf_destroy(actual);
}

void test_estimate_tf(void) {
    AudioBuf* in  = audiobuf_from_wav("../tests/data/sweep.wav");
    AudioBuf* out = audiobuf_from_wav("../tests/data/sweep_w.wav");
    assert(in->length == out->length);

    size_t length     = 5;
    pfloat expected[] = {0, 0.5, -0.5, 0, 0};

    RLSFilter* filt = rls_new(length, 1, 10);
    rls_train(filt, in->data, out->data, in->length);

    TEST_ASSERT_DOUBLE_ARRAY_WITHIN(1e-7, expected, filt->w, length);

    audiobuf_destroy(in);
    audiobuf_destroy(out);
    rls_destory(filt);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_sweep);
    RUN_TEST(test_estimate_tf);
    return UNITY_END();
}