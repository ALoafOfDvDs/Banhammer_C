//clang-format off
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "bf.h"
#include "bv.h"
#include "city.h"
#include "ht.h"
#include "ll.h"
#include "messages.h"
#include "node.h"
#include "parser.h"
//clang-format on

#define OPTIONS "ht:f:ms"
static char *str_dup(char *word) {
  char *copy;
  int i = 0;
  while (word[i] != '\0') {
    i += 1;
  }
  i += 1; // for the null terminator
  copy = (char *)malloc(i);

  int j = 0;
  while ((copy[j] = word[j]) != '\0') {
    j += 1;
  }
  return copy;
}
static char *str_dup_for_fgets(char *word) {
  char *copy;
  int i = 0;
  while (word[i] != '\n') {
    i += 1;
  }
  i += 1; // for the null terminator
  copy = (char *)malloc(i);

  int j = 0;
  while ((copy[j] = word[j]) != '\n') {
    j += 1;
  }
  copy[j] = '\0';
  return copy;
}
static char *build_translation(char *word, char *translation) {
  char *copy;
  int i = 0;
  while (word[i] != '\0') {
    i += 1;
  }
  i += 1; // for the space
  i += 2; // for the arrow
  i += 1; // for the next space

  int s = 0;
  while (translation[s] != '\0') {
    s += 1;
    i += 1;
  }
  i += 1; // for the null terminator

  copy = (char *)malloc(i);

  int j = 0;
  while ((copy[j] = word[j]) != '\0') {
    j += 1;
  }

  copy[j] = ' ';
  j += 1;
  copy[j] = '-';
  j += 1;
  copy[j] = '>';
  j += 1;
  copy[j] = ' ';
  j += 1;
  int k = 0;
  while ((copy[j] = translation[k]) != '\0' &&
         (copy[j] = translation[k]) != '\n') {
    j += 1;
    k += 1;
  }

  return copy;
}

int my_strlen(char *word) {
  int i = 0;
  while (word[i] != '\0') {
    i += 1;
  }
  return i + 1; // adding 1 to account for null terminator
}

