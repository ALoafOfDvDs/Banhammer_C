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

// void ll_delete(LinkedList **ll) {
//     LinkedList * list = *ll;
//     Node * ptr_to_next;
//     // so lets think about this. Our linked list constructor allocates 2
//     nodes immediately, those being the sentinel nodes
//     // that means we need to make sure we call the node destructor on those
//     nodes too while(list->length > 0) {
//         Node * element = list->head->next;
//         // printf("length left: %u\n", list->length);
//         ptr_to_next = element->next;

//         list->head->next = ptr_to_next;
//         // list->head->next = element->next;
//         node_delete(&element);
//         list->length -= 1;
//     }
//     node_delete(&(list->head));
//     node_delete(&(list->tail));
//     free(*ll);
//     *ll = NULL;

// }
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
  // printf("ll_lookup is unfinished\n");
  // ll_print(ll);
  if (ll == NULL) {
    return NULL;
  }
  seeks += 1;
  // printf("dropping into for loop\n");
  for (Node *curr = ll->head->next; curr->next != NULL; curr = curr->next) {
    // printf("got inside for loop\n");
    // node_print(curr);
    // printf("inside ll_lookup for loop\n");

    // if(curr == NULL) {
    //     printf("its null\n");
    // }
    // else {
    //     printf("%s\n",curr->oldspeak);
    // }
    links += 1;

    if (my_strcmp(curr->oldspeak, oldspeak) == 0) {
      // printf("got here\n");
      if (ll->mtf) {
        // MOVE TO FRONT HERE
        // printf("got here 2\n");
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

        // next->prev = prev;
        // prev->next = next;
        ll->head->next =
            curr; // somehow this is causing me to have an infinite loop
      }
      // printf("%s\n",curr->oldspeak);
      // printf("got here 3\n");
      return curr;
    }
    // printf("looping here infinitely\n");
    // node_print(curr->next);
  }
  // printf("returning null\n");
  return NULL;
}

void ll_insert(LinkedList *ll, char *oldspeak, char *newspeak) {
  Node *existing_node = ll_lookup(ll, oldspeak);
  if (existing_node != NULL) {
    // printf("printing node: ");
    // node_print(existing_node);
    // printf("oldspeak word: %s\n", oldspeak);
  }
  // printf("existing node: %p\n", (void *)existing_node);
  if (existing_node == NULL) {
    ll->length += 1;
    // printf("created the node for %s\n", oldspeak);
    Node *new_node = node_create(oldspeak, newspeak);
    new_node->next = ll->head->next;
    new_node->prev = ll->head;
    // Node * ptr_to_first_node = ll->head->next;
    ll->head->next->prev = new_node;
    ll->head->next = new_node;
  }
  // ll_print(ll);
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
