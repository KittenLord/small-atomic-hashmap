#include <stdlib.h>
#include <stdio.h>
#include "atomichashmap.c"

int main() {

    struct AtomicHashmap hm = createHM();
    insertHM(&hm, "ABOBA", 5, "TEST", 4);
    insertHM(&hm, "ZUZ", 3, "ASDASD", 6);
    insertHM(&hm, "ABC", 3, "ASDASD", 6);
    insertHM(&hm, "CDE", 3, "ASDASD", 6);
    insertHM(&hm, "EFG", 3, "ASDASD", 6);
    insertHM(&hm, "A", 1, "ASDASD", 6);
    insertHM(&hm, "B", 1, "ASDASD", 6);
    insertHM(&hm, "C", 1, "ASDASD", 6);
    insertHM(&hm, "D", 1, "ASDASD", 6);
    insertHM(&hm, "E", 1, "ASDASD", 6);
    insertHM(&hm, "F", 1, "ASDASD", 6);
    insertHM(&hm, "G", 1, "ASDASD", 6);
    insertHM(&hm, "H", 1, "ASDASD", 6);
    insertHM(&hm, "I", 1, "ASDASD", 6);

    printf("%s\n", getHM(&hm, "ZUZ", 3, NULL));
    printf("%s\n", getHM(&hm, "ABOBA", 5, NULL));

    printf("%d\n", hm.maxChain);
    printf("%d\n", hm.count);

    return 0;
}
