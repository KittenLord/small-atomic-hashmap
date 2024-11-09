#include <stdlib.h>
#include <stdio.h>
#include "atomichashmap.c"

int main() {

    struct AtomicHashmap hm = createHM();
    setHM(&hm, "ABOBA", 5, "TEST", 4);
    setHM(&hm, "ABOBB", 5, "TEST", 4);
    setHM(&hm, "ABOBC", 5, "TEST", 4);

    printf("%d\n", hm.count);

    removeHM(&hm, "ABOBB", 5);

    printf("%d\n", hm.count);

    return 0;
}
