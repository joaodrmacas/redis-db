#include "structures.hpp"


/*For the hashtable I opted to use powers of 2 as size since it will allow
me to perform remainder operations using a mask (size - 1) where all the bits are
set to 1. This allow to perform fast remainder operations using bitwise AND for hashing

USING INTRUSIVE DATA STRUCTURES
*/

// Size must be a power of two to be able to growth
int hashtable_init(HashTable *ht, uint32_t size){

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
void hashtable_insert(HashTable *ht, Node *node){

    size_t index = node->hash_value & ht->mask;

    //Get the first node of the list
    Node *head = ht->table[index];

    //Insert at the head of the list
    node->next = head;
    ht->table[index] = node;
    ht->cur_size++;

}

//Lookup a node in the hashtable using a custom equality function and returns a pointer to a pointer to the node for deletion
Node** hashtable_lookup(HashTable *ht, Node* key, bool (*eq) (Node*, Node*)){
    if (!ht->table){
        printf("hashtable_lookup: hashtable not initialized\n");
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
Node* hashtable_detach(Hashtable *ht, Node** from){

    Node *to_delete = *from;
    *from = to_delete->next;
    ht->cur_size--;

    return to_delete;
}
