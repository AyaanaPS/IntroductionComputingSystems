#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "multimap.h"

/* This indicates the size of our value array in the multimap_value structure */
#define ARRAY_SIZE 14

/* This defines the amount of keys that can be held in a node */
#define MIN_FACTOR 3

#define FULL 5

/*============================================================================
 * TYPES
 *
 *   These types are defined in the implementation file so that they can
 *   be kept hidden to code outside this source file.  This is not for any
 *   security reason, but rather just so we can enforce that our testing
 *   programs are generic and don't have any access to implementation details.
 *============================================================================*/

/* Represents a value array that is associated with a given key in the mm. */

typedef struct multimap_value {

    /* This represents the value array */
    int value[ARRAY_SIZE];

    /* This represents the current amount of values in the array */
    int currentSize;

    /* This points to the next value array for this key. */
    struct multimap_value *next;

} multimap_value;

/* Represents a key in the node */

typedef struct multimap_key {

    /* This holds the value of the key */
    int key;

    /* A linked list of the values associated with this key in the multimap. */
    multimap_value *values;

    /* The tail of the linked list of values, so that we can retain the order
     * they were added to the list.
     */
    multimap_value *values_tail;

    /* The left child of the key.  This will reference nodes that
     * hold keys that are strictly less than this key.
    */
    struct multimap_node *left_child;

    /* This holds the pointer to the next key in the node */
    struct multimap_key *next;

    /* This holds the poitner to the previous key in the node */
    struct multimap_key *prev;

} multimap_key;

/* Represents a node that has a linked list of keys, each with their 
 * own linked list of values. This also contains pointers to the next
 * child. There is only one child, because the node as a whole will
 * only point to a node with keys higher than this one.
 */

typedef struct multimap_node {

    /* A linked list of the keys associated with this node. */
    multimap_key *keys;

    /* The tail of the linked list of values, so that we can retain the order
     * they were added to the list.
     */
    multimap_key *keys_tail;

    /* This keeps track of the number of keys in this node */
    int num_keys;

    /* The right child of the multimap node. This will reference nodes that
     * hold keys that are strictly greater than this node's key.
     */
    struct multimap_node *node_child;

} multimap_node;

/* The entry point of the multimap data structure. */
struct multimap {

    /* This contains the root of the map. */
    multimap_node *root;
};


/*============================================================================
 * HELPER FUNCTION DECLARATIONS
 *
 *   Declarations of helper functions that are local to this module.  Again,
 *   these are not visible outside of this module.
 *============================================================================*/

multimap_node * alloc_mm_node();

multimap_key * alloc_mm_key();

multimap_key * search_tree(multimap_node *node, int key);

void splitChild(multimap_node *parent, multimap_node *child);

multimap_key * insertHelper(multimap_node *node, int key);

multimap_key * insertKey(multimap *mm, int key);

void free_multimap_values(multimap_value *values);

void free_multimap_keys(multimap_key *keys);

void free_multimap_node(multimap_node *node);

void print_keys_in_node(multimap_node *node);


/*============================================================================
 * FUNCTION IMPLEMENTATIONS
 *============================================================================*/

/* Allocates a multimap node, and zeros out its contents so that we know what
 * the initial value of everything will be.
 */
multimap_node * alloc_mm_node() 
{
    multimap_node *node = malloc(sizeof(multimap_node));
    bzero(node, sizeof(multimap_node));

    return node;
}

/* Allocates a multimap key, and zeros out its contents so that we know what
 * the initial value of everything will be.
 */
multimap_key * alloc_mm_key() 
{
    multimap_key *key = malloc(sizeof(multimap_key));
    bzero(key, sizeof(multimap_key));

    return key;
}

/* This searches through a tree looking for the specified key.
 * It returns the key if it exists and NULL if it does not exist.
 */

