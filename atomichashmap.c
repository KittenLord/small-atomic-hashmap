#ifndef KITTENLORD_ATOMIC_HASHMAP__
#define KITTENLORD_ATOMIC_HASHMAP__

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// TODO: obviously needs rebalancing functionality, but ill add it later lol

// https://github.com/fabiogaluppo/fnv/blob/main/fnv64.hpp
// from here
uint64_t shiftSum(uint64_t v) {
    return (v << 1) + (v << 4) + (v << 5) + (v << 7) + (v << 8) + (v << 40);
}

uint64_t fnv64hash(uint8_t *data, size_t len) {
    uint64_t h = 0xcbf29ce484222325ULL;
    while (len--) {
        h += shiftSum(h);
        h ^= (uint64_t)(*data++);
    }
    return h;
}
// to here

struct AtomicHashmapNode {
    uint8_t                     *key;
    size_t                      keyLen;

    uint8_t                     *data;
    size_t                      dataLen;

    struct AtomicHashmapNode    *next;
};

struct AtomicHashmap {
    _Atomic bool                locked;

    size_t                      capacity;
    
    size_t                      maxChain;
    size_t                      count;

    struct AtomicHashmapNode    **nodes;
};

void trySetMaxChain(struct AtomicHashmap *hm, size_t value) {
    if(hm->maxChain < value) hm->maxChain = value;
}

void freeHMNode(struct AtomicHashmapNode *node) {
    free(node->key);
    free(node->data);
    free(node);
}

bool nodeMatchesKey(struct AtomicHashmapNode *node, uint8_t *key, size_t keyLen) {
    return node->keyLen == keyLen && !memcmp(node->key, key, keyLen);
}

bool removeHM(struct AtomicHashmap *hm, uint8_t *key, size_t keyLen) {
    while(hm->locked) {}
    hm->locked = true;
    bool result = false;

    size_t hash = fnv64hash(key, keyLen);
    size_t index = hash % hm->capacity;

    struct AtomicHashmapNode *node = hm->nodes[index];
    if(nodeMatchesKey(node, key, keyLen)) {
        hm->nodes[index] = node->next;
        freeHMNode(node);
        result = true;
        goto cleanup;
    }

    while(node->next) {
        struct AtomicHashmapNode *next = node->next;
        if(nodeMatchesKey(next, key, keyLen)) {
            node->next = next->next;
            freeHMNode(next);
            result = true;
            goto cleanup;
        }
    }

    result = false;
    goto cleanup;

cleanup:
    hm->count -= result;
    hm->locked = false;
    return result;
}

uint8_t *getHM(struct AtomicHashmap *hm, uint8_t *key, size_t keyLen, size_t *dataLen) {
    while(hm->locked) {}
    hm->locked = true;

    size_t hash = fnv64hash(key, keyLen);
    size_t index = hash % hm->capacity;

    struct AtomicHashmapNode *node = hm->nodes[index];
    while(node) {
        if(nodeMatchesKey(node, key, keyLen)) {
            if(dataLen) *dataLen = node->dataLen;
            goto cleanup;
        }

        node = node->next;
    }

cleanup:
    hm->locked = false;
    if(node) return node->data;
    return NULL;
}

// #define setHM_NX(hm, key, data, dataLen) setHM(hm, key, strlen(key), data, dataLen)
// #define setHM_FF(hm, key, data) setHM(hm key, sizeof(key), data, sizeof(data))
void setHM(struct AtomicHashmap *hm, uint8_t *key, size_t keyLen, uint8_t *data, size_t dataLen) {
    while(hm->locked) {}
    hm->locked = true;

    size_t hash = fnv64hash(key, keyLen);
    size_t index = hash % hm->capacity;

    size_t maxChain = 1;
    struct AtomicHashmapNode **insertNode = &(hm->nodes[index]);

    // definitely doesnt exist
    if(!*insertNode) { goto insert; }

    do {
        struct AtomicHashmapNode *node = *insertNode;
        if(nodeMatchesKey(node, key, keyLen)) {
            free(node->data);
            node->data = malloc(dataLen);
            node->dataLen = dataLen;
            memcpy(node->data, data, dataLen);
            goto cleanup;
        }

        insertNode = &(node->next);
        maxChain++;
    }
    while(*insertNode);
    goto insert;

insert:
    struct AtomicHashmapNode *node = malloc(sizeof(struct AtomicHashmapNode));
    *node = (struct AtomicHashmapNode){
        .key = malloc(keyLen),
        .keyLen = keyLen,
        .data = malloc(dataLen),
        .dataLen = dataLen,
        .next = NULL
    };
    memcpy(node->key, key, keyLen);
    memcpy(node->data, data, dataLen);
    *insertNode = node;
    trySetMaxChain(hm, maxChain);
    hm->count++;

cleanup:
    hm->locked = false;
}

#define HM_INITCAPACITY 32
#define createHM() createCapHM(HM_INITCAPACITY)
// TODO: maybe malloc it and return a pointer? idk
struct AtomicHashmap createCapHM(size_t capacity) {
    return (struct AtomicHashmap){ 
        .locked = false, 
        .capacity = capacity, 
        .maxChain = 0, 
        .nodes = calloc(capacity, sizeof(struct AtomicHashmapNode *))
    };
}

#else
#endif
