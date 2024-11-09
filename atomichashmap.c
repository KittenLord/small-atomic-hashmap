#ifndef KITTENLORD_ATOMIC_HASHMAP__
#define KITTENLORD_ATOMIC_HASHMAP__

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

uint64_t shiftSum(uint64_t v) {
    return (v << 1) + (v << 4) + (v << 5) + (v << 7) + (v << 8) + (v << 40);
}

// https://github.com/fabiogaluppo/fnv/blob/main/fnv64.hpp
uint64_t fnv64hash(uint8_t *data, size_t len) {
    uint64_t h = 0xcbf29ce484222325ULL;
    while (len--) {
        h += shiftSum(h);
        h ^= (uint64_t)(*data++);
    }
    return h;
}

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
};

#else
#endif