multimap_key * search_tree(multimap_node *node, int key)
{
    /* If the inputted node is NULL, return NULL because there is no key. */
    if(node == NULL)
    {
        return NULL;
    }

    /* Else, set the current key to the node->keys value, which is the
     * first key in node->keys.
     */
    multimap_key *current_key = node->keys;

    /* While the current key isn't null and while the key being looked for
     * is greater than the current key, continue updating the current key */
    while(current_key != NULL && key > current_key->key)
    {
        current_key = current_key->next;
    }

    /*
     * If the current key is NULL */
    if(current_key == NULL)
    {
        /* If the current node is also a leaf */
        if(node->node_child == NULL)
        {
            /* The key wasn't found, so return NULL. */
            return NULL;
        }

        /* Recursively search tree with the child of this node */
        return search_tree(node->node_child, key);
    }

    /* If the keys are equal */
    if(key == current_key->key)
    {
        /* Return the current key pointer */
        return current_key;
    }
    else
    {
        /* Else, check if the key has a left child */
        if(current_key->left_child == NULL)
        {
            /* If no child, the key was not found */
            return NULL;
        }

        /* If there is a left child, recursively call the function on the
         * left child.
         */
        return search_tree(current_key->left_child, key);
    }
}

/* This is passed a nonfull parent ad a full child and performs the split
 * function. This splits the child into left, right and middle keys. 
 * The middle key is placed in the parent node in the appropriate location.
 * The left keys are then pointed to by the middle key.
 * The right keys are then pointed to by the key next to the middle key in 
 * the parent node.
 */
void splitChild(multimap_node *parent, multimap_node *child)
{
    // printf("In Split Child\n");
    // printf("\n\nParent Node\n");
    // print_keys_in_node(parent);
    // printf("\n\nChild Node\n");
    // print_keys_in_node(child);
    /* This ensures we are working with a nonfull parent and a full child */
    assert(parent->num_keys != FULL && child->num_keys == FULL);

    /* Create a new node and other variables to help restructure */
    multimap_node *newNode = alloc_mm_node();

    /* This is the number of keys in the new node. */
    newNode->num_keys = MIN_FACTOR - 1;

    /* This is the key from the child that will be added to the parent */
    multimap_key *middleKey;

    /* This holds the last key in the child. */
    multimap_key *movedKey = child->keys_tail;

    /* The new nodes last key is the movedKey value */
    newNode->keys_tail = movedKey;

    int i = 1;

    /* Figures out which keys in the current child node gets moved */
    while(i < (MIN_FACTOR - 1))
    {
        movedKey = movedKey->prev;
        i = i + 1;
    } 

    /* Middle key is the key right before the one that was moved. */
    middleKey = movedKey->prev;
    movedKey->prev = NULL;

    /* newNode's first key is the new movedKey. Adjust the pointers to 
     * and from the movedKey.
     */
    newNode->keys = movedKey;

    /* Update the child's last key. */
    child->keys_tail = middleKey->prev;
    (child->keys_tail)->next = NULL;

    /* Our updated child's number of keys becomes MIN_FACTOR - 1 */
    child->num_keys = MIN_FACTOR - 1;

    middleKey->next = NULL;
    middleKey->prev = NULL;

    /* Adust the node_child. If child had a node_child, make it the 
     * node_child of the new node and make child's node_child NULL.
     */
    if(child->node_child != NULL)
    {
        newNode->node_child = child->node_child;
        child->node_child = NULL;
    }

    /* Now we have to insert the middle key into the parent node. */
    multimap_key *currKey = parent->keys;

    /* There are no keys in the parent */
    if(currKey == NULL)
    {
        parent->num_keys = 1;
        parent->keys = middleKey;
        parent->keys_tail = middleKey;
    }
    /* Check if the node should be inserted before the first key. */
    else if(middleKey->key < currKey->key)
    {
        /* Inserted before the first key. Updated the node->keys value. */
        parent->keys = middleKey;
        middleKey->next = currKey;
        currKey->prev = middleKey;
        parent->num_keys = parent->num_keys + 1;
    }
    else
    {
        while(1)
        {
            /* If we find the key that's bigger than the one we want to input
             * we add the input before it.
             */
            if(middleKey->key < currKey->key)
            {
                /* Update all the pointers */
                multimap_key *tempPrev = currKey->prev;
                middleKey->prev = tempPrev;
                middleKey->next = currKey;
                currKey->prev = middleKey;
                tempPrev->next = middleKey;
                parent->num_keys = parent->num_keys + 1;
                break;
            }
            else
            {
                /* Check if we need to add the key to the end of the keys list*/
                if(currKey->next == NULL)
                {
                    /* Inserted after the last key. Updated the node->keys_tail
                     * value.
                     */
                    currKey->next = middleKey;
                    middleKey->prev = currKey;
                    parent->keys_tail = middleKey;
                    parent->num_keys = parent->num_keys + 1;
                    break;
                }

            }

            /* If we haven't broken out of the loop, we haven't inserted.
             * Thus, we update the current key.
             */
            currKey = currKey->next;
        }
    }

    /* If the middleKey has a child, that child becomes the child of the
     * child node.
     */
    if(middleKey->left_child != NULL)
    {
        child->node_child = middleKey->left_child;
    }

    /* The middle key's child becomes the old child. */
    middleKey->left_child = child;

    /* If middle is not the only key, the next key's child becomes
     * the new node.
     */
    if(middleKey->next != NULL)
    {
        (middleKey->next)->left_child = newNode;
    }

    /* If it is the only key, then update the parent's child node. */
    else
    {
        parent->node_child = newNode;
    }

    // printf("\n\nEND OF SPLIT NODE\n");
    // printf("\n\nParent: \n");
    // print_keys_in_node(parent);
    // printf("\n\nChild: \n");
    // print_keys_in_node(child);
    // printf("\nTEST CHILD: \n");
    // print_keys_in_node((parent->keys)->left_child);
    // printf("\nPARENT NODE CHILD: \n");
    // print_keys_in_node(parent->node_child);
    // printf("NUM KEYS PARENT: %d\n", parent->num_keys);
    // printf("NUM KEYS LEFT CHILD: %d\n", child->num_keys);
    // printf("NUM KEYS RIGHT CHILD: %d\n", (parent->node_child)->num_keys);

}

