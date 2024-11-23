#include <stdlib.h>
#include <stdio.h>
#include "atomichashmap.c"

int main() {

    struct AtomicHashmap hm = createHM_SX(int);

    // TODO: i'm not sure if it is possible to improve, but this looks quite bad
    // maybe in practice it's not that bad though
    int one = 1;
    int two = 2;
    int thr = 3;

    acquireLock(&hm);
    setHM_SX(&hm, &one, "ABOBA", 5);
    setHM_SX(&hm, &two, "TEST", 4);
    setHM_SX(&hm, &thr, "GUG", 3);

    printf("%s\n", getHM_SX(&hm, &one, NULL));
    printf("%s\n", getHM_SX(&hm, &two, NULL));
    printf("%s\n", getHM_SX(&hm, &thr, NULL));
    releaseLock(&hm);

    return 0;
}
