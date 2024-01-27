#include "structures.hpp"


/*For the hashtable I opted to use powers of 2 as size since it will allow
me to perform remainder operations using a mask (size - 1) where all the bits are
set to 1. This allow to perform fast remainder operations using bitwise AND for hashing

USING INTRUSIVE DATA STRUCTURES
*/

// Size must be a power of two to be able to growth
static int hashtable_init(HashTable *ht, uint32_t size){

    //Check for negative numbers and for non power of two
    if (size<=0 || (size & (size-1))){
        printf("Invalid size\n");
        return -1;
    }

    ht->table = (Node**) calloc(sizeof(Node*),size);
    ht->cur_size = 0;
    ht->mask = size - 1;

    return 0;
}

//Insert at the head of the list
static void hashtable_insert(HashTable *ht, Node *node){

    size_t index = node->hash_value & ht->mask;

    //Get the first node of the list
    Node *head = ht->table[index];

    //Insert at the head of the list
    node->next = head;
    ht->table[index] = node;
    ht->cur_size++;

}

//Lookup a node in the hashtable using a custom equality function and returns a pointer to a pointer to the node for deletion
static Node** hashtable_lookup(HashTable *ht, Node* key, bool (*eq) (Node*, Node*)){
    if (!ht->table){
        return NULL;
    }

    size_t index = key->hash_value & ht->mask;
    Node** head = &ht->table[index];

    for (Node *cur; (cur= *head) != NULL ; head = &cur->next){
        if (cur->hash_value == key->hash_value && eq(cur,key)){
            return head;
        }
    }

    return NULL;
}

//Since we pass a pointer to a pointer to the node we can just update the pointer to the next node, since the address remains the same 
//for the pointer that was removed.
static Node* hashtable_detach(HashTable *ht, Node** from){

    Node *to_delete = *from;
    *from = to_delete->next;
    ht->cur_size--;

    return to_delete;
}

static void hashmap_resize(HashMap *hm){

    if (hm->t2.table){
        printf("hashmap_resize: unexpected behaviour, t2 is already initialized\n");
        return;
    }

    hm->t2 = hm->t1;
    hashtable_init(&hm->t1, (hm->t2.mask+1)*2);
    hm->resizing_index = 0;

}

static void hashmap_populate_new_table(HashMap *hm){

    size_t nwork = 0;
    while (nwork < RESIZING_WORK && hm->t2.cur_size > 0){

        //Get nodes from the old table and move them to the newer table
        Node** from = &hm->t2.table[hm->resizing_index];
        if (!*from){
            hm->resizing_index++;
            continue;
        }

        hashtable_insert(&hm->t1, hashtable_detach(&hm->t2, from));
        nwork++;
    }

    if (hm->t2.cur_size == 0){
        free(hm->t2.table);
        hm->t2.table = NULL;
    }
}

void hashmap_insert(HashMap *hm, Node *node){

    printf("Inserting node | DB_Size: %ld | DB_Max_Size %ld\n", hm->t1.cur_size, (hm->t1.mask+1));

    if (!hm->t1.table){
        printf("Initializing db\n");
        hashtable_init(&hm->t1, HT_INIT_SIZE);
    }
    hashtable_insert(&hm->t1, node);

    //Check if we need to resize
    if (!hm->t2.table){
        //mask+1 is the whole size of the table
        size_t load_factor = hm->t1.cur_size / (hm->t1.mask+1);
        if (load_factor >= MAX_LOAD_FACTOR){
            hashmap_resize(hm);
        }
    }

    hashmap_populate_new_table(hm);

}

Node* hashmap_lookup(HashMap *hm, Node *key, bool (*eq) (Node*, Node*)){

    printf("Looking up node | DB_Size: %ld | DB_Max_Size %ld\n", hm->t1.cur_size, (hm->t1.mask+1));

    if (!hm->t1.table){
        printf("hashmap_lookup: hashmap is empty\n");
        return NULL;
    }
    hashmap_populate_new_table(hm);
    Node **from = NULL;
    from = hashtable_lookup(&hm->t1, key, eq);
    if (!from) from = hashtable_lookup(&hm->t2, key, eq);
    
    if (!from) return NULL;

    return *from;
}

Node* hashmap_delete(HashMap *hm, Node *key, bool (*eq) (Node*, Node*)){

    printf("Deleting node | DB_Size: %ld | DB_Max_Size %ld\n", hm->t1.cur_size, (hm->t1.mask+1));

    if (!hm->t1.table){
        printf("hashmap_delete: hashmap not initialized\n");
        return NULL;
    }

    hashmap_populate_new_table(hm);

    Node **from = NULL;
    from = hashtable_lookup(&hm->t1, key, eq);
    if (from) 
        return hashtable_detach(&hm->t1, from);
    
    from = hashtable_lookup(&hm->t2, key, eq);
    if (from) 
        return hashtable_detach(&hm->t2, from);

    printf("hashmap_delete: key not found\n");
    return NULL;

}

static void hashtable_scan(HashTable *ht, void (*func) (Node* , void* ), void *out){
    if (ht->cur_size == 0)
        return;
    
    for (size_t t=0; t<ht->mask+1;t++){
        Node *cur = ht->table[t];
        while(cur){
            func(cur,out);
            cur = cur->next;
        }
    }
}

void hashmap_scan(HashMap *hm, void (*func) (Node* , void* ), void *out){

    if (!hm->t1.table){
        printf("hashmap_scan: hashmap not initialized\n");
        return;
    }

    hashtable_scan(&hm->t1, func, out);
    hashtable_scan(&hm->t2, func, out);
}

void hashmap_destroy(HashMap *hm){

    if (hm->t1.table){
        free(hm->t1.table);
        hm->t1.table = NULL;
    }

    if (hm->t2.table){
        free(hm->t2.table);
        hm->t2.table = NULL;
    }

    *hm = HashMap{};
}

uint32_t hashmap_total_size(HashMap *hm){
    return hm->t1.cur_size + hm->t2.cur_size;
}
