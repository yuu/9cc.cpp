#include <cstdio>
#include <memory>
#include <string>
#include <vector>
#include <cassert>
#include "cmdline.h"

enum {
    TK_NUM = 256,
};

struct Token {
    int type;

    int64_t val;
    char *input;  // about error
};

enum {
      ND_MUL,
      ND_TERM,

      ND_NUM = 200,
};
struct Node {
    std::shared_ptr<Node> lhs;
    std::shared_ptr<Node> rhs;

    int ty;
    int val;

    Node() = delete;

    Node(int ty, std::shared_ptr<Node> lhs, std::shared_ptr<Node> rhs)
        : lhs(lhs)
        , rhs(rhs)
        , ty(ty)
        , val(0) {}

    Node(int val_)
        : lhs(nullptr)
        , rhs(nullptr)
        , ty(ND_NUM)
        , val(val_) {}
};

char *user_input;
std::vector<Token> tokens;
using TokenIter = decltype(tokens)::iterator;

bool consume(int ty, TokenIter &iter);
std::shared_ptr<Node> term(TokenIter &iter);
std::shared_ptr<Node> mul(TokenIter &iter);
std::shared_ptr<Node> expr(TokenIter &iter);

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

        if (*p == '*' || *p == '/') {
            tokens.emplace_back(Token{*p, -1, p});
            p++;
            continue;
        }

        if (*p == '+' || *p == '-') {
            tokens.emplace_back(Token{*p, -10, p});
            p++;
            continue;
        }

        if (isdigit(*p)) {
            const auto num = strtol(p, &p, 10);
            const auto t = Token{TK_NUM, num, p};
            tokens.emplace_back(t);
            continue;
        }

        error_at(p, "Failed tokenize");
    }
}

bool consume(int ty, TokenIter &iter) {
    if (iter->type != ty) {
        // printf("%s iter=%c or %d\n", __func__, iter->type, iter->type);
        return false;
    }

    ++iter;
    // printf("%s next iter=%c or %d\n", __func__, iter->type, iter->type);

    return true;
}

std::shared_ptr<Node> term(TokenIter &iter) {
    // printf("%s iter=%c or %d\n", __func__, iter->type, iter->type);
    if (consume('(', iter)) {
        auto node = expr(iter);
        if (!consume(')', iter))
            error_at(iter->input, ") is not found");

        return node;
    }

    if (iter->type == TK_NUM) {
        auto ret = std::make_shared<Node>(iter->val);
        ++iter;
        return ret;
    }

    error_at(iter->input, "not numlic or (");
}

std::shared_ptr<Node> mul(TokenIter &iter) {
    // printf("%s iter=%c or %d\n", __func__, iter->type, iter->type);
    auto node = term(iter);

    for (;;) {
        if (consume('*', iter))
            node = std::make_shared<Node>('*', node, term(iter));
        else if (consume('/', iter))
            node = std::make_shared<Node>('/', node, term(iter));

        return node;
    }
}

std::shared_ptr<Node> expr(TokenIter &iter) {
    // printf("%s iter=%c or %d\n", __func__, iter->type, iter->type);
    auto node = mul(iter);

    for (;;) {
        if (consume('+', iter))
            node = std::make_shared<Node>('+', node, mul(iter));
        else if (consume('-', iter))
            node = std::make_shared<Node>('-', node, mul(iter));

        return node;
    }
}

void gen(std::weak_ptr<Node> node) {
    /*
    auto iter = tokens.begin();
    if (iter->type != TK_NUM) {
        error_at(iter->input, "not num type");
    }
    printf("  mov rax, %lld\n", iter->val);

    iter++;
    for (; iter != tokens.end(); iter++) {
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
    */
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
    // for (auto &&t : tokens) {
    //     printf("type=%c val=%ld\n", t.type, t.val);
    // }
    auto iter = tokens.begin();
    auto node = expr(iter);

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    gen(node);

    printf("  pop rax\n");
    printf("  ret\n");

    return 0;
}
