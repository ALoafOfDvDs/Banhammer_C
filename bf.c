//clang-format off

#include "bf.h"
#include "bv.h"

#include "city.h"
#include "ht.h"
#include "ll.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#define N_HASHES 5
//clang-format on

struct BloomFilter {
  uint64_t salts[N_HASHES];
  uint32_t n_keys;
  uint32_t n_hits;
  uint32_t n_misses;
  uint32_t n_bits_examined;
  uint32_t size;

  BitVector *filter;

  uint64_t counter;
  uint64_t collision_tracker;
};

static uint64_t default_salts[] = {0x5adf08ae86d36f21, 0x419d292ea2ffd49e,
                                   0x50d8bb08de3818df, 0x272347aea4045dd5,
                                   0x7c8e16f768811a21};

BloomFilter *bf_create(uint32_t size) {
  BloomFilter *bf = (BloomFilter *)malloc(sizeof(BloomFilter));
  if (bf) {
    bf->counter = bf->collision_tracker = 0;

    // printf("got to here in bf create\n");
    bf->n_keys = bf->n_hits = 0;
    bf->n_misses = bf->n_bits_examined = 0;
    for (int i = 0; i < N_HASHES; i++) {
      bf->salts[i] = default_salts[i];
    }
    bf->size = size;
    bf->filter = bv_create(size);

    if (bf->filter == NULL) {
      free(bf);
      printf("bloomfilter's filter is null\n");
      bf = NULL;
    }
    return bf;
  }
  return NULL;
}

void bf_delete(BloomFilter **bf) {
  bv_delete(&((*bf)->filter));
  // free((*bf)->salts);
  free(*bf);
  *bf = NULL;
}

uint32_t bf_size(BloomFilter *bf) { return bf->size; }

void bf_insert(BloomFilter *bf, char *oldspeak) {
  bf->n_keys += 1;
  for (int i = 0; i < N_HASHES; i += 1) {
    uint64_t hash_value = hash(bf->salts[i], oldspeak);
    hash_value = hash_value % bf->size; // THIS IS A MASSIVELY IMPORTANT CHANGE

    uint8_t value1 = bv_get_bit(bf->filter, hash_value);

    // printf("hash value #%d: %lu\n",i, hash_value);
    bv_set_bit(bf->filter, hash_value); // this doesnt feel like it should work

    uint8_t value2 = bv_get_bit(bf->filter, hash_value);
    if (value1 != value2) {
      bf->counter += 1;
    }
    if (value1 == value2) {
      bf->collision_tracker += 1;
    }
  }
  // printf("bloomfilter insertion count: %lu\n", bf->counter);
  // printf("bloomfilter collision count: %lu\n", bf->collision_tracker);
}

bool bf_probe(BloomFilter *bf, char *oldspeak) {
  // printf("got here with string %s\n", oldspeak);
  for (int i = 0; i < N_HASHES; i += 1) {
    bf->n_bits_examined += 1;
    uint64_t hash_value = hash(bf->salts[i], oldspeak);
    hash_value = hash_value % bf->size;
    // printf("probed hash value #%d: %lu\n",i, hash_value);
    int bit_val = bv_get_bit(bf->filter, hash_value);
    // printf("right before bitval check %d\n", bit_val);
    if (!bit_val) {
      // printf("got into here with probe\n");
      bf->n_misses += 1;
      return false;
    }
  }
  bf->n_hits += 1;
  return true;
}

// static uint32_t count(uint64_t val) {
//     if (val == 0) {
//         return 0;
//     }
//     else {
//         return (val & 1) + count(val >> 1);
//     }
// }

uint32_t bf_count(BloomFilter *bf) {
  BitVector *bv = bf->filter;
  uint32_t length = bv_length(bv);
  // uint32_t num_ints = (length / 64) + ((length % 64) != 0 ? 1 : 0);
  uint64_t counter = 0;
  for (uint32_t i = 0; i < length; i += 1) {
    counter += bv_get_bit(bf->filter, i);
    // printf("bit #%u: %lu\n", i, counter);
  }
  // for (uint32_t i = 0; i < num_ints; i += 1) { // This works best because I
  // can make sure i am not editing the bitvector at all by passing the uint64_t
  // by VALUE

  //     BitVector * bv = bf->filter;
  //     uint64_t * vector_list = (*bv).vector;
  //     uint64_t value = vector_list[i];
  //     counter += count(value);
  // }
  return counter;
}

void bf_print(BloomFilter *bf) {
  printf("keys: %" PRIu32 "\nhits: %" PRIu32 "\nmisses: %" PRIu32
         "\nbits examined: %" PRIu32 "\nsize: %" PRIu32 "\n",
         bf->n_keys, bf->n_hits, bf->n_misses, bf->n_bits_examined, bf->size);
  // BitVector * bv = bf->filter;
  // uint32_t length = bv_length(bv);
  // uint32_t num_ints = (length / 64) + ((length % 64) != 0 ? 1 : 0);
  bv_print(bf->filter);
}

void bf_stats(BloomFilter *bf, uint32_t *nk, uint32_t *nh, uint32_t *nm,
              uint32_t *ne) {
  *nk = bf->n_keys;
  *nh = bf->n_hits;
  *nm = bf->n_misses;
  *ne = bf->n_bits_examined;
  return;
}
