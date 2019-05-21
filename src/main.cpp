#include <cstdio>
#include <string>
#include "cmdline.h"

int main(int argc, char **argv) {
    cmdline::parser p;
    p.add<std::string>("expr", 'e', "input expr", true, "");
    p.parse_check(argc, argv);

    char *expr;
    {
        auto tmp = p.get<std::string>("expr");
        expr = tmp.data();
    }

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    printf("  mov rax, %ld\n", strtol(expr, &expr, 10));
    printf("  ret\n");

    return 0;
}
