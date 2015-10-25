#include <htable.h>

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>

static const int uproc_htable_primes[] = { 29, 53, 97, 193, 389, 769, 1543, 3079, 6151,
                                           12289, 24593, 49157, 98317, 196613, 393241,
                                           786433, 1572869, 3145739, 3145739, 12582917,
                                           25165843, 50331653, 100663319, 201326611, 
                                           402653189, 805306457, 1610612741
                                          };

static const int uproc_htable_nprimes = sizeof(uproc_htable_primes) / sizeof(int);

/*
* Expand the size of the hash table to @size.
* @ht: the hash table to expand
* @size: the size we expand to
*/
static int uproc_htable_expand(uproc_htable_t *ht, int size){
    int new_len, new_idx, new_load_limit,  i;
    struct hlist_head *new_buckets, *head;
    struct hlist_node *p, *q;
    unsigned h;
    new_load_limit = ht->load_limit;
    new_len = ht->len;
    new_idx = ht->p_index;
    while(new_load_limit < size && new_idx < uproc_htable_nprimes){
        new_len = uproc_htable_primes[++new_idx];
        new_load_limit = ht->load_factor * new_len;
    }

    if((new_buckets = malloc(new_len * sizeof(struct hlist_head))) == NULL){
        fprintf(stderr, "failed to malloc: %s", strerror(errno));
        return -ENOMEM;
    }

    for(i = 0; i < new_len; ++i){
        INIT_HLIST_HEAD(&new_buckets[i]);
    }

    /*
    * Rehash and move all event to new_buckets.
    */
    for(i = 0; i < ht->len; ++i){
        head = &(ht->buckets[i]);
        if(!hlist_empty(head)){
            p = head->first;
            while(p){
                q = p->next;
                hlist_del(p);
                h = ht->hf(p) % new_len;
                hlist_add_head(&new_buckets[h], p);
                p = q;
            }
        }
    }

    free(ht->buckets);

    ht->p_index = new_idx;
    ht->buckets = new_buckets;
    ht->len = new_len;
    ht->load_limit = new_load_limit;

    return 0;
}

inline int uproc_htable_init(uproc_htable_t *ht, double load_factor, hash_t hf, entry_equal_t heef){
    int i;
    
    ht->len = uproc_htable_primes[0];
    ht->buckets = malloc(ht->len * sizeof(struct hlist_head));

    if(ht->buckets == NULL){
        fprintf(stderr, "can't allocate hash buckets, memory shortage.");
        return -ENOMEM;
    }

    for(i = 0; i < ht->len; ++i){
        INIT_HLIST_HEAD(&ht->buckets[i]);
    }

    ht->p_index = 0;
    ht->load_limit = load_factor * uproc_htable_primes[0];
    ht->load_factor = load_factor;
    ht->n_entries = 0;
    
    ht->hf = hf;
    ht->heef = heef;
    return 0;
}

int uproc_htable_insert(uproc_htable_t *ht, struct hlist_node *new, unsigned key,
                        void *d1, void *d2, void *d3){
    unsigned h;
    struct hlist_node *p;
    if(new->pprev || new->next){
        /*
        * This event is already in the hash table.
        */
        return -EEXIST;
    }

    /* expand the hash table if nessesary */
    if(ht->n_entries >= ht->load_limit)
        uproc_htable_expand(ht, ht->n_entries + 1);

    h =  key % ht->len;

    hlist_for_each(p, &ht->buckets[h]){
        if(ht->heef(p, d1, d2, d3)){
            return -EEXIST;
        }
    }

    hlist_add_head(&ht->buckets[h], new);
    ++ht->n_entries;
    return 0;
}

void uproc_htable_delete_by_key(uproc_htable_t *ht, unsigned key,
                                void *d1, void *d2, void *d3){
    struct hlist_node *p;
    unsigned h = key % ht->len;

    hlist_for_each(p, &ht->buckets[h]){
        if(ht->heef(p, d1, d2, d3)){
            hlist_del(p);
            --ht->n_entries;
            return;
        }
    }
}

int uproc_htable_delete(uproc_htable_t *ht, struct hlist_node *entry){

    if(entry->pprev == NULL || entry->next == NULL){
        /*
        * This event is not in the hash table.
        */
        return -EINVAL;
    }

    hlist_del(entry);
    --ht->n_entries;
    return 0;
}

struct hlist_node* uproc_htable_find(uproc_htable_t * ht, unsigned key, 
                                     void *d1, void *d2, void *d3){
    unsigned h;
    struct hlist_node *p;

    h =  key % ht->len;

    hlist_for_each(p, &ht->buckets[h]){
        if(ht->heef(p, d1, d2, d3)){
            return p;
        }
    }

    return NULL;
}

inline void uproc_htable_free(uproc_htable_t * ht){
    free(ht->buckets);
    memset(ht, 0, sizeof(uproc_htable_t));
}