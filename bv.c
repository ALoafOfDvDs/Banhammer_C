//clang-format off
#include "bv.h"
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//clang-format on

struct BitVector {
  uint32_t length;
  uint64_t *vector;
};

BitVector *bv_create(uint32_t length) {
  BitVector *bv = (BitVector *)malloc(sizeof(BitVector));
  if (bv) {
    bv->length = length;
    uint32_t num_64ints_reqd = (length / 64) + ((length % 64) != 0 ? 1 : 0);
    bv->vector = (uint64_t *)calloc(num_64ints_reqd, sizeof(uint64_t));
    if (!bv->vector) {
      free(bv);
      bv = NULL;
    }
    for (uint32_t i = 0; i < num_64ints_reqd; i += 1) {
      bv->vector[i] = 0;
    }
    // printf("got here in bitvector\n");
    return bv;
  }
  return NULL;
}

void bv_delete(BitVector **bv) {
  free((*bv)->vector);
  // (*bv)->vector = NULL;
  free(*bv);
  *bv = NULL;
}

uint32_t bv_length(BitVector *bv) { return bv->length; }

void bv_set_bit(BitVector *bv, uint32_t i) {
  int correct_index = i / 64;
  uint64_t bit_to_set = i % 64;
  // printf("value out of bounds: %u\n", i);
  uint64_t mask = (1UL << bit_to_set);
  // uint64_t vector = mask & (bv->vector)[correct_index];
  // if(vector) {
  //     return;
  // }
  // else {
  //     (bv->vector)[correct_index] += (1 << bit_to_set);
  // }
  (bv->vector)[correct_index] |= mask;

  // Yes i just realized after writing this code if I had just bitwise-ORed the
  // numbers I wouldnt have had the problem to fix in the first place
}

void bv_clr_bit(BitVector *bv, uint32_t i) {
  int correct_index = i / 64;
  int bit_to_set = i % 64;
  // uint64_t new_vector = 0
  // for(int j = 0; j < 64; j += 1){
  //     new_vector += (1 << j);
  // }

  (bv->vector)[correct_index] =
      (bv->vector)[correct_index] ^ (1UL << bit_to_set);
}

uint8_t bv_get_bit(BitVector *bv, uint32_t i) {
  int correct_index = i / 64;
  uint64_t bit_to_set = i % 64;

  uint64_t mask = (1UL << bit_to_set);
  uint64_t vector = mask & (bv->vector)[correct_index];
  // printf("vector value %lu\n", vector);
  if (vector > 0) {
    return 1;
  } else {
    return 0;
  }
}

void bv_print(BitVector *bv) {
  printf("length: %" PRIu32 "\n", bv->length);
  uint32_t num64bits = (bv->length / 64) + ((bv->length % 64) != 0 ? 1 : 0);
  for (uint32_t i = 0; i < num64bits; i += 1) {
    printf("vector #%" PRIu32 ": %lu\n", i, bv->vector[i]);
  }
}
