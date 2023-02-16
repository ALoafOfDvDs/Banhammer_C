//clang-format off
#include "node.h"
#include <stdio.h>
#include <stdlib.h>

//clang-format on

static char *my_strdup(char *word) {
  char *copy;
  int i = 0;
  while (word[i] != '\0') {
    i += 1;
  }
  i += 1; // for the null terminator
  copy = (char *)calloc(i, sizeof(char));

  int j = 0;
  while ((copy[j] = word[j]) != '\0') {
    j += 1;
  }
  return copy;
}

Node *node_create(char *oldspeak, char *newspeak) {
  Node *t = (Node *)malloc(sizeof(Node));
  if (t) {
    if (oldspeak != NULL) {
      t->oldspeak = my_strdup(oldspeak); // NOT ALLOWED TO USE STRDUP
      t->newspeak = NULL;
    } else {
      t->oldspeak = NULL;
      t->newspeak = NULL;
      // we probably should never have a situation where oldspeak is NULL
      //  printf("passing oldspeak as NULL, probably an issue here");
      //  THIS WILL HAPPEN TWICE, WE HAVE TO PASS BOTH AS NULL FOR THE 2
      //  SENTINEL NODES
    }
    if (newspeak != NULL) {
      t->newspeak = my_strdup(newspeak); // NOT ALLOWED TO USE STRDUP
    }
    t->next = NULL;
    t->prev = NULL;
  }
  return t;
}

void node_delete(Node **n) {
  if ((*n)->oldspeak != NULL) {
    free((*n)->oldspeak);
  }
  if ((*n)->newspeak != NULL) {
    free((*n)->newspeak);
  }
  free(*n);
  *n = NULL;
  return;
}

void node_print(Node *n) {
  if (n->newspeak != NULL && n->oldspeak != NULL) {
    printf("%s -> %s\n", n->oldspeak, n->newspeak);
  } else if (n->oldspeak != NULL) {
    printf("%s", n->oldspeak);
    printf("\n");
  }
}
