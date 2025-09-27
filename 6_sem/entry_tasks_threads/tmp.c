#include <stdio.h>

int main() {
#ifdef _OPENMP
    printf("OpenMP version: %d\n", _OPENMP);
#else
    printf("OpenMP не поддерживается\n");
#endif
    return 0;
}

