#include <cstdio>

int main() {
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    printf("  mov rax, 42\n");
    printf("  ret\n");

    return 0;
}
