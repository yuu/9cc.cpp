#include <cstdio>
#include "cmdline.h"

int main(int argc, char **argv) {
    cmdline::parser p;
    p.add<int>("value", 'v', "input value", true, 0);
    p.parse_check(argc, argv);

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    printf("  mov rax, %d\n", p.get<int>("value"));
    printf("  ret\n");

    return 0;
}
