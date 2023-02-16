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
// void strcpy(char *s, char *t) {
//     int i;

//     i=0;
//     while ((s[i] = t[i]) != '\0') {
//         i += 1;
//     }

// }

int my_strlen(char *word) {
  int i = 0;
  while (word[i] != '\0') {
    i += 1;
  }
  return i + 1; // adding 1 to account for null terminator
}

int main(int argc, char **argv) {
  // uint32_t counter_all_words_added = 0;
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
  // FILE * test = fopen("test.txt", "r");

  // int FLAG = 0;
  // unsure of how to implement reading in the values from badspeak.txt
  // for now we will import all words
  // NOTE THAT THIS WILL TAKE A HOT FUCKING MINUTE
  char *word = (char *)malloc(1001);
  char *word2; // = (char *)malloc(1001);
  // char * returned_word = (char *)malloc(1001);

  while ((fgets(word, 1001, badspeak)) != NULL) {
    // printf("word = %s\n", word);
    word2 = str_dup_for_fgets(word);
    bf_insert(bf, word2);
    // if (!strcmp(word2, "zwaggered")) { //USING STRING COMPARE HERE AS A TEST,
    // NEED TO REMOVE IT LATER
    //     FLAG = 1;
    // }
    ht_insert(ht, word2, NULL);
    free(word2);
    // counter_all_words_added += 1;
  }
  // now we will do the same for the newspeak translations
  // returned_word = "starting_word";

  // while ((/*returned_word = */fgets(word, 1001, badspeak)) != NULL) {
  //     printf("got here\n");
  //     int counter = 0;
  //     printf("returned word: %s   line: %d\n", word, counter);
  //     printf("check 1\n");
  //     int str_len = sizeof(word); // should return the number of bytes used
  //     to store returned word, and since every character is 1 byte, should
  //     return how many  chars are in returned word int i = 0; while (i <
  //     str_len) {
  //         char curr = word[i];
  //         if (!(isalnum(curr) || curr == '-' || curr == '\'')) {
  //             // we have found a non-alphanumerical character that is not a
  //             hyphen or apostrophe and we need to break off the word char
  //             oldspeak_word[i+1]; for (int j = 0; j < i; j += 1) {
  //                 oldspeak_word[j] = word[j];
  //             }
  //             oldspeak_word[i] = '\0';
  //             printf("oldspeak word: %s\n", oldspeak_word);

  //             // i += 1;

  //             // char newspeak[str_len + 1 - i]; //THIS MIGHT NOT BE ENOUGH
  //             // int i_copy = i;

  //             // // since returned word is NULL terminated itself, I should
  //             not have to add a NULL terminator to this
  //             // for (int j = 0; j < str_len - i_copy; j += 1) {
  //             //     newspeak[j] = returned_word[i];
  //             //     i += 1;
  //             // }
  //             // printf("newspeak word: %s\n", newspeak);

  //         }

  //         i += 1;
  //     }
  //     counter += 1;
  // }

  // printf("\n END OF BADSPEAK \n\n\n");
  // uint32_t num_words_added = 0;
  while ((fgets(word, 1001, translations)) != NULL) {
    // num_words_added+=1;
    // printf("returned word: %s\n", word);
    int str_len =
        my_strlen(word); // should return the number of bytes used to store
                         // returned word, and since every character is 1 byte,
                         // should return how many  chars are in returned word
    // this is the string length of the entire line, meaning first word + space
    // + second word + newline printf("%d\n", str_len);
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
        // printf("oldspeak word: %s\n", oldspeak_word);
        // counter_all_words_added += 1;
        i += 1;

        char newspeak[str_len - i - 1]; // THIS MIGHT NOT BE ENOUGH
        // printf("my string length: %d\n", str_len);
        // printf("i val: %d\n", i);

        // printf("newspeak length: %lu\n",strlen(newspeak));
        // printf("length it should be: %d\n", str_len - i - 1);
        int i_copy = i;

        // printf("len of newspeak: %d\n", str_len + 1 - i);
        // since returned word is NULL terminated itself, I should not have to
        // add a NULL terminator to this
        for (int j = 0; j < str_len - i_copy - 1; j += 1) {
          // printf("added char:%c\n", word[i]);
          newspeak[j] = word[i];
          i += 1;
          // printf("current char = %c\n", newspeak[j]);
          // printf("current letter: %c\n", word[i]);
        }
        // printf("current letter: %c\n", word[i]);
        newspeak[str_len - i_copy - 2] = '\0';
        // printf("%lu\n",strlen(newspeak));
        // printf("char =%c\n", newspeak[str_len-i_copy-1]);
        // printf("newspeak word: %s\n", newspeak);
        ht_insert(ht, oldspeak_word, newspeak);
        bf_insert(bf, oldspeak_word);
      }
      i += 1;
    }
  }
  // printf("\n\nEND OF TRANSLATIONS \n\n\n");
  // This marks the end of getting all the words into the hashtable and
  // bloomfilter

  // printf("all words added\n");

  // FILE *test = fopen("test.txt", "r");
  // printf("adar is at least recognized as a word %d\n", FLAG);
  Parser *parser = parser_create(stdin);
  // char ** thoughtcrime_list = NULL;
  LinkedList *thoughtcrime_list = ll_create(false);
  LinkedList *translations_required_list = ll_create(false);

  char *word_to_read = (char *)malloc(1001);
  for (int i = 0; i < 1000; i += 1) {
    word_to_read[i] = '-';
  }
  word_to_read[1000] = '\0';
  // uint32_t thoughtcrime_index = 0; // not only will this keep track of how
  // many words have been thoughtcrimes, but it will also tell me if there was a
  // thoughtcrime uint32_t trans_index = 0;// not only will this keep track of
  // how many words have been added to counsel, but it will also tell me if
  // counsel is needed
  Node *node;

  // while(fgets(parser->current_line, 1000, parser->f) != NULL) {
  while (next_word(parser, word_to_read)) { // get a new word and make sure that
                                            // we have a new word on the line
    // printf("word to read: %s\n", word_to_read);

    // printf("string comp value %d\n", strcmp(word_to_read, "adar"));

    // int i = my_strlen(word_to_read);
    // printf("len of word = %d\n", i);
    // char * working_word = (char *)malloc(i);
    // for (int j = 0; j < i; j += 1) {
    //     working_word[j] = word_to_read[j];
    // }
    // printf("new string comp value %d\n", strcmp(working_word, "adar"));

    // Node * checkInBadSpeak = ll_lookup(thoughtcrime_list, word_to_read);
    // Node * checkInTrans = ll_lookup(translations_required_list,
    // word_to_read); if (checkInBadSpeak || checkInTrans) {
    //     continue;
    // }

    if (bf_probe(bf,
                 word_to_read)) { // check if the word is in the bloom filter,
                                  // if so, we need to check the hashtable
      // printf("%s is in the bloomfilter\n", word_to_read);
      if ((node = ht_lookup(ht, word_to_read)) !=
          NULL) { // if the word IS in the hashtable then we do something
        // printf("%s is in the hashtable\n", word_to_read);
        if (node->newspeak ==
            NULL) { // the node exists, and does not have a translation
          // printf("got here with word: %s\n", word_to_read);
          // thoughtcrime_list[thoughtcrime_index] = str_dup(word_to_read);
          // thoughtcrime_index += 1;
          word2 = str_dup(word_to_read);
          ll_insert(thoughtcrime_list, word2, NULL);
          // ll_print(thoughtcrime_list);
          // printf("adding word to thoughtcrime list\n");
          // ll_print(thoughtcrime_list);
          free(word2);
        } else {
          word2 = build_translation(word_to_read, node->newspeak);
          ll_insert(translations_required_list, word_to_read, node->newspeak);
          // printf("adding word to translations list\n");
          // ll_print(translations_required_list);
          free(word2);
        }
      }
    }
  }
  // }

  // printf("\n\ngot through all words in stdin\n\n\n");

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
  // uint32_t counter_test = bf_count(bf);
  // printf("%u\n", counter_test);
  // uint32_t bloom_size = bf_size(bf);
  // float bf_load = (float)counter_test / bloom_size;
  // printf("Bloom filter load: %1.6f\n\n", bf_load);
  // printf("num words added (should be 14680): %u\n", counter_all_words_added);
  // bf_print(bf);
  if (stats) {
    // printf("stats\n");
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
    // float examined_per_miss = ((float)bf_examined -
    // (N_HASHES*bf_hits))/bf_misses; printf("Bits examined per miss: %1.6f\n",
    // examined_per_miss);

    if (bf_hits != 0) {
      // printf("hits%u\n",bf_hits);
      float false_positives = (float)ht_misses / bf_hits;
      printf("False positives: %1.6f\n", false_positives);
    } else {
      printf("False positives: %1.6f\n", 0.000000);
    }
    // float false_positives = (float)ht_misses / bf_hits;
    // printf("False positives: %1.6f\n", false_positives);

    if ((ht_hits + ht_misses) != 0) {
      float seek_len = (float)ht_examined / (ht_hits + ht_misses);
      printf("Average seek length: %1.6f\n", seek_len);
    } else {
      printf("Average seek length: %1.6f\n", 0.000000);
    }
    // float seek_len = (float)ht_examined / (ht_hits + ht_misses);
    // printf("Average seek length: %1.6f\n", seek_len);

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
