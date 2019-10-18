#ifndef EXERCISE_I__GENERIC_LIST_H_
#define EXERCISE_I__GENERIC_LIST_H_

#include <stdbool.h>
#include "attributes.h"
#include "common.h"

typedef struct generic_list {
  struct generic_list *next, *prev;
} generic_list;

#define container_of(ptr, type, member) ({ \
    const typeof(((type*) 0)->member) *__member_ptr = (ptr); \
    (type*) ((char*) __member_ptr - offsetof(type, member)); \
}) \

#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

#define list_first_entry(ptr, type, member) \
    list_entry((ptr)->next, type, member)

#define list_next_entry(pos, member) \
    list_entry((pos)->member.next, typeof(*(pos)), member)

#define list_for_each(head, pos) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_entry(head, pos, member) \
    for (pos = list_entry((head)->next, typeof(*pos), member); \
         &pos->member != (head); \
         pos = list_entry(pos->member.next, typeof(*pos), member))

#define list_for_each_entry_safe(head, pos, n, member)            \
    for (pos = list_first_entry(head, typeof(*pos), member),    \
        n = list_next_entry(pos, member);            \
         &pos->member != (head);                    \
         pos = n, n = list_next_entry(n, member))

#define list_free(head, type, member, deallocate_func) \
{ \
    type *tmp = NULL; \
    type *it = NULL; \
    list_for_each_entry_safe(head, it, tmp, member) { \
        list_delete_entry(&it->member); \
        deallocate_func(it); \
    } \
}

#define LIST_HEAD_INIT(struct_name) { &(struct_name), &(struct_name) }

#define LIST_HEAD(var_name) \
    generic_list var_name = LIST_HEAD_INIT(var_name)

static __ALWAYS_INLINE
void INIT_LIST_HEAD(generic_list *list) {
  list->next = list;
  list->prev = list;
}

static __ALWAYS_INLINE
void __list_add(generic_list *new_node,
                generic_list *previous_node,
                generic_list *next_node) {
  next_node->prev = new_node;
  new_node->next = next_node;
  new_node->prev = previous_node;
  previous_node->next = new_node;
}

static __ALWAYS_INLINE
void __list_delete(generic_list *previous, generic_list *next) {
  next->prev = previous;
  previous->next = next;
}

static __ALWAYS_INLINE
bool list_is_empty(const generic_list *head) {
  return (uint8_t) (head->next == head);
}

static __ALWAYS_INLINE
void list_add(generic_list *head, generic_list *new_node) {
  __list_add(new_node, head, head->next);
}

static __ALWAYS_INLINE
void list_add_tail(generic_list *head, generic_list *new_node) {
  __list_add(new_node, head->prev, head);
}

static __ALWAYS_INLINE
void list_delete_entry(generic_list *entry) {
  __list_delete(entry->prev, entry->next);
}

#endif //EXERCISE_I__GENERIC_LIST_H_