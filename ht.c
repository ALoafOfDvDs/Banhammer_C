//clang-format off
#include "ht.h"
#include "city.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

//clang-format on

struct HashTable {
  bool mtf;
  uint64_t salt;
  uint32_t size;
  uint32_t count; // this is the only extra variable that I personally added. It
                  // increments when a new linked list is created within the
                  // hashtable
  uint32_t n_keys;
  uint32_t n_hits;
  uint32_t n_misses;
  uint32_t n_examined;
  LinkedList **lists;
};

HashTable *ht_create(uint32_t size, bool mtf) {
  HashTable *ht = (HashTable *)malloc(sizeof(HashTable));
  if (ht != NULL) {
    ht->mtf = mtf;
    ht->salt = 0x9846e4f157fe8840;
    ht->n_hits = ht->n_misses = ht->n_examined = ht->n_keys = ht->count = 0;
    ht->size = size;
    ht->lists = (LinkedList **)calloc(size, sizeof(LinkedList *));
    if (!ht->lists) {
      free(ht);
      ht = NULL;
    }
  }
  return ht;
}

void ht_delete(HashTable **ht) {
  for (uint32_t i = 0; i < (*ht)->size; i += 1) {
    if (((*ht)->lists)[i] != NULL) {
      ll_delete(&(
          ((*ht)->lists)[i])); // this will not work, we need to keep track of
                               // what linked lists are initialized to properly
                               // handle this theoretically I could make this
                               // work by letting the linked list destructor
                               // take a NULL pointer as an input, and then just
                               // have it immediately return if thats the case
    }
  }
  free((*ht)->lists);
  free(*ht);
  *ht = NULL;
}

uint32_t ht_size(HashTable *ht) { return ht->size; }

Node *ht_lookup(HashTable *ht, char *oldspeak) {
  // stat tracking setup
  uint32_t seek_var = 0;
  uint32_t links_initial = 0;
  uint32_t links_final = 0;
  ll_stats(&seek_var, &links_initial);

  // functionality setep
  uint64_t index = hash(ht->salt, oldspeak);
  index = index % ht->size;
  LinkedList *correct_ll = ht->lists[index];
  Node *node = ll_lookup(correct_ll, oldspeak);
  ll_stats(&seek_var, &links_final);
  ht->n_examined += (links_final - links_initial);
  if (node) {
    ht->n_hits += 1;
  } else {
    ht->n_misses += 1;
  }
  return node;
}

void ht_insert(HashTable *ht, char *oldspeak, char *newspeak) {
  uint64_t index = hash(ht->salt, oldspeak);
  index = index % ht->size;
  if (ht->lists[index] ==
      NULL) { // This is supposed to check if the list at the index index has
              // been initialized, and if not we can initialize it. I am not
              // confident this works
    ht->lists[index] = ll_create(ht->mtf);
    ll_insert(ht->lists[index], oldspeak, newspeak);
    ht->count += 1;
    ht->n_keys += 1;
  } else {
    if (ll_lookup(ht->lists[index], oldspeak) ==
        NULL) { // dont know why this isnt checking for repeats of insertions of
                // words in ht_keys
      ht->n_keys += 1;
    }
    ll_insert(ht->lists[index], oldspeak, newspeak);
  }
}

uint32_t ht_count(HashTable *ht) { return ht->count; }

void ht_print(HashTable *ht) {
  printf("mtf: %s\n", ht->mtf ? "True" : "False");
  printf("salt: %lu\nsize: %" PRIu32 "\ncount: %" PRIu32
         "\nnumber of keys: %" PRIu32 "\nnumber of hits: %" PRIu32
         "\nnumber of misses: %" PRIu32 "\nnumber examined: %" PRIu32 "\n",
         ht->salt, ht->size, ht->count, ht->n_keys, ht->n_hits, ht->n_misses,
         ht->n_examined);

  // I can't print the linked lists inside the ht. I cannot do that because i do
  // not have direct reference to any of the linked lists, I need an oldspeak to
  // get the hash to produce the index to know where there are valid locations
  // that linked lists are. Otherwise this is EXTREMELY inefficient and slow
}

void ht_stats(HashTable *ht, uint32_t *nk, uint32_t *nh, uint32_t *nm,
              uint32_t *ne) {
  *nk = ht->n_keys;
  *nh = ht->n_hits;
  *nm = ht->n_misses;
  *ne = ht->n_examined;
}
