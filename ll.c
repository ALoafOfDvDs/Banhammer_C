//clang-format off
#include "ll.h"
#include <stdio.h>
#include <stdlib.h>

//clang-format on

uint64_t seeks; // Number of seeks performed.
uint64_t links; // Number of links traversed.

struct LinkedList {
  uint32_t length;
  Node *tail;
  Node *head;
  bool mtf;
};

LinkedList *ll_create(bool mtf) {
  LinkedList *ll = (LinkedList *)malloc(sizeof(LinkedList));
  ll->mtf = mtf;
  Node *head = node_create(NULL, NULL);
  Node *tail = node_create(NULL, NULL);
  head->next = tail;
  tail->prev = head;
  ll->tail = tail;
  ll->head = head;
  ll->length = 0;
  return ll;
}

void ll_delete(LinkedList **ll) {
  Node *head = (*ll)->head;
  while (head != NULL) {
    Node *next = NULL;
    next = head->next;
    node_delete(&head);
    head = next;
  }
  free(*ll);
  *ll = NULL;
}

uint32_t ll_length(LinkedList *ll) { return ll->length; }

int my_strcmp(char *word_1, char *word_2) {
  if (word_1 == NULL || word_2 == NULL) {
    if (word_1 == NULL && word_2 == NULL) {
      printf("both words to compare are NULL, might be an error\n");
      return 0;
    } else {
      printf("one word is null\n");
      return 1;
    }
  }
  int i = 0;
  while (word_1[i] != '\0' && word_2[i] != '\0') {
    if (word_1[i] != word_2[i]) {
      return 1;
    }
    i += 1;
  }
  if (word_1[i] !=
      word_2[i]) { // to make sure that both are on the null char. Could also
                   // just comp strlen, but thats extra room for error
    return 1;
  }
  return 0;
}

Node *ll_lookup(LinkedList *ll, char *oldspeak) {
  // NEED TO MAKE SURE TO UPDATE STATS
  if (ll == NULL) {
    return NULL;
  }
  seeks += 1;
  for (Node *curr = ll->head->next; curr->next != NULL; curr = curr->next) {
    links += 1;

    if (my_strcmp(curr->oldspeak, oldspeak) == 0) {
      if (ll->mtf) {
        // MOVE TO FRONT HERE
        // curr points to the current node, and that is the node that needs to
        // be moved to the front

        Node *next = curr->next;
        Node *prev = curr->prev;
        next->prev = prev;
        prev->next = next;

        Node *front = ll->head->next;
        curr->next = front;
        curr->prev = ll->head;
        front->prev = curr;

        ll->head->next =
            curr;
      }
      return curr;
    }
  }
  return NULL;
}

void ll_insert(LinkedList *ll, char *oldspeak, char *newspeak) {
  Node *existing_node = ll_lookup(ll, oldspeak);
  if (existing_node == NULL) {
    ll->length += 1;
    Node *new_node = node_create(oldspeak, newspeak);
    new_node->next = ll->head->next;
    new_node->prev = ll->head;
    ll->head->next->prev = new_node;
    ll->head->next = new_node;
  }
}

void ll_print(LinkedList *ll) {
  for (Node *curr = ll->head->next; curr->next != NULL; curr = curr->next) {
    node_print(curr);
  }
}

void ll_stats(uint32_t *n_seeks, uint32_t *n_links) {
  *n_seeks = seeks;
  *n_links = links;
}
