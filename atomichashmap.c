#ifndef KITTENLORD_ATOMIC_HASHMAP__
#define KITTENLORD_ATOMIC_HASHMAP__

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

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

uint8_t *getHM(struct AtomicHashmap *hm, uint8_t *key, size_t keyLen, size_t *dataLen) {
    while(hm->locked) {}
    hm->locked = true;

    size_t hash = fnv64hash(key, keyLen);
    size_t index = hash % hm->capacity;

    struct AtomicHashmapNode *node = hm->nodes[index];
    while(node) {
        if(node->keyLen == keyLen && !memcmp(node->key, key, keyLen)) {
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

void insertHM(struct AtomicHashmap *hm, uint8_t *key, size_t keyLen, uint8_t *data, size_t dataLen) {
    while(hm->locked) {}
    hm->locked = true;

    size_t hash = fnv64hash(key, keyLen);
    size_t index = hash % hm->capacity;

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

    size_t maxChain = 1;
    struct AtomicHashmapNode *insertNode = hm->nodes[index];
    if(!insertNode) { hm->nodes[index] = node; trySetMaxChain(hm, maxChain); goto cleanup; }

    while(insertNode->next) {
        insertNode = insertNode->next;
        maxChain++;
    }
    maxChain++;

    insertNode->next = node;
    trySetMaxChain(hm, maxChain);
    goto cleanup;

cleanup:
    hm->count++;
    hm->locked = false;
}

#define HM_INITCAPACITY 32
// TODO: maybe malloc it and return a pointer? idk
struct AtomicHashmap createHM() {
    return (struct AtomicHashmap){ 
        .locked = false, 
        .capacity = HM_INITCAPACITY, 
        .maxChain = 0, 
        .nodes = calloc(HM_INITCAPACITY, sizeof(struct AtomicHashmapNode *))
    };
}

#else
#endif