/* This function takes in a nonfull node and inserts the key into it or one
 * of it's children.
 */
multimap_key * insertHelper(multimap_node *node, int key)
{
    // printf("\nInsert Helper Called on Node: \n");
    // print_keys_in_node(node);
    assert(node->num_keys != FULL);

    /* The current key being looked at. This is initially set to the first
     * key in the node passed in.
     */
    multimap_key *current_key = node->keys;
    multimap_key *new_key;

    /* Initialize iterators and flags */
    int lastKey = 0;

    /* While the current key isn't null and while the key being looked for
     * is greater than the current key, continue updating the current key */

    while(key > current_key->key)
    {
        /* Check if we are looking at the last key. If we are, then
         * raise the flag that says we can't insert the key before the
         * last current key and then break.
         */
        if(current_key->next == NULL)
        {
            lastKey = 1;
            break;
        }

        current_key = current_key->next;
    }

    /* If we get to the last of the keys in the nonfull node, we just have
     * add the key to the end of the node list.
     */
    if(lastKey == 1)
    {
        /* Created a new key and inserted it after the current last key.
         * Updated the node->keys_tail pointer.
         */

        new_key = alloc_mm_key();
        new_key->key = key;
        current_key->next = new_key;
        new_key->prev = current_key;
        node->keys_tail = new_key;
        node->num_keys = node->num_keys + 1;
        return new_key;
    }

    /* If the child of the current node is NULL, then this is a leaf node.
     * Thus, we shift over the keys to make room for this new key.
     */
    if(current_key->left_child == NULL)
    {
        // printf("INSERTING %d INTO NODE\n", key);
        // print_keys_in_node(node);
        /* Initialize new key */
        new_key = alloc_mm_key();
        new_key->key = key;

        current_key = node->keys;

        /* Check if the key should be inserted before the first key */
        if(new_key->key < current_key->key)
        {
            // printf("\nInserting it before\n");
            /*Inserted before the first key. Updated the node->keys value */
            node->keys = new_key;
            new_key->next = current_key;
            current_key->prev = new_key;
            node->num_keys = node->num_keys + 1;
        }
        else
        {
            while(1)
            {
                /* If we find the key that's bigger than the one we want to 
                 * input, we add the input before it.
                 */
                 if(new_key->key < current_key->key)
                {
                    // printf("\nInserting it inside\n");
                    /* Update all the pointers */
                    multimap_key *tempPrev = current_key->prev;
                    new_key->prev = tempPrev;
                    new_key->next = current_key;
                    current_key->prev = new_key;
                    tempPrev->next = new_key;
                    node->num_keys = node->num_keys + 1;
                    break;
                }

                current_key = current_key->next;
                if(current_key == NULL)
                {
                    break;
                }
            }
        }

        // printf("\nAFTER\n");
        // print_keys_in_node(node);
        return new_key;
    }

    /* If there is a child node for the current node, we don't add this number
     * to the current node.
     */
    else
    {
        multimap_node *child = current_key->left_child;
        if(child->num_keys == FULL)
        {
            /* If the child is full, we have the case of a nonfull parent and
             * full child. Thus, we call splitChild on the node and the
             * child.
             */
            splitChild(node, child);
        }

        /* Call insert helper on the now not full child and the key */
        return insertHelper(child, key);
    }
}

