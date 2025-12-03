#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node
{
    int key;
    char data[50];
    struct Node *prev;
    struct Node *next;
} Node;

typedef struct Hash
{
    int key;
    Node *node;
    struct Hash *next;
} Hash;

typedef struct
{
    int capacity;
    int size;
    Node *head;
    Node *tail;
    Hash **map;
} LRUCache;

#define HASH_SIZE 1000

LRUCache *createCache(int capacity)
{
    LRUCache *cache = (LRUCache *)malloc(sizeof(LRUCache));
    cache->capacity = capacity;
    cache->size = 0;

    cache->head = NULL;
    cache->tail = NULL;

    cache->map = (Hash **)malloc(sizeof(Hash *) * HASH_SIZE);

    for (int i = 0; i < HASH_SIZE; i++)
    {
        cache->map[i] = NULL;
    }
    return cache;
}

int hashFunction(int key)
{
    return key % HASH_SIZE;
}

Node *hashGet(LRUCache *cache, int key)
{
    int index = hashFunction(key);
    Hash *entry = cache->map[index];

    while (entry != NULL)
    {
        if (entry->key == key)
            return entry->node;

        entry = entry->next;
    }
    return NULL;
}

void hashPut(LRUCache *cache, int key, Node *node)
{
    int index = hashFunction(key);
    Hash *entry = cache->map[index];

    // key exist then update
    while (entry != NULL)
    {
        if (entry->key == key)
        {
            entry->node = node;
            return;
        }
        entry = entry->next;
    }

    Hash *newEntry = (Hash *)malloc(sizeof(Hash));
    newEntry->key = key;
    newEntry->node = node;
    newEntry->next = cache->map[index];
    cache->map[index] = newEntry;
}

void hashRemove(LRUCache *cache, int key)
{
    int index = hashFunction(key);
    Hash *curr = cache->map[index];
    Hash *prev = NULL;

    while (curr != NULL)
    {
        if (curr->key == key)
        {
            if (prev == NULL)
                cache->map[index] = curr->next;

            else
                prev->next = curr->next;

            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void addNodeToFront(LRUCache *cache, Node *node)
{
    node->prev = NULL;
    node->next = cache->head;

    if (cache->head != NULL)
        cache->head->prev = node;

    cache->head = node;

    if (cache->tail == NULL)
        cache->tail = node;
}

void removeNode(LRUCache *cache, Node *node)
{
    if (node->prev != NULL)
        node->prev->next = node->next;

    else
        cache->head = node->next; // removed head

    if (node->next != NULL)
        node->next->prev = node->prev;

    else
        cache->tail = node->prev; // removed tail
}

void moveToFront(LRUCache *cache, Node *node)
{
    if (cache->head == node)
        return;

    removeNode(cache, node);
    addNodeToFront(cache, node);
}

Node *removeLastNode(LRUCache *cache)
{
    Node *tail = cache->tail;
    if (tail == NULL)
        return NULL;

    removeNode(cache, tail);
    return tail;
}

char *get(LRUCache *cache, int key)
{
    Node *node = hashGet(cache, key);

    if (node == NULL)
    {
        return NULL;
    }

    moveToFront(cache, node);

    return node->data;
}

void put(LRUCache *cache, int key, char *value)
{
    Node *existingNode = hashGet(cache, key);
    // if exists , move to mru
    if (existingNode != NULL)
    {
        strcpy(existingNode->data, value);
        moveToFront(cache, existingNode);
        return;
    }

    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->key = key;
    strcpy(newNode->data, value);
    newNode->prev = NULL;
    newNode->next = NULL;

    // if full , delete lru
    if (cache->size == cache->capacity)
    {
        Node *lru = removeLastNode(cache);
        hashRemove(cache, lru->key);
        free(lru);
        cache->size--;
    }

    addNodeToFront(cache, newNode);
    hashPut(cache, key, newNode);
    cache->size++;
}

int main()
{
    LRUCache *cache = NULL;
    char operation[20];

    while (1)
    {
        scanf("%s", operation);

        if (strcmp(operation, "createCache") == 0)
        {
            int size;
            scanf("%d", &size);
            cache = createCache(size);
        }

        else if (strcmp(operation, "put") == 0)
        {
            int key;
            char value[50];
            scanf("%d %s", &key, value);
            put(cache, key, value);
        }

        else if (strcmp(operation, "get") == 0)
        {
            int key;
            scanf("%d", &key);
            char *result = get(cache, key);

            if (result == NULL)
                printf("NULL\n");

            else
                printf("%s\n", result);
        }

        else if (strcmp(operation, "exit") == 0)
        {
            break;
        }
    }
    return 0;
}
