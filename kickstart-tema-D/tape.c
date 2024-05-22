#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "tape.h"

struct _s_node {
    tape_elem elem;
    struct _s_node *next;
};

typedef struct _s_node * node_t;

struct _s_tape {
    unsigned int size;
    node_t cursor;  // Puntero al elemento actual
    node_t start;   // Puntero al primer nodo de la cinta
};

static bool invrep(tape_t tape) {
    // La invariante de representación asegura que la cinta siempre es válida
    bool is_valid = (tape != NULL);
    unsigned int count = 0;
    node_t node = tape->start;
    while (node != NULL) {
        count++;
        node = node->next;
    }
    return is_valid && (count == tape->size);
}

static node_t create_node(tape_elem e) {
    node_t new_node = malloc(sizeof(struct _s_node));
    new_node->elem = e;
    new_node->next = NULL;
    return new_node;
}

static node_t destroy_node(node_t node) {
    free(node);
    return NULL;
}

tape_t tape_create(void) {
    tape_t tape = malloc(sizeof(struct _s_tape));
    tape->size = 0;
    tape->cursor = NULL;
    tape->start = NULL;
    assert(invrep(tape) && tape_is_empty(tape) && tape_at_start(tape));
    return tape;
}

tape_t tape_rewind(tape_t tape) {
    assert(invrep(tape));
    tape->cursor = tape->start;
    assert(invrep(tape) && tape_at_start(tape));
    return tape;
}

bool tape_at_start(tape_t tape) {
    assert(invrep(tape));
    return tape->cursor == tape->start;
}

bool tape_at_stop(tape_t tape) {
    assert(invrep(tape));
    return tape->cursor == NULL;
}

bool tape_is_empty(tape_t tape) {
    assert(invrep(tape));
    return tape->size == 0;
}

unsigned int tape_length(tape_t tape) {
    assert(invrep(tape));
    return tape->size;
}

tape_t tape_step(tape_t tape) {
    assert(invrep(tape));
    if (tape->cursor != NULL) {
        tape->cursor = tape->cursor->next;
    }
    assert(invrep(tape));
    return tape;
}

tape_t tape_insertl(tape_t tape, tape_elem e) {
    assert(invrep(tape));
    node_t new_node = create_node(e);
    if (tape->cursor == tape->start) {
        new_node->next = tape->start;
        tape->start = new_node;
    } else {
        node_t prev = tape->start;
        while (prev->next != tape->cursor) {
            prev = prev->next;
        }
        new_node->next = tape->cursor;
        prev->next = new_node;
    }
    tape->cursor = new_node;
    tape->size++;
    assert(invrep(tape) && !tape_is_empty(tape) && !tape_at_stop(tape) && e == tape_read(tape));
    return tape;
}

tape_t tape_insertr(tape_t tape, tape_elem e) {
    assert(invrep(tape) && (!tape_at_stop(tape) || tape_is_empty(tape)));
    node_t new_node = create_node(e);
    if (tape->start != NULL) {
        new_node->next = tape->cursor->next;
        tape->cursor->next = new_node;
        tape->cursor = new_node;
    } else {
        tape->start = new_node;
        tape->cursor = new_node;
    }
    tape->size++;
    assert(invrep(tape) && !tape_is_empty(tape) && !tape_at_stop(tape) && e == tape_read(tape));
    return tape;
}

tape_t tape_erase(tape_t tape) {
    assert(invrep(tape) && !tape_is_empty(tape) && !tape_at_stop(tape));
    node_t to_remove = tape->cursor;
    if (tape->cursor == tape->start) {
        tape->start = tape->cursor->next;
        tape->cursor = tape->start;
    } else {
        node_t prev = tape->start;
        while (prev->next != tape->cursor) {
            prev = prev->next;
        }
        prev->next = tape->cursor->next;
        tape->cursor = prev->next;
    }
    to_remove = destroy_node(to_remove);
    tape->size--;
    assert(invrep(tape));
    return tape;
}

tape_elem tape_read(tape_t tape) {
    assert(invrep(tape) && !tape_is_empty(tape) && !tape_at_stop(tape));
    return tape->cursor->elem;
}

void tape_dump(tape_t tape) {
    assert(invrep(tape));
    node_t node = tape->start;
    printf("#");
    while (node != NULL) {
        if (node != tape->cursor) {
            printf("-%c-", node->elem);
        } else {
            printf("-[%c]-", node->elem);
        }
        node = node->next;
    }
    if (tape->cursor == NULL) {
        printf("-[]-");
    }
    printf("#\n");
}

tape_t tape_copy(tape_t tape) {
    assert(invrep(tape));
    tape_t copy = tape_create();
    node_t node = tape->start;
    node_t last = NULL;
    while (node != NULL) {
        node_t new_node = create_node(node->elem);
        if (copy->start == NULL) {
            copy->start = new_node;
        } else {
            last->next = new_node;
        }
        last = new_node;
        if (node == tape->cursor) {
            copy->cursor = new_node;
        }
        node = node->next;
    }
    copy->size = tape->size;
    assert(invrep(copy) && copy != tape && tape_at_start(copy) && tape_length(tape) == tape_length(copy));
    return copy;
}

tape_t tape_destroy(tape_t tape) {
    assert(invrep(tape));
    node_t node = tape->start;
    while (node != NULL) {
        node_t next = node->next;
        destroy_node(node);
        node = next;
    }
    free(tape);
    tape = NULL;
    assert(tape == NULL);
    return tape;
}
