#include <cstdio>
#include <string>
#include <vector>
#include "cmdline.h"

enum {
    TK_NUM = 256,
};

struct Token {
    int type;

    int64_t val;
    char *input;  // about error
};

char *user_input;
std::vector<Token> tokens;

#if 0
void error(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}
#endif

void error_at(char *loc, const char *msg) {
    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ %s\n", msg);
    exit(1);
}

void tokenize() {
    char *p = user_input;

    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-') {
            tokens.emplace_back(Token{*p, -1, p});
            p++;
            continue;
        }

        if (isdigit(*p)) {
            tokens.emplace_back(Token{TK_NUM, strtol(p, &p, 10), p});
            continue;
        }

        error_at(p, "Failed tokenize");
    }
}

int main(int argc, char **argv) {
    cmdline::parser p;
    p.add<std::string>("expr", 'e', "input expr", true, "");
    p.parse_check(argc, argv);

    {
        auto tmp = p.get<std::string>("expr");
        user_input = tmp.data();
    }
    tokenize();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    auto iter = tokens.begin();
    if (iter->type != TK_NUM) {
        error_at(iter->input, "not num type");
    }
    printf("  mov rax, %lld\n", iter->val);

    iter++;
    for (;iter != tokens.end(); iter++) {
        if (iter->type == '+') {
            iter++;
            if (iter->type != TK_NUM) {
                error_at(iter->input, "unexpected num type");
            }
            printf("  add rax, %lld\n", iter->val);
            continue;
        }

        if (iter->type == '-') {
            iter++;
            if (iter->type != TK_NUM) {
                error_at(iter->input, "unexpected num type");
            }
            printf("  sub rax, %lld\n", iter->val);
            continue;
        }

        error_at(iter->input, "Failed unexpected token");
    }

    printf("  ret\n");

    return 0;
}
