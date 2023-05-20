#include "../include/mamalloc.h"
#include <stddef.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* MEMALLOC LINKEDLIST TEST

typedef struct node {
  int data;
  struct node* next;
} node;
node *head = NULL;
void insert(int data) {
  node *temp;
  temp = memalloc(sizeof(node));
  temp->data = data;
  temp->next = NULL;
  if(head == NULL) {
    head = temp;
  }
  else {
    node * current;
    current = head;
    while(current->next != NULL) {
      current = current->next;
    }
    current->next = temp;
  }
}
void print_linked() {
  node* current = head;
  while(current->next != NULL) {
    printf("%d --> ", current->data);
    current = current->next;
  }
  printf("NULL");
}



int main()
{
  int val = 0;
  while(val != -1) {
    printf("data: ");
    scanf("%d", &val);
    insert(val);
  }
  print_linked();
  return 0;
}
*/

int main(int argc, char** argv)
{

  return 0;
};
