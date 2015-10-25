#ifndef _UPROC_HTABLE_H_
#define _UPROC_HTABLE_H_

#include "list.h"
#ifdef __cplusplus
extern "C"{
#endif

/* hash function type */
typedef unsigned (*hash_t)(struct hlist_node*);
typedef unsigned (*entry_equal_t)(struct hlist_node*, void *, void*, void*);
typedef struct uproc_htable uproc_htable_t;

struct uproc_htable {
    struct hlist_head *buckets;
    /* The index of uproc_htable_primes we are using as the size of the hash table */
    int p_index;
    /* We should expand the hash table if the threshold has been exceeded. */
    int load_limit;
    /* The load factor we apply to the hash table */
    double load_factor;
    /* the number of entries this hash table has */
    int n_entries;
    /* The number of slots this hash table has */
    int len;
    hash_t hf;
    entry_equal_t heef;
};

int uproc_htable_init(uproc_htable_t *ht, double load_factor, hash_t hf, entry_equal_t heqf);

int uproc_htable_insert(uproc_htable_t *ht, struct hlist_node *entry, 
                        unsigned key, void *d1, void *d2, void *d3);

void uproc_htable_delete_by_key(uproc_htable_t *ht, unsigned key, 
                                void *d1, void *d2, void *d3);

int uproc_htable_delete(uproc_htable_t *ht, struct hlist_node *entry);

struct hlist_node* uproc_htable_find(uproc_htable_t *ht, unsigned key,
                                     void *d1, void *d2, void*d3);

void uproc_htable_free(uproc_htable_t *ht);
#ifdef __cplusplus
}
#endif
#endif