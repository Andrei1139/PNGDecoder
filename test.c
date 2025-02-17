#include "image.h"

void print_bytes(unsigned int x) {
    byte b;
    for (int i = 0; i < 4; ++i) {
        b = ((byte *)&x)[i];
        printf("%02x ", b);
    }
    printf("\n");
}

int main() {
    unsigned int x;
    scanf("%u", &x);
    print_bytes(x);
    print_bytes(SWITCH_ENDIANS(x));

}