int main(int argc, char **argv) {
  bool stats = false;
  bool mtf = false;
  uint32_t bf_size_var = 524288;
  uint32_t ht_num_entries = 10000;
  int opt = 0;

  while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
    switch (opt) {
    case 'h':
      // print help messages
      fprintf(stderr, "%s%s%s%s%s", "Usage: ./banhammer [options]\n",
              "  ./banhammer will read in words from stdin, identify any "
              "badspeak or old speak and output an\n",
              "  appropriate punishment message. The badspeak and oldspeak "
              "(with the newspeak translation)\n",
              "  that caused the punishment will be printed after the message. "
              "If statistics are enabled\n",
              "  punishment messages are suppressed and only statistics will "
              "be printed.\n");

      fprintf(stderr, "%s\n%s\n%s\n%s\n%s\n",
              "    -t <ht_size>: Hash table size set to <ht_size>. (default: "
              "10000)",
              "    -f <bf_size>: Bloom filter size set to <bf_size>. (default "
              "2^19)",
              "    -s          : Enables the printing of statistics.",
              "    -m          : Enables move-to-front rule.",
              "    -h          : Display program synopsis and usage.");

      return 0;
    case 't':
      // hashtable num entries
      ht_num_entries = strtoul(optarg, NULL, 10);
      if (ht_num_entries <= 0) {
        printf("./banhammer: Invalid hash table size.\n");
        return 1;
      }
      // I MAY NEED TO CHECK FOR BOUNDS, USE BINARY TO CHECK
      break;
    case 'f':
      // bloom filter num entries
      bf_size_var = strtoul(optarg, NULL, 10);
      if (bf_size_var <= 0) {
        printf("./banhammer: Invalid bloom filter table size.\n");
        return 1;
      }
      break;
    case 'm':
      mtf = true;
      break;
    case 's':
      stats = true;
      break;
    }
  }

  BloomFilter *bf = bf_create(bf_size_var);
  HashTable *ht = ht_create(ht_num_entries, mtf);
  FILE *badspeak = fopen("badspeak.txt", "r");
  FILE *translations = fopen("newspeak.txt", "r");

  char *word = (char *)malloc(1001);
  char *word2; 

  while ((fgets(word, 1001, badspeak)) != NULL) {
    word2 = str_dup_for_fgets(word);
    bf_insert(bf, word2);
    ht_insert(ht, word2, NULL);
    free(word2);
  }

  while ((fgets(word, 1001, translations)) != NULL) {
    int str_len =
        my_strlen(word); // should return the number of bytes used to store
                         // returned word, and since every character is 1 byte,
                         // should return how many  chars are in returned word
    // this is the string length of the entire line, meaning first word + space
    int i = 0;
    while (i < str_len) {
      char curr = word[i];
      if (!((isalnum(curr)) || curr == '-' || curr == '\'')) {
        // we have found a non-alphanumerical character that is not a hyphen or
        // apostrophe and we need to break off the word
        char oldspeak_word[i + 1];
        for (int j = 0; j < i; j += 1) {
          oldspeak_word[j] = word[j];
        }
        oldspeak_word[i] = '\0';

        i += 1;

        char newspeak[str_len - i - 1]; 
        int i_copy = i;

        // since returned word is NULL terminated itself, I should not have to
        // add a NULL terminator to this
        for (int j = 0; j < str_len - i_copy - 1; j += 1) {
          newspeak[j] = word[i];
          i += 1;
        }
        newspeak[str_len - i_copy - 2] = '\0';
        ht_insert(ht, oldspeak_word, newspeak);
        bf_insert(bf, oldspeak_word);
      }
      i += 1;
    }
  }
  // This marks the end of getting all the words into the hashtable and
  // bloomfilter

  Parser *parser = parser_create(stdin);
  LinkedList *thoughtcrime_list = ll_create(false);
  LinkedList *translations_required_list = ll_create(false);

  char *word_to_read = (char *)malloc(1001);
  for (int i = 0; i < 1000; i += 1) {
    word_to_read[i] = '-';
  }
  word_to_read[1000] = '\0';

  Node *node;

  while (next_word(parser, word_to_read)) { // get a new word and make sure that
                                            // we have a new word on the line
    if (bf_probe(bf,
                 word_to_read)) { // check if the word is in the bloom filter,
                                  // if so, we need to check the hashtable
      if ((node = ht_lookup(ht, word_to_read)) !=
          NULL) { // if the word IS in the hashtable then we do something
        if (node->newspeak ==
            NULL) { // the node exists, and does not have a translation
          word2 = str_dup(word_to_read);
          ll_insert(thoughtcrime_list, word2, NULL);
          free(word2);
        } else {
          word2 = build_translation(word_to_read, node->newspeak);
          ll_insert(translations_required_list, word_to_read, node->newspeak);
          free(word2);
        }
      }
    }
  }

  if (!stats) {
    if (ll_length(thoughtcrime_list) && ll_length(translations_required_list)) {
      printf("%s", mixspeak_message);
      ll_print(thoughtcrime_list);
      ll_print(translations_required_list);
    } else if (ll_length(thoughtcrime_list) &&
               !ll_length(translations_required_list)) {
      printf("%s", badspeak_message);
      ll_print(thoughtcrime_list);
    } else if (!ll_length(thoughtcrime_list) &&
               ll_length(translations_required_list)) {
      printf("%s", goodspeak_message);
      ll_print(translations_required_list);
    }
  }

  if (stats) {
    // ht stats
    uint32_t ht_keys, ht_hits, ht_misses, ht_examined;
    ht_stats(ht, &ht_keys, &ht_hits, &ht_misses, &ht_examined);
    uint32_t bf_keys, bf_hits, bf_misses, bf_examined;
    bf_stats(bf, &bf_keys, &bf_hits, &bf_misses, &bf_examined);
    printf("ht keys: %u\n", ht_keys);
    printf("ht hits: %u\n", ht_hits);
    printf("ht misses: %u\n", ht_misses);
    printf("ht probes: %u\n", ht_examined);
    printf("bf keys: %u\n", bf_keys);
    printf("bf hits: %u\n", bf_hits);
    printf("bf misses: %u\n", bf_misses);
    printf("bf bits examined: %u\n", bf_examined);
    if (bf_misses != 0) {
      float examined_per_miss =
          ((float)bf_examined - (N_HASHES * bf_hits)) / bf_misses;
      printf("Bits examined per miss: %1.6f\n", examined_per_miss);
    } else {
      printf("Bits examined per miss: %1.6f\n", 0.000000);
    }


    if (bf_hits != 0) {

      float false_positives = (float)ht_misses / bf_hits;
      printf("False positives: %1.6f\n", false_positives);
    } else {
      printf("False positives: %1.6f\n", 0.000000);
    }

    if ((ht_hits + ht_misses) != 0) {
      float seek_len = (float)ht_examined / (ht_hits + ht_misses);
      printf("Average seek length: %1.6f\n", seek_len);
    } else {
      printf("Average seek length: %1.6f\n", 0.000000);
    }

    uint32_t bloom_size = bf_size(bf);
    float bf_load = (float)bf_count(bf) / bloom_size;
    printf("Bloom filter load: %1.6f\n", bf_load);
  }

  ht_delete(&ht);
  bf_delete(&bf);
  ll_delete(&thoughtcrime_list);
  ll_delete(&translations_required_list);
  if (parser != NULL) {
    parser_delete(&parser);
  }
  free(word);

  free(word_to_read);

  fclose(badspeak);
  fclose(translations);
  fclose(stdin);

  return 0;
}