multimap_key * insertKey(multimap *mm, int key)
{
    // printf("\nInsert Key Called on Node: \n");
    // print_keys_in_node(root);
    /* If the entire multimap is empty, the root will be NULL.
     * Therefore, we create a node and initialize it with this
     * key.
     */
    multimap_key *newKey;
    multimap_node *node;

    /* If the node is NULL it means the map has no nodes. */
    if(mm->root == NULL)
    {
        /* Create a key first */
        newKey = alloc_mm_key();
        newKey->key = key;

        /* Create a new node */
        node = alloc_mm_node();
        node->keys = newKey;
        node->keys_tail = newKey;
        node->num_keys = 1;

        /* Change the root pointer of the map. */
        mm->root = node;

        return newKey;
    }
    
    node = mm->root;

    /* If the current node is full of keys */
    if(node->num_keys == FULL)
    {
        /* Make a new node that will be the new root. It will point to
         * the current root.
         */
        multimap_node *newNode = alloc_mm_node();
        newNode->node_child = node;
        newNode->num_keys = 0;

        mm->root = newNode;

        /* Then we call splitChild on this empty parent and the full node */
        splitChild(newNode, node);

        /* We can then call the insertHelper on the nonfull newNode. */
        return insertHelper(newNode, key);
    }
    else
    {
        /* If it isn't full, we can call the insertHelper on the nonfull node */
        return insertHelper(node, key);
    }
}

/* This helper function frees all values in a multimap key's value-list. */
void free_multimap_values(multimap_value *values)
{
    while(values != NULL) 
    {
        multimap_value *next = values->next;
#ifdef DEBUG_ZERO
        /* Clear out what we are about to free, to expose issues quickly. */
        bzero(values, sizeof(multimap_value));
#endif
        free(values);
        values = next;
    }
}

/* This helper function frees all keys in a multimap node's key_list */
void free_multimap_keys(multimap_key *keys)
{
    while(keys != NULL)
    {
        multimap_key *next = keys->next;

        /* Free the child node first */
        free_multimap_node(keys->left_child);
        /* Free the list of values */
        free_multimap_values(keys->values);

#ifdef DEBUG_ZERO
    /* Clear out what we are about to free, to expose issues quickly. */
    bzero(keys, sizeof(multimap_key));
#endif
        free(keys);
        keys = next;

    }
}

/* This helper function frees a multimap node, including its children and
 * value-list.
 */
void free_multimap_node(multimap_node *node) 
{
    if (node == NULL)
        return;

    /* Free the children first. */
    free_multimap_node(node->node_child);

    /* Free the list of keys. */
    free_multimap_keys(node->keys);

#ifdef DEBUG_ZERO
    /* Clear out what we are about to free, to expose issues quickly. */
    bzero(node, sizeof(multimap_node));
#endif
    free(node);
}




/* THE FOLLOWING ARE THE FUNCTIONS AVAILABLE OUTSIDE OF THIS FILE. 
 * DO NOT CHANGE THEIR HEADERS.
 */

/* Initialize a multimap data structure. */
multimap * init_multimap() {
    multimap *mm = malloc(sizeof(multimap));
    mm->root = NULL;
    return mm;
}

/* Release all dynamically allocated memory associated with the multimap
 * data structure.
 */
void clear_multimap(multimap *mm) {
    // printf("-----------------------------------------------------\n");
    // printf("Printing final root keys\n");
    // printf("-----------------------------------------------------\n");
    // print_keys_in_node(mm->root);
    // print_keys_in_node((mm->root)->node_child);
    assert(mm != NULL);
    free_multimap_node(mm->root);
    mm->root = NULL;
}

