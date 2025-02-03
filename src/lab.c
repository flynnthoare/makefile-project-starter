#include <stdlib.h>
#include <stdio.h>
#include "lab.h"

// Removes unused compare_to and destroy_data functions to get rid of warnings

/**
    * @brief Create a new list with callbacks that know how to deal with the data that
    * list is storing. The caller must pass the list to list_destroy when finished to
    * free any memory that was allocated.
    *
    * @param destroy_data Function that will free the memory for user supplied data
    * @param compare_to Function that will compare two user data elements
    * @return struct list* pointer to the newly allocated list.
    */
list_t *list_init(void (*destroy_data)(void *), int (*compare_to)(const void *, const void *))
{
    list_t *list = (list_t *)malloc(sizeof(list_t));
    if (list == NULL) {
        return NULL;
    }

    //assign functions
    list->destroy_data = destroy_data;
    list->compare_to = compare_to;

    //init sentinel node
    list->head = (node_t *)malloc(sizeof(node_t));
    if (list->head == NULL) {  // Check allocation failure and free list if failed
        free(list);
        return NULL;
    }
    list->head->data = NULL;
    list->head->next = list->head;
    list->head->prev = list->head;

    list->size = 0;

    return list;
}

/**
* @brief Destroy the list and and all associated data. This functions will call
* destroy_data on each nodes data element.
*
* @param list a pointer to the list that needs to be destroyed
*/
void list_destroy(list_t **list)
{
    if (list == NULL || *list == NULL) {
        return;
    }

    //set current to node after sentinel node
    node_t *current = (*list)->head->next;

    while(current != (*list)->head) {
        //store next node
        node_t *next = current->next;
        //free the nodes data
        if((*list)->destroy_data) {
            (*list)->destroy_data(current->data);
        }
        //free the node itself
        free(current);
        //move to next node
        current = next;
    }

    //free sentinel node
    free((*list)->head);

    //free actual list
    free(*list);
    *list = NULL;
}

/**
* Adds data to the front of the list
*
* @param list a pointer to an existing list.
* @param data the data to add
* @return A pointer to the list
*/
list_t *list_add(list_t *list, void *data)
{
    //function ends if allocation fails
    if (list == NULL) {
        return NULL;
    }

    //allocate memory for new node
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    if (new_node == NULL) {
        return NULL; // Handle allocation failure
    }

    new_node->data = data;

    new_node->next = list->head->next;
    new_node->prev = list->head;

    list->head->next->prev = new_node;
    list->head->next = new_node;

    list->size++;

    return list;
}

/**
* @brief Removes the data at the specified index. If index is invalid
* then this function does nothing and returns NULL
*
* @param list The list to remove the element from
* @param index The index
* @return void* The data that was removed or NULL if nothing was removed
*/
void *list_remove_index(list_t *list, size_t index)
{
    //check if list/index is valid
    if(list == NULL || index>= list->size) {
        return NULL;
    }

    //traverse to the node we want to remove
    node_t *current = list->head->next;
    for (size_t i = 0; i < index; i++) {
        current = current->next;
    }

    //remove the node
    current->prev->next = current->next;
    current->next->prev = current->prev;

    if (list->size == 1) {
        list->head->next = list->head;
        list->head->prev = list->head;
    }

    //extract data
    void *data = current->data;

    //free the node
    free(current);

    //update list size
    list->size--;

    return data;
}

/**
* @brief Search for any occurrence of data from the list.
* Internally this function will call compare_to on each item in the list
* until a match is found or the end of the list is reached. If there are
* multiple copies of the same data in the list the first one will be returned.
*
* @param list the list to search for data
* @param data the data to look for
* @return The index of the item if found or -1 if not
*/
int list_indexof(list_t *list, void *data)
{
    //validate input/list
    if (list == NULL || list->size == 0) {
        return -1; 
    }

    node_t *current = list->head->next;
    int index = 0;

    while (current != list->head) {
        
        if (list->compare_to(current->data, data) == 0) {
            return index;
        }
        current = current->next;
        index++;
    }

    //data not found
    return -1;
}