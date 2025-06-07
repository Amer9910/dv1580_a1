#include <stdio.h>
#include "linked_list.h"
#include "memory_manager.h"

void list_init(Node** head, size_t size) {
    *head = NULL;
    mem_init(size);
}

void list_insert(Node** head, uint16_t data) {
    Node* new_node = (Node*)mem_alloc(sizeof(Node));
    if (new_node == NULL) {
        printf("ERROR: Failed to allocate node for value %u\n", data);
        return;
    }

    new_node->data = data;
    new_node->next = NULL;

    if (*head == NULL) {
        *head = new_node;
    } else {
        Node* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}


void list_insert_after(Node* prev_node, uint16_t data) {
    if (prev_node == NULL) {
        printf("Error: Provided previous node is NULL.\n");
        return;
    }

    Node* new_node = (Node*)mem_alloc(sizeof(Node));
    if (new_node == NULL) {
        printf("Error: Memory allocation failed.\n");
        return;
    }

    new_node->data = data;
    new_node->next = prev_node->next;
    prev_node->next = new_node;
}


void list_insert_before(Node** head, Node* next_node, uint16_t data) {
    if (head == NULL || *head == NULL || next_node == NULL) {
        printf("Error: Invalid input.\n");
        return;
    }

    Node* new_node = (Node*)mem_alloc(sizeof(Node));
    if (new_node == NULL) {
        printf("Error: Allocation failed.\n");
        return;
    }

    new_node->data = data;

    if (*head == next_node) {
        new_node->next = *head;
        *head = new_node;
        return;
    }

    Node* current_node = *head;
    while (current_node != NULL && current_node->next != next_node) {
        current_node = current_node->next;
    }

    if (current_node == NULL) {
        printf("Error: Target node not found.\n");
        mem_free(new_node);
        return;
    }

    new_node->next = next_node;
    current_node->next = new_node;
}


void list_delete(Node** head, uint16_t data) {
    if (head == NULL) return;
    if (*head == NULL) return;

    Node* current = *head;
    Node* prev = NULL;

    for (; current != NULL; current = current->next) {
        if (current->data == data) break;
        prev = current;
    }

    if (current == NULL) return;

    if (prev == NULL) {
        *head = current->next;
    } else {
        prev->next = current->next;
    }

    mem_free(current);
}


Node* list_search(Node** head, uint16_t data) {
    if (head == NULL || *head == NULL) return NULL;

    Node* current = *head;

    for (; current != NULL; current = current->next) {
        if (current->data == data) {
            return current;
        }
    }

    return NULL;
}


void list_display(Node** head) {
    if (head == NULL || *head == NULL) {
        printf("[]");
        return;
    }

    printf("[");

    Node* current = *head;
    while (current != NULL) {
        printf("%u", current->data);
        if (current->next != NULL) {
            printf(", ");
        }
        current = current->next;
    }

    printf("]");
}

void list_display_range(Node** head, Node* start_node, Node* end_node) {
    Node* current = (start_node != NULL) ? start_node : (head ? *head : NULL);

    printf("[");
    int is_first = 1;

    while (current != NULL) {
        if (!is_first) {
            printf(", ");
        }

        printf("%u", current->data);

        if (current == end_node) break;

        is_first = 0;
        current = current->next;
    }

    printf("]");
}

int list_count_nodes(Node** head) {
    int count = 0;

    if (head == NULL || *head == NULL) return 0;

    for (Node* current = *head; current != NULL; current = current->next) {
        count++;
    }

    return count;
}



void list_cleanup(Node** head) {
    if (head == NULL || *head == NULL) return;

    Node* walker = *head;

    while (walker != NULL) {
        Node* temp = walker->next;
        mem_free(walker);
        walker = temp;
    }

    *head = NULL;
    mem_deinit();
}
