#ifndef _UPROC_LIST_H
#define _UPROC_LIST_H
#include <stddef.h>

#ifdef __cplusplus
extern "C"{
#endif

struct list_head{
    struct list_head *prev, *next;
};

/* prefetch for read */
static inline void prefetch(const void *ptr)  
{ 
    __builtin_prefetch(ptr, 0, 3);
}
/* prefetch for write */
static inline void prefetchw(const void *ptr)  
{ 
    __builtin_prefetch(ptr, 1, 3);
}

#define container_of(ptr, type, member) (((type*)((char*)ptr - (offsetof(type, member)))))

struct hlist_node;
/* dummy bucket node */
struct hlist_head {
    struct hlist_node *first;
};

struct hlist_node {
    struct hlist_node *next, **pprev;
};

#define HLIST_HEAD_INIT(name) { .fist = NULL }

#define HLIST_HEAD(name) \
    struct hlist_head name = LIST_HEAD_INIT(name)

#define INIT_HLIST_HEAD(ptr) ((ptr)->first = NULL)

static inline void INIT_HLIST_NODE(struct hlist_node *h)
{
    h->next = NULL;
    h->pprev = NULL;
}

static inline int hlist_empty(struct hlist_head *h) {
    return h->first == NULL;
}

static inline void __hlist_del(struct hlist_node *n) {
    struct hlist_node *next = n->next;
    struct hlist_node **pprev = n->pprev;
    *pprev = next;
    if (next)
        next->pprev = pprev;
}

static inline void hlist_del(struct hlist_node *n) {
    __hlist_del(n);
    n->next = NULL;
    n->pprev = NULL;
}

static inline void hlist_add_head(struct hlist_head *h, struct hlist_node *n) {
    struct hlist_node *first = h->first;
    n->next = first;
    if (first)
        first->pprev = &n->next;
    h->first = n;
    n->pprev = &h->first;
}

#define hlist_entry(ptr, type, member) container_of(ptr,type,member)

#define hlist_for_each(pos, head) \
    for (pos = (head)->first; pos && ({ prefetch(pos->next); 1; }); \
         pos = pos->next)

/**
 * hlist_for_each_entry - iterate over list of given type
 * @tpos:   the type * to use as a loop cursor.
 * @pos:    the &struct hlist_node to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry(tpos, pos, head, member)            \
    for (pos = (head)->first;                    \
         pos && ({ prefetch(pos->next); 1;}) &&          \
        ({ tpos = hlist_entry(pos, typeof(*tpos), member); 1;}); \
         pos = pos->next)

#ifdef __cplusplus
}
#endif
#endif /*_UPROC_LIST_H_*/