/* Adds the specified (key, value) pair to the multimap. */
void mm_add_value(multimap *mm, int key, int value) 
{
    // printf("\nADDING A VALUE\n");

    multimap_value *new_value;
    multimap_key *currentKey;

    /* Ensure the map has been initialized */
    assert(mm != NULL);

    /* If there is no root, we call insertKey on the NULL node, which makes
     * a root node with this key. 
     */
    if(mm->root == NULL)
    {
        currentKey = insertKey(mm, key);
    }
    else
    {
        /* We search the tree for the key. */
        currentKey = search_tree(mm->root, key);

        /* If the result of the search is NULL, it didn't find the key. Thus,
         * we insert it.
         */
        if(currentKey == NULL)
        {
            currentKey = insertKey(mm, key);
        }
    }

    /* Now, we add the value to the key's values. */

    multimap_value *lastArr;

    /* If the key doesn't have any values, we give it values. */
    if(currentKey->values == NULL)
    {
        new_value = malloc(sizeof(multimap_value));
        new_value->value[0] =  value;
        new_value->currentSize = 1;
        new_value->next = NULL;
        currentKey->values = new_value;
        currentKey->values_tail = new_value;
    }
    else
    {
        /* If it does have values, we find the values array with available
         * space.
         */
        lastArr = currentKey->values_tail;

        /* If it's current last array is full, we make a new one. */
        if(lastArr->currentSize == ARRAY_SIZE)
        {
            new_value = malloc(sizeof(multimap_value));
            new_value->value[0] = value;
            new_value->next = NULL;
            lastArr->next = new_value;
            currentKey->values_tail = new_value;
        }

        /* Else, we just add to the existing last array. */
        else
        {
            lastArr->value[lastArr->currentSize] = value;
            lastArr->currentSize = lastArr->currentSize + 1;
        }
    }

}

/* Returns nonzero if the multimap contains the specified key-value, zero
 * otherwise.
 */
 int mm_contains_key(multimap *mm, int key)
 {
    multimap_key *result = search_tree(mm->root, key);
    if(result == NULL)
    {
        return 0;
    }
    return 1;
 }

/* Returns nonzero if the multimap contains the specified (key, value) pair,
 * zero otherwise.
 */
 int mm_contains_pair(multimap *mm, int key, int value)
 {
    multimap_key *resultKey;
    multimap_value *curr;
    int i;

    resultKey = search_tree(mm->root, key);
    if(resultKey == NULL)
    {
        return 0;
    }

    curr = resultKey->values;
    while(curr != NULL)
    {
        for(i = 0; i < curr->currentSize; i++)
        {
            if((curr->value)[i] == value)
            {
                return 1;
            }
        }

        curr = curr->next;
    }

    return 0;
 }

 /* This helper function is used by mm_traverse() to traverse every pair within
 * the multimap.
 */
void mm_traverse_helper(multimap_node *node, void (*f)(int key, int value))
{
    multimap_key *currKey;
    multimap_value *currValue;
    int i = 0;

    /* If the inputted node is NULL, then there are no more pairs to go through
     * in this direction.
     */
    if(node == NULL)
    {
        return;
    }
    /* Else, we have to look at each key in this node, the node child of
     * each key and the node's overall child.
     */
    else
    {
        /* First we call traverse helper on the node's overall child. */
        mm_traverse_helper(node->node_child, f);

        // Now iterate over every key in this current node 
        currKey = node->keys;
        while(currKey != NULL)
        {
            /* Go through all the values in this key. */
            currValue = currKey->values;
            while(currValue != NULL)
            {
                for(i = 0; i < currValue->currentSize; i++)
                {
                    f(currKey->key, (currValue->value)[i]);       
                }

                currValue = currValue->next;
            }


            /* Then call the mm_traverse_helper on the child node of the key. */

            mm_traverse_helper(currKey->left_child, f);

            currKey = currKey->next;
        }

    }
    
}

/* Performs an in-order traversal of the multimap, passing each (key, value)
 * pair to the specified function.
 */
void mm_traverse(multimap *mm, void (*f)(int key, int value)) {
    mm_traverse_helper(mm->root, f);
    // printf("Failed\n");
}

void print_keys_in_node(multimap_node *node)
{
    printf("Keys in Node: \n");
    multimap_key * currentKey;
    if(node == NULL)
    {
        return;
    }
    else
    {
        currentKey = node->keys;
        while(currentKey != NULL)
        {
            printf("%d ", currentKey->key);
            currentKey = currentKey->next;
        }
        printf("\n");
        printf("Number of keys in node: %d\n", node->num_keys);
    }
}