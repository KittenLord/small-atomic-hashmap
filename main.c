#include <stdlib.h>
#include <stdio.h>
#include "atomichashmap.c"

int main() {

    struct AtomicHashmap hm = createHM();
    setHM(&hm, "ABOBA", 5, "TEST", 4);
    setHM(&hm, "ABOBB", 5, "TEST", 4);
    setHM(&hm, "ABOBC", 5, "TEST", 4);

    printf("%d\n", fnv64hash("ABOBA", 5) % 32);
    printf("%d\n", fnv64hash("ZUZ", 3) % 32);

    return 0;
